/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fstream>
#include <iostream>
#include <thread>

#include "TusClient.h"
#include "cache/CacheRepository.h"
#include "cache/TUSFile.h"
#include "chunk/FileChunker.h"
#include "chunk/TUSChunk.h"
#include "chunk/utility/ChunkUtility.h"
#include "http/HttpClient.h"
#include "logging/EasyLoggingService.h"
#include "exceptions/TUSException.h"
using boost::uuids::random_generator;
using TUS::TusClient;
using TUS::TusStatus;
using TUS::Logging::EasyLoggingService;
using TUS::Logging::LogLevel;

void TusClient::initialize(int chunkSize)
{
    sanitizeUrl();
    createTusFile();
    m_cacheManager = std::make_unique<TUS::Cache::CacheRepository>(m_appName);
    m_fileChunker = std::make_unique<TUS::Chunk::FileChunker>(m_appName, getUUIDString(), m_filePath, chunkSize);
    // update the tusFile with the data from the cache
    if (m_cacheManager->findByHash(m_tusFile->getIdentificationHash()) !=
        nullptr)
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

TusClient::TusClient(std::string appName, std::string url, path filePath,
                     int chunkSize, Logging::LogLevel logLevel)
    : m_appName(appName), m_status(TusStatus::READY),
      m_url(std::move(url)), m_filePath(std::move(filePath)),
      m_logger(std::make_unique<Logging::EasyLoggingService>(logLevel)),
      m_httpClient(std::make_unique<TUS::Http::HttpClient>(
          std::make_unique<TUS::Logging::EasyLoggingService>(logLevel)))
{
    initialize(chunkSize);
}

TusClient::TusClient(std::string appName, std::string url, path filePath,
                     TUS::Logging::LogLevel logLevel)
    : m_appName(std::move(appName)), m_url(std::move(url)),
      m_filePath(std::move(filePath)), m_status(TusStatus::READY),
      m_httpClient(std::make_unique<TUS::Http::HttpClient>(
          std::make_unique<TUS::Logging::EasyLoggingService>(logLevel))),
      m_logger(std::make_unique<TUS::Logging::EasyLoggingService>(logLevel))
{
    initialize(0);
}

TusClient::~TusClient() {
    m_httpClient->abortAll();
}

void TusClient::createTusFile()
{

    m_tusFile.reset();
    boost::uuids::uuid uuid = random_generator()();
    m_uuid = uuid;
    m_tusFile = std::make_unique<TUS::Cache::TUSFile>(m_filePath, m_url,
                                                      m_appName, m_uuid);
}

std::string extractHeaderValue(const std::string &header,
                               const std::string &key)
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
        else if (valueStart <
                 header.length()) // Handle case where valueEnd is not found
        {
            return header.substr(valueStart);
        }
    }

    return "";
}

bool TusClient::upload()
{

    if (m_tusFile == nullptr)
    {
        createTusFile();
    }

    // chunk the file
    m_chunkNumber = m_fileChunker->chunkFile();
    if (m_chunkNumber == -1)
    {
        m_logger->error("Error: Unable to divide file in chunks");
        return false;
    }
    uintmax_t size = std::filesystem::file_size(m_filePath);
    std::map<std::string, std::string> headers;
    headers["Tus-Resumable"] = TUS_PROTOCOL_VERSION;
    headers["Content-Type"] =
        "application/octet-stream"; // Set the appropriate content type
    headers["Content-Disposition"] =
        "attachment; filename=\"" + getFilePath().filename().string() + "\"";
    headers["Content-Length"] = "0";
    headers["Upload-Length"] = std::to_string(size);
    headers["Upload-Metadata"] =
        "filename " + getFilePath().filename().string();
    OnSuccessCallback onPostSuccess = [this](std::string header,
                                             std::string data)
    {
        m_tusLocation = extractHeaderValue(header, "Location");
        size_t lastSlashPosition = m_tusLocation.find_last_of('/');
        if (lastSlashPosition != std::string::npos) {
            // remove the url from the location
            m_tusLocation.replace(0, lastSlashPosition+1,  "");
            std::cout<<"slash position:"<<lastSlashPosition<<std::endl;
        } else {
            // Handle the case where '/' is not found in m_tusLocation
            std::cerr << "Error: '/' not found in m_tusLocation" << std::endl;
        }

    };
    OnErrorCallback onError = [this](std::string header, std::string data)
    {
        m_logger->error(data);
        m_status.store(TusStatus::FAILED);
        throw TUS::Exceptions::TUSException(data);
    };
    m_logger->debug("Starting new upload");
    m_httpClient->post(Http::Request(m_url , "",
                                     TUS::Http::HttpMethod::_POST, headers,
                                     onPostSuccess, onError));
    m_httpClient->execute();
    m_logger->debug("Getting information about the upload");

    getUploadInfo();

    m_logger->debug("Saving tusFile to cache");
    m_cacheManager->add(m_tusFile);
    m_cacheManager->save();
    m_logger->debug("Uploading");
    m_logger->info("Upload started");
    // patch chunks of the file to the server while chunk is not the last one
    return uploadChunks();
}

bool TusClient::uploadChunks()
{

    int i = m_uploadedChunks;
    m_status.store(TusStatus::UPLOADING);
    if (m_fileChunker->getChunkNumber() == 0)
    {
        if (!m_fileChunker->loadChunks())
        {
            m_status.store(TusStatus::FAILED);
            throw TUS::Exceptions::TUSException("Error: Unable to load chunks");
        }
    }

    if (m_uploadLength == 0)
    {
        m_logger->warning("No file to upload");
        m_status.store(TusStatus::FINISHED);
        return true;
    }
    for (; (m_uploadOffset < m_uploadLength) &&
           m_status.load() == TusStatus::UPLOADING;)
    {
        try
        {
            uploadChunk(i);
        }
        catch (TUS::Exceptions::TUSException &e)
        {
            m_logger->error(e.what());
            m_status.store(TusStatus::FAILED);
            return false;
        }
        i++;
    }
    stop();
    return true;
}

void TusClient::uploadChunk(int chunkNumber)
{
    if (m_status.load() != TusStatus::UPLOADING)
    {
        return;
    }

    Chunk::TUSChunk chunk = m_fileChunker->getChunks().at(chunkNumber);
    std::map<std::string, std::string> patchHeaders;

    m_nextChunk = false;

    patchHeaders["Tus-Resumable"] = TUS_PROTOCOL_VERSION;
    patchHeaders["Content-Type"] = "application/offset+octet-stream";
    patchHeaders["Content-Length"] = std::to_string(chunk.getChunkSize());
    patchHeaders["Upload-Offset"] = std::to_string(m_uploadOffset);

    OnSuccessCallback onPatchSuccess = [this](std::string header,
                                              std::string data)
    {
        if (header.find("204 No Content") != std::string::npos)
        {
            m_uploadedChunks++;
            m_nextChunk = true;
            m_uploadOffset =
                std::stoi(extractHeaderValue(header, "Upload-Offset"));

            m_progress.store(static_cast<float>(m_uploadOffset) /
                             std::filesystem::file_size(m_filePath) * 100);
        }
        else if (header.find("409 Conflict") != std::string::npos)
        {
            static int retry = 0;
            if (retry < 3)
            {
                retry++;
                m_logger->warning("Conflict detected, retrying the upload");
                getUploadInfo();
            }
            else
            {
                m_logger->error("Error: Too many conflicts " + std::to_string(
                                                                   m_uploadedChunks));
                m_logger->error(header);
                m_status.store(TusStatus::FAILED);
                throw TUS::Exceptions::TUSException(
                    "Error: Too many conflicts");
            }
            std::this_thread::sleep_for(
                m_requestTimeout); // wait a bit before the other request
        }
        else
        {
            m_logger->error("Error: Unable to upload chunk " + std::to_string(m_uploadedChunks));
            m_logger->error(header);
            m_status.store(TusStatus::FAILED);
            throw TUS::Exceptions::TUSException("Error: Unable to upload chunk");
        }
    };

    OnErrorCallback onPatchError = [this](std::string header,
                                          std::string data)
    {
        m_logger->error("Error: Unable to upload chunk");
        if (m_status.load() != TusStatus::CANCELED &&
            m_status.load() != TusStatus::PAUSED) // in this case is not a
                                                  // problem if request fails
        {
            m_status.store(TusStatus::FAILED);
            throw TUS::Exceptions::TUSException("Error: Unable to upload chunk");
        }
    };
    m_logger->debug("Uploading chunk " + std::to_string(chunkNumber));

    m_httpClient->patch(Http::Request(
        m_url + m_tusLocation,
        std::string(reinterpret_cast<char *>(chunk.getData().data()),
                    chunk.getChunkSize()),
        Http::HttpMethod::_PATCH, patchHeaders, onPatchSuccess, onPatchError));
    m_httpClient->execute();
}

void TusClient::cancel()
{
    m_logger->debug("Cancelling upload");
    if (m_tusLocation.empty())
    {
        m_logger->error("No upload to cancel");
        return;
    }
    m_status.store(TusStatus::CANCELED);
    function<void(string, string)> onSuccess = [this](string header,
                                                      string data)
    {
        m_cacheManager->remove(m_tusFile);
        m_cacheManager->save();
        m_logger->info("Upload canceled");
    };
    std::map<std::string, std::string> headers;
    headers["Tus-Resumable"] = TUS_PROTOCOL_VERSION;
    headers["accept"] = "*/*";
    m_httpClient->abortAll();
    m_httpClient->del(Http::Request(m_url + m_tusLocation, "",
                                    Http::HttpMethod::_DELETE, headers,
                                    onSuccess));
    m_httpClient->execute();
}

void TusClient::getUploadInfo()
{
    std::map<std::string, std::string> headers;

    OnSuccessCallback headSuccess = [this](std::string header,
                                           std::string data)
    {
        m_uploadOffset = std::stoi(extractHeaderValue(header, "Upload-Offset"));
        m_uploadLength = std::stoi(extractHeaderValue(header, "Upload-Length"));
    };

    OnErrorCallback onError = [this](std::string header, std::string data)
    {
        m_logger->error(data);
        m_status.store(TusStatus::FAILED);
        throw TUS::Exceptions::TUSException("Error: Unable to get upload information");
    };

    headers.clear();
    headers["Tus-Resumable"] = TUS_PROTOCOL_VERSION;
    m_httpClient->head(Http::Request(m_url + m_tusLocation, "",
                                     Http::HttpMethod::_HEAD, headers,
                                     headSuccess, onError));

    m_httpClient->execute();
    if (m_status.load() == TusStatus::FAILED)
    {
        return;
    }
}

std::map<std::string, std::string> TusClient::getTusServerInformation()
{
    std::map<std::string, std::string> serverInfo;
    std::map<std::string, std::string> headers;
    headers["accept"] = "*/*";

    function<void(string, string)> onSuccess = [&serverInfo](string header,
                                                             string data)
    {
        serverInfo["Upload-Offset"] =
            extractHeaderValue(header, "Upload-Offset");
        serverInfo["Upload-Length"] =
            extractHeaderValue(header, "Upload-Length");
        serverInfo["Tus-Resumable"] =
            extractHeaderValue(header, "Tus-Resumable");
        serverInfo["Tus-Version"] = extractHeaderValue(header, "Tus-Version");
        serverInfo["Tus-Extension"] =
            extractHeaderValue(header, "Tus-Extension");
        serverInfo["Tus-Max-Size"] = extractHeaderValue(header, "Tus-Max-Size");
    };
    m_logger->debug("Getting server information");
    m_httpClient->options(Http::Request(
        m_url , "", Http::HttpMethod::_OPTIONS, headers, onSuccess));
    m_httpClient->execute();
    return serverInfo;
}

bool TusClient::resume()
{
    m_logger->debug("Resuming the upload");
    getUploadInfo();
    m_status.store(TusStatus::READY);

    return uploadChunks();
}

void TusClient::pause()
{
    m_logger->debug("Pausing the upload");
    if (m_status.load() == TusStatus::UPLOADING)
    {
        m_status.store(TusStatus::PAUSED);
        m_logger->info("Upload paused");
        m_httpClient->abortAll();
    }
    else
    {
        m_logger->error("Cannot pause, current status is not UPLOADING");
    }
}

void TusClient::stop()
{
    if (m_status.load() == TusStatus::PAUSED)
    {
        m_logger->debug("upload paused");
        return;
    }
    m_logger->debug("Stopping the upload");
    if (m_uploadOffset == m_uploadLength &&
        (m_status.load() != TusStatus::CANCELED &&
         m_status.load() != TusStatus::FAILED))
    {
        m_logger->debug("Upload completed");
        m_status.store(TusStatus::FINISHED);
    }

    m_cacheManager->remove(m_tusFile);
    m_cacheManager->save();
}

float TusClient::progress() const { return m_progress.load(); }

TusStatus TusClient::status() { return m_status.load(); }

bool TusClient::retry()
{
    if (m_status.load() == TusStatus::FAILED ||
        m_status.load() == TusStatus::CANCELED)
    {
        m_logger->debug("Retrying upload");
        m_status.store(TusStatus::READY);
        m_fileChunker->clearChunks();
        m_uploadedChunks = 0;
        m_uploadOffset = 0;
        m_nextChunk = false;
        m_progress.store(0);
        return upload();
    }
    else
    {
        m_logger->warning("Nothing to retry");
        return false;
    }
}

path TusClient::getFilePath() const { return m_filePath; }

std::string TusClient::getUrl() const { return m_url; }

std::string TusClient::getUUIDString()
{
    return boost::uuids::to_string(m_uuid);
}
std::chrono::milliseconds TusClient::getRequestTimeout() const
{

    return m_requestTimeout;
}

void TusClient::setRequestTimeout(std::chrono::milliseconds ms)
{
    m_requestTimeout = ms;
}




void TusClient::sanitizeUrl()
{
    if (m_url.back() != '/')
    {
        m_url.push_back('/');
    }
}