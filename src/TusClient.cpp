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
using boost::uuids::random_generator;
using TUS::TusClient;
using TUS::TusStatus;
TusClient::TusClient(std::string url, std::string filePath) : m_url(url), m_filePath(filePath), m_status(TusStatus::READY), m_tempDir(TEMP_DIR), m_httpClient(std::make_unique<TUS::Http::HttpClient>())
{
    boost::uuids::uuid uuid = random_generator()();
    m_uuid = uuid;
}

TusClient::~TusClient()
{
}
std::string extractLocation(const std::string& header) {
    std::string locationKey = "Location: ";
    size_t locationPos = header.find(locationKey);
    
    if (locationPos != std::string::npos) {
        size_t urlStart = locationPos + locationKey.length();
        size_t urlEnd = header.find("\n", urlStart); 
        
        if (urlEnd != std::string::npos) {
            return header.substr(urlStart, urlEnd - urlStart);
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

    std::map<std::string, std::string> headers;
    headers["Tus-Resumable"] = "1.0.0";
    // this is for resume
    //  m_httpClient->head(Http::Request(m_url + "/files/" + getUUIDString(), "", Http::HttpMethod::_HEAD, headers));
    headers["Content-Type"] = "application/octet-stream"; // Set the appropriate content type
    headers["Content-Disposition"] = "attachment; filename=\"" + getFilePath().filename().string() + "\"";
    headers["Content-Range"] = "bytes */" + std::to_string(m_chunkNumber * CHUNK_SIZE);
    headers["Content-Length"] = "0";
    headers["Upload-Length"] = std::to_string(m_chunkNumber * CHUNK_SIZE);
    std::function<void(std::string header, std::string data)> onSuccess = [this](std::string header, std::string data)
    {     
        m_tusLocation=extractLocation(header);
        m_tusLocation.replace(0, getUrl().length() + 7, ""); // remove the url from the location
        // clean location from \r
        m_tusLocation.replace(m_tusLocation.find("\r"), 1, "");
    };
    m_httpClient->post(Http::Request(m_url + "/files", "", TUS::Http::HttpMethod::_POST, headers, onSuccess));
     m_httpClient->execute();
    while (!m_httpClient->isLastRequestCompleted())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
std::cout << "Waiting for the location header" << std::endl;
    }
    
    m_status = TusStatus::UPLOADING;

    // patch chunks of the file to the server while chunk is not the last one
    for(int i = 0; i < m_chunkNumber; i++)
    {
        /* code */
        path chunkFilePath = getTUSTempDir() / getChunkFilename(m_uploadedChunks);
        std::ifstream chunkFile(chunkFilePath, std::ios::binary);

        if (!chunkFile)
        {
            std::cerr << "Error: Unable to open chunk file " << chunkFilePath << std::endl;
            m_status = TusStatus::FAILED;
            return;
        }

        chunkFile.seekg(0, std::ios::end);             // Seek to the end of the file to get the size
        std::streamsize chunkSize = chunkFile.tellg(); // Get the current position in the file, which is the size of the file
        chunkFile.seekg(0, std::ios::beg);             // Seek back to the beginning of the file

        std::vector<char> chunkData(chunkSize);
        chunkFile.read(chunkData.data(), chunkSize);
        chunkFile.close();

        std::map<std::string, std::string> patchHeaders;
        patchHeaders["Tus-Resumable"] = "1.0.0";
        patchHeaders["Content-Type"] = "application/offset+octet-stream";
        patchHeaders["Upload-Offset"] = std::to_string(m_uploadedChunks);
        patchHeaders["Content-Length"] = std::to_string(chunkSize);
        std::function<void(std::string header, std::string data)> onSuccess = [this](std::string header, std::string data)
        {
            if (m_httpClient->isLastRequestCompleted())
            {
                std::cout << "Chunk " << m_uploadedChunks << " uploaded successfully" << std::endl;
                m_uploadedChunks++;
            }
            else
            {
                std::cerr << "Error: Unable to upload chunk " << m_uploadedChunks << std::endl;
                m_status = TusStatus::FAILED;
                return;
            }
        };
        std::function<void(std::string header, std::string data)> onError = [this](std::string header, std::string data)
        {
            std::cerr << "Error: Unable to upload chunk" << std::endl;
            m_status = TusStatus::FAILED;
            return;
        };
        m_httpClient->patch(Http::Request(m_url + "/files/" + m_tusLocation, std::string(chunkData.data(), chunkSize), Http::HttpMethod::_PATCH, patchHeaders, onSuccess, onError));

       
    }
    m_httpClient->execute();
    while (progress() < 100)
    {
        if (!m_httpClient->isLastRequestCompleted())
        {
            std::cerr << "Error: Unable to upload chunk " << m_uploadedChunks << std::endl;
            m_status = TusStatus::FAILED;
            return;
        }
        else
        {
            std::cout << progress() << "%";
        }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

    stop();
}

void TusClient::cancel()
{
    m_status = TusStatus::FAILED;
}

void TusClient::resume()
{
    m_status = TusStatus::READY;
}

void TusClient::stop()
{

    // remove the chunk files
    for (int i = 0; i < m_chunkNumber; i++)
    {
        path chunkFilePath = getTUSTempDir() / getChunkFilename(i);
        if (!removeChunkFiles(chunkFilePath))
        {
            std::cerr << "Error: Unable to remove chunk file " << chunkFilePath << std::endl;
        }
    }
    m_status = TusStatus::FINISHED;
}

int TusClient::progress()
{
    return (m_uploadedChunks / m_chunkNumber) * 100;
}

TusStatus TusClient::status()
{
    return m_status;
}

void TusClient::retry()
{
    std::cout << "Retrying upload of file " << m_filePath << std::endl;
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
    path filesTempDir = m_tempDir / getUUIDString();
    if (!std::filesystem::exists(filesTempDir))
    {
        std::filesystem::create_directory(filesTempDir);
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
    return remove(filePath);
}