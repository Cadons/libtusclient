/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <iostream>
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <thread>

#include "TusClient.h"
#include "http/HttpClient.h"
#include "model/TUSFile.h"
#include "repository/CacheRepository.h"
#include "model/TUSChunk.h"

using boost::uuids::random_generator;
using TUS::TusClient;
using TUS::TusStatus;
TusClient::TusClient(std::string appName,std::string url, path filePath, int chunkSize) :
    m_appName(appName),
    m_url(url), m_filePath(filePath),
    m_status(TusStatus::READY), m_tempDir(TEMP_DIR), 
    m_httpClient(std::make_unique<TUS::Http::HttpClient>()), 
    CHUNK_SIZE(chunkSize * 1024)
{
    boost::uuids::uuid uuid = random_generator()();
    m_uuid = uuid;
    m_cacheManager = std::make_unique<TUS::CacheRepository>(m_appName);
    m_tusFile = std::make_unique<TUS::TUSFile>(filePath, url,appName,m_uuid);
    //update the tusFile with the data from the cache
    if(m_cacheManager->findByHash(m_tusFile->getIdentificationHash())!= nullptr)
    {
        auto tusFile = m_cacheManager->findByHash(m_tusFile->getIdentificationHash());
        m_tusFile->setUploadOffset(tusFile->getUploadOffset());
        m_tusFile->setLastEdit(tusFile->getLastEdit());
        m_tusFile->setTusIdentifier(tusFile->getTusIdentifier());
        m_tusFile->setResumeFrom(tusFile->getResumeFrom());
        m_tusFile->setChunkNumber(tusFile->getChunkNumber());
        m_chunkNumber = m_tusFile->getChunkNumber();
    }


}

TusClient::~TusClient()
{
}
std::string extractHeaderValue(const std::string &header, const std::string &key)
{
    size_t record = header.find(key + ": ");
    if (record != std::string::npos)
    {
        size_t valueStart = record + key.length() + 2; // +2 for " :"
        size_t valueEnd = header.find("\r\n", valueStart);

        if (valueEnd != std::string::npos && valueStart < header.length())
        {
            return header.substr(valueStart, valueEnd - valueStart);
        }
        else if (valueStart < header.length()) // Handle case where valueEnd is not found
        {
            return header.substr(valueStart);
        }
    }

    return "";
}

void TusClient::upload()
{


    // chunk the file
    m_chunkNumber = divideFileInChunks(m_filePath, m_uuid);
    if (m_chunkNumber == -1)
    {
        std::cerr << "Error: Unable to divide file in chunks" << std::endl;
        return;
    }
    uintmax_t size = std::filesystem::file_size(m_filePath);
    std::map<std::string, std::string> headers;
    headers["Tus-Resumable"] = "1.0.0";
    headers["Content-Type"] = "application/octet-stream"; // Set the appropriate content type
    headers["Content-Disposition"] = "attachment; filename=\"" + getFilePath().filename().string() + "\"";
    headers["Content-Length"] = "0";
    headers["Upload-Length"] = std::to_string(size);
    headers["Upload-Metadata"] = "filename " + getFilePath().filename().string();
    OnSuccessCallback onPostSuccess = [this](std::string header, std::string data)
    {
        m_tusLocation = extractHeaderValue(header, "Location");
        m_tusLocation.replace(0, getUrl().length() + 7, ""); // remove the url from the location
    };
    m_httpClient->post(Http::Request(m_url + "/files", "", 
                       TUS::Http::HttpMethod::_POST, headers, onPostSuccess));
    m_httpClient->execute();
   
    getUploadInfo();
    std::cout<<"Upload started"<<std::endl;
    // patch chunks of the file to the server while chunk is not the last one
    uploadChunks();
}

void TusClient::uploadChunks()
{

    int i=m_uploadedChunks;
    m_status.store(TusStatus::UPLOADING);
    if(m_chunks.size()==0)
    {
        loadChunks();
    }
    if(m_uploadLength==0)
    {
       std::cout<<"No file to upload"<<std::endl;
       m_status.store(TusStatus::FINISHED);
       return;
    }
    for (; (m_uploadOffset<m_uploadLength)&&m_status==TusStatus::UPLOADING;)
    {
        if(m_status==TusStatus::CANCELED)
        {
            stop();
            return;
        }else if(m_status==TusStatus::FAILED)
        {
            std::cerr<<"Upload failed"<<std::endl;
            return;
        }
        else if(m_status==TusStatus::PAUSED)
        {
            std::cout<<"Upload paused"<<std::endl;
            return;
        }
        else{
            uploadChunk(i);
            i++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } 
       

    }
 
    stop();
}

void TusClient::loadChunks()
{
    for (int i = 0; i < m_chunkNumber; i++)
    {
        path chunkFilePath = getTUSTempDir() / getChunkFilename(i);

        std::ifstream chunkFile(chunkFilePath, std::ios::binary);
        if (!chunkFile)
        {
            std::cerr << "Error: Unable to open chunk file " << chunkFilePath << std::endl;
           m_status.store(TusStatus::FAILED);
            return;
        }

        chunkFile.seekg(0, std::ios::end);             // Seek to the end of the file to get the size
        std::streamsize chunkSize = chunkFile.tellg(); // Get the current position in the file, which is the size of the file
        chunkFile.seekg(0, std::ios::beg);             // Seek back to the beginning of the file

        std::vector<char> chunkData(chunkSize);
        chunkFile.read(chunkData.data(), chunkSize);
        chunkFile.close();

        TUSChunk chunk(chunkData, chunkSize);

        m_chunks.push_back(chunk);
    }
}

void TusClient::uploadChunk(int chunkNumber)
{
    if (m_status == TusStatus::PAUSED)
    {
        return;
    }

    path chunkFilePath = getTUSTempDir() / getChunkFilename(chunkNumber);
    TUSChunk chunk = m_chunks[chunkNumber];
    std::map<std::string, std::string> patchHeaders;

    m_nextChunk = false;

    patchHeaders["Tus-Resumable"] = "1.0.0";
    patchHeaders["Content-Type"] = "application/offset+octet-stream";
    patchHeaders["Content-Length"] = std::to_string(chunk.getChunkSize());
    patchHeaders["Upload-Offset"] = std::to_string(m_uploadOffset);

    OnSuccessCallback onPatchSuccess = [this](std::string header, std::string data)
    {
     
            if (header.find("204 No Content") != std::string::npos)
            {
                m_uploadedChunks++;
                m_nextChunk = true;
                m_uploadOffset = std::stoi(extractHeaderValue(header, "Upload-Offset"));

                m_progress.store(static_cast<float>(m_uploadOffset) / std::filesystem::file_size(m_filePath) * 100);
            }
            else if (header.find("409 Conflict") != std::string::npos)
            {
                static int retry = 0;
                if (retry < 3)
                {
                    retry++;
                    std::cout << "Conflict detected, retrying the upload" << std::endl;
                    getUploadInfo();
                }
                else
                {
                    std::cerr << "Error: Too many conflicts " << m_uploadedChunks << std::endl << header << std::endl;
                    m_status.store(TusStatus::FAILED);
                }
            }
            else
            {
                std::cerr << "Error: Unable to upload chunk " << m_uploadedChunks << std::endl << header << std::endl;
                m_status.store(TusStatus::FAILED);
            }           
    };

    OnErrorCallback onPatchError = [this](std::string header, std::string data)
    {
        std::cerr << "Error: Unable to upload chunk" << std::endl;
        m_status.store(TusStatus::FAILED);
    };

    m_httpClient->patch(Http::Request(m_url + "/files/" + m_tusLocation, 
                        std::string(chunk.getData().data(), chunk.getChunkSize()), 
                        Http::HttpMethod::_PATCH, patchHeaders,
                        onPatchSuccess, onPatchError));
    m_httpClient->execute();
}

void TusClient::cancel()
{
    m_status.store(TusStatus::CANCELED);
}

void TusClient::getUploadInfo()
{
    std::map<std::string, std::string> headers;

    OnSuccessCallback headSuccess = [this](std::string header, std::string data)
    {
        m_uploadOffset = std::stoi(extractHeaderValue(header, "Upload-Offset"));
        m_uploadLength= std::stoi(extractHeaderValue(header, "Upload-Length"));
    };

    headers.clear();
    headers["Tus-Resumable"] = "1.0.0";
    m_httpClient->head(Http::Request(m_url + "/files/" + m_tusLocation, "", Http::HttpMethod::_HEAD, headers, headSuccess));

    m_httpClient->execute();
}

void TusClient::resume()
{

    m_status.store(TusStatus::READY);
    getUploadInfo();
    uploadChunks();
}

void TusClient::pause()
{
    if (m_status == TusStatus::UPLOADING) {
        m_status.store(TusStatus::PAUSED);
        m_httpClient->abortAll();
        std::cout << "Upload paused" << std::endl;
    } else {
        std::cerr << "Cannot pause, current status is not UPLOADING" << std::endl;
    }
}

void TusClient::stop()
{
    if(m_uploadOffset==m_uploadLength&&m_status!=TusStatus::CANCELED&&m_status!=TusStatus::FAILED)
    {
        m_status.store(TusStatus::FINISHED);
    }
    else
    {
        return;
    }
    // remove the chunk files

    for (int i = 0; i < m_chunkNumber; i++)
    {
        path chunkFilePath = getTUSTempDir() / getChunkFilename(i);
        if (std::filesystem::exists(chunkFilePath))
        {
            if (!removeChunkFiles(chunkFilePath))
            {
                std::cerr << "Error: Unable to remove chunk file " << chunkFilePath << std::endl;
            }
        }
    }
    std::filesystem::remove(getTUSTempDir()/m_appName/getUUIDString());//remove the temp directory

}

float TusClient::progress()
{
    return m_progress;
}

TusStatus TusClient::status()
{
    return m_status;
}

void TusClient::retry()
{
    std::cout << "Retrying upload of file " << m_filePath << std::endl;
   resume();
   uploadChunks();
}

path TusClient::getFilePath() const
{
    return m_filePath;
}

std::string TusClient::getUrl() const
{
    return m_url;
}

std::string TusClient::getUUIDString()
{
    return boost::uuids::to_string(m_uuid);
}

path TusClient::getTUSTempDir()
{
    path filesTempDir = m_tempDir/m_appName/"files"/getUUIDString();

    if (!std::filesystem::exists(filesTempDir))
    {
        std::filesystem::create_directories(filesTempDir);
    }
    return filesTempDir;
}

std::string TusClient::getChunkFilename(int chunkNumber)
{
    return getUUIDString() + CHUNK_FILE_NAME_PREFIX + std::to_string(chunkNumber) + CHUNK_FILE_EXTENSION;
}



int TusClient::divideFileInChunks(path filePath, boost::uuids::uuid uuid)
{
    std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate); // Open input file in binary mode and seek to the end

    if (!inputFile)
    {
        std::cerr << "Error: Unable to open input file " << filePath << std::endl;
        return -1;
    }

    // Get the size of the file
    std::streamsize fileSize = inputFile.tellg(); // Get the current position in the input file, which is the size of the file
    inputFile.seekg(0, std::ios::beg);            // Seek back to the beginning of the file

    // Calculate the number of chunks
    int numChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Create a buffer to store the chunk data
    std::vector<char> buffer(CHUNK_SIZE);
    int totalBytesRead = 0;

    for (int i = 0; i < numChunks; ++i)
    {
        path outputFilePath = getTUSTempDir() / getChunkFilename(i);
        std::ofstream outputFile(outputFilePath, std::ios::binary); // Open output file in binary mode

        if (!outputFile)
        {
            std::cerr << "Error: Unable to create output file " << outputFilePath << std::endl;
            return -1;
        }

        // Read from input file and write to output file
        inputFile.read(buffer.data(), CHUNK_SIZE);
        std::streamsize bytesRead = inputFile.gcount();
        outputFile.write(buffer.data(), bytesRead);
        totalBytesRead += bytesRead;
    }

    inputFile.close();
    return numChunks;
}

bool TusClient::removeChunkFiles(path filePath)
{
    if(std::filesystem::exists(filePath)){
    
            return remove(filePath);

    }
    return false;
}