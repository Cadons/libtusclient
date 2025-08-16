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
#include <format>

#include "TusClient.h"
#include "cache/CacheRepository.h"
#include "cache/TUSFile.h"
#include "chunk/FileChunker.h"
#include "chunk/TUSChunk.h"
#include "http/HttpClient.h"
#include "logging/GLoggingService.h"
#include "exceptions/TUSException.h"
using boost::uuids::random_generator;
using TUS::TusClient;
using TUS::TusStatus;
using TUS::Logging::GLoggingService;
using TUS::Logging::LogLevel;

void TusClient::initialize(int chunkSize) {
    sanitizeUrl();
    createTusFile();
    m_cacheManager = std::make_unique<TUS::Cache::CacheRepository>(m_appName);
    m_fileChunker = std::make_unique<TUS::Chunk::FileChunker>(m_appName, getUUIDString(), m_filePath, chunkSize);
    // update the tusFile with the data from the cache
    if (m_cacheManager->findByHash(m_tusFile->getIdentificationHash()) !=
        nullptr) {
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
                     const int chunkSize, Logging::LogLevel logLevel)
    : m_url(std::move(url)), m_filePath(std::move(filePath)),
      m_status(TusStatus::READY), m_httpClient(std::make_unique<TUS::Http::HttpClient>(
          std::make_unique<TUS::Logging::GLoggingService>(logLevel))),
      m_logger(std::make_unique<Logging::GLoggingService>(logLevel)),
      m_appName(std::move(appName)) {
    initialize(chunkSize);
}

TusClient::TusClient(std::string appName, std::string url, path filePath,
                     TUS::Logging::LogLevel logLevel)
    : m_url(std::move(url)), m_filePath(std::move(filePath)),
      m_status(TusStatus::READY), m_httpClient(std::make_unique<TUS::Http::HttpClient>(
          std::make_unique<TUS::Logging::GLoggingService>(logLevel))),
      m_logger(std::make_unique<TUS::Logging::GLoggingService>(logLevel)),
      m_appName(std::move(appName)) {
    initialize(0);
}

TusClient::~TusClient() {
    m_httpClient->abortAll();
}

void TusClient::createTusFile() {
    m_tusFile.reset();
    boost::uuids::uuid uuid = random_generator()();
    m_uuid = uuid;
    m_tusFile = std::make_unique<TUS::Cache::TUSFile>(m_filePath, m_url,
                                                      m_appName, m_uuid);
}

std::string extractHeaderValue(const std::string &header,
                               const std::string &key) {
    const auto toLower = [](const std::string &s) {
        std::string result = s;
        std::ranges::transform(result, result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    };

    std::string keyLower = toLower(key);
    size_t pos = 0;

    while (pos < header.size()) {
        size_t lineEnd = header.find("\r\n", pos);
        if (lineEnd == std::string::npos)
            lineEnd = header.size();


        if (const size_t colonPos = header.find(':', pos); colonPos != std::string::npos && colonPos < lineEnd) {
            std::string lineKey = header.substr(pos, colonPos - pos);
            std::string lineValue = header.substr(colonPos + 1, lineEnd - colonPos - 1);

            // Trim spaces helper
            auto trim = [](std::string &str) {
                const char *whitespace = " \t";
                size_t start = str.find_first_not_of(whitespace);
                size_t end = str.find_last_not_of(whitespace);
                if (start == std::string::npos) {
                    str.clear();
                    return;
                }
                str = str.substr(start, end - start + 1);
            };

            trim(lineKey);
            trim(lineValue);

            if (toLower(lineKey) == keyLower)
                return lineValue;
        }

        pos = lineEnd + 2;
    }

    return "";
}

bool TusClient::upload() {
    if (m_tusFile == nullptr) {
        createTusFile();
    }

    // chunk the file
    m_chunkNumber = m_fileChunker->chunkFile();
    if (m_chunkNumber == -1) {
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
    OnSuccessCallback onPostSuccess = [this](const std::string &header,
                                             [[maybe_unused]] const std::string &data) {
        m_tusLocation = extractHeaderValue(header, "Location");
        size_t lastSlashPosition = m_tusLocation.find_last_of('/');
        if (lastSlashPosition != std::string::npos) {
            // remove the url from the location
            m_tusLocation.replace(0, lastSlashPosition + 1, "");
        } else {
            // Handle the case where '/' is not found in m_tusLocation
            std::cerr << "Error: '/' not found in m_tusLocation, check if the upload url point to a TUS route" << std::endl;
        }
    };
    OnErrorCallback onError = [this]([[maybe_unused]] const std::string &header, const std::string &data) {
        m_logger->error(data);
        m_status.store(TusStatus::FAILED);
        throw TUS::Exceptions::TUSException(data);
    };
    m_logger->debug("Starting new upload");
    m_httpClient->post(Http::Request(m_url, "",
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

bool TusClient::uploadChunks() {
    int i = m_uploadedChunks;
    m_status.store(TusStatus::UPLOADING);
    if (m_fileChunker->getChunkNumber() == 0 && !m_fileChunker->loadChunks()) {
        m_status.store(TusStatus::FAILED);
        throw TUS::Exceptions::TUSException("Error: Unable to load chunks");
    }

    if (m_uploadLength == 0) {
        m_logger->warning("No file to upload");
        m_status.store(TusStatus::FINISHED);
        return true;
    }
    while ((m_uploadOffset < m_uploadLength) &&
           m_status.load() == TusStatus::UPLOADING) {
        try {
            uploadChunk(i);
        } catch (TUS::Exceptions::TUSException &e) {
            m_logger->error(e.what());
            m_status.store(TusStatus::FAILED);
            return false;
        }
        i++;
    }
    stop();
    return true;
}

void TusClient::handleSuccessfulUpload(const string &header) {
    m_uploadedChunks++;
    m_nextChunk = true;
    m_uploadOffset = std::stoi(extractHeaderValue(header, "Upload-Offset"));

    float progress = static_cast<float>(m_uploadOffset) /
                     static_cast<float>(std::filesystem::file_size(m_filePath)) * 100;
    m_progress.store(progress);
}

void TusClient::handleUploadConflict(const string &header) {
    if (m_retry < 3) {
        m_retry++;
        m_logger->warning("Conflict detected, retrying the upload");
        getUploadInfo();
    } else {
        m_logger->error(std::format("Error: Too many conflicts {}", m_uploadedChunks));
        m_logger->error(header);
        m_status.store(TusStatus::FAILED);
        throw TUS::Exceptions::TUSException("Error: Too many conflicts");
    }
    std::this_thread::sleep_for(m_requestTimeout);
}

 void TusClient::handleUploadError(const string &header) {
    m_logger->error(std::format("Error: Unable to upload chunk {}", m_uploadedChunks));
    m_logger->error(header);
    m_status.store(TusStatus::FAILED);
    throw TUS::Exceptions::TUSException("Error: Unable to upload chunk");
}


void TusClient::uploadChunk(int chunkNumber) {
    if (m_status.load() != TusStatus::UPLOADING) {
        return;
    }

    Chunk::TUSChunk chunk = m_fileChunker->getChunks().at(chunkNumber);
    std::map<std::string, std::string> patchHeaders;

    m_nextChunk = false;

    patchHeaders["Tus-Resumable"] = TUS_PROTOCOL_VERSION;
    patchHeaders["Content-Type"] = "application/offset+octet-stream";
    patchHeaders["Content-Length"] = std::to_string(chunk.getChunkSize());
    patchHeaders["Upload-Offset"] = std::to_string(m_uploadOffset);
    OnSuccessCallback onPatchSuccess = [this](const std::string &header, [[maybe_unused]] const std::string &data) {
        if (header.find("204 No Content") != std::string::npos) {
            handleSuccessfulUpload(header);
        } else if (header.find("409 Conflict") != std::string::npos) {
            handleUploadConflict(header);
        } else {
            handleUploadError(header);
        }
    };


    OnErrorCallback onPatchError = [this]([[maybe_unused]] const std::string &header,
                                          [[maybe_unused]] const std::string &data) {
        m_logger->error("Error: Unable to upload chunk");
        if (m_status.load() != TusStatus::CANCELED &&
            m_status.load() != TusStatus::PAUSED) // in this case is not a
        // problem if request fails
        {
            m_status.store(TusStatus::FAILED);
            throw TUS::Exceptions::TUSException("Error: Unable to upload chunk");
        }
    };
    m_logger->debug(std::format("Uploading chunk {}", chunkNumber));
    m_httpClient->patch(Http::Request(
        m_url + m_tusLocation,
        std::string(reinterpret_cast<char *>(chunk.getData().data()),
                    chunk.getChunkSize()),
        Http::HttpMethod::_PATCH, patchHeaders, onPatchSuccess, onPatchError));
    m_httpClient->execute();
}

void TusClient::cancel() {
    m_logger->debug("Cancelling upload");
    if (m_tusLocation.empty()) {
        m_logger->error("No upload to cancel");
        return;
    }
    m_status.store(TusStatus::CANCELED);
    function<void(string, string)> onSuccess = [this]([[maybe_unused]] const string &header,
                                                      [[maybe_unused]] const string &data) {
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

void TusClient::getUploadInfo() {
    std::map<std::string, std::string> headers;

    OnSuccessCallback headSuccess = [this](const std::string &header,
                                           [[maybe_unused]] const std::string &data) {
        m_uploadOffset = std::stoi(extractHeaderValue(header, "Upload-Offset"));
        m_uploadLength = std::stoi(extractHeaderValue(header, "Upload-Length"));
    };

    OnErrorCallback onError = [this]([[maybe_unused]] const std::string &header, const std::string &data) {
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
    if (m_status.load() == TusStatus::FAILED) {
        return;
    }
}

std::map<std::string, std::string, std::less<> > TusClient::getTusServerInformation() const {
    std::map<string, string, std::less<> > serverInfo;
    std::map<std::string, std::string> headers;
    headers["accept"] = "*/*";

    function<void(string, string)> onSuccess = [&serverInfo](const string &header,
                                                             [[maybe_unused]] const string &data) {
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
        m_url, "", Http::HttpMethod::_OPTIONS, headers, onSuccess));
    m_httpClient->execute();
    return serverInfo;
}

bool TusClient::resume() {
    m_logger->debug("Resuming the upload");
    getUploadInfo();
    m_status.store(TusStatus::READY);

    return uploadChunks();
}

void TusClient::pause() {
    m_logger->debug("Pausing the upload");
    if (m_status.load() == TusStatus::UPLOADING) {
        m_status.store(TusStatus::PAUSED);
        m_logger->info("Upload paused");
        m_httpClient->abortAll();
    } else {
        m_logger->error("Cannot pause, current status is not UPLOADING");
    }
}

void TusClient::stop() {
    if (m_status.load() == TusStatus::PAUSED) {
        m_logger->debug("upload paused");
        return;
    }
    m_logger->debug("Stopping the upload");
    if (m_uploadOffset == m_uploadLength &&
        (m_status.load() != TusStatus::CANCELED &&
         m_status.load() != TusStatus::FAILED)) {
        m_logger->debug("Upload completed");
        m_status.store(TusStatus::FINISHED);
    }

    m_cacheManager->remove(m_tusFile);
    m_cacheManager->save();
}

float TusClient::progress() const { return m_progress.load(); }

TusStatus TusClient::status() { return m_status.load(); }

bool TusClient::retry() {
    if (m_status.load() == TusStatus::FAILED ||
        m_status.load() == TusStatus::CANCELED) {
        m_logger->debug("Retrying upload");
        m_status.store(TusStatus::READY);
        m_fileChunker->clearChunks();
        m_uploadedChunks = 0;
        m_uploadOffset = 0;
        m_nextChunk = false;
        m_progress.store(0);
        return upload();
    } else {
        m_logger->warning("Nothing to retry");
        return false;
    }
}

path TusClient::getFilePath() const { return m_filePath; }

std::string TusClient::getUrl() const { return m_url; }

std::string TusClient::getUUIDString() const {
    return boost::uuids::to_string(m_uuid);
}

std::chrono::milliseconds TusClient::getRequestTimeout() const {
    return m_requestTimeout;
}

void TUS::TusClient::setBearerToken(const std::string &token) const {
    m_httpClient->setAuthorization(token);
}

bool TUS::TusClient::isTokenSet() const {
    return m_httpClient->isAuthenticated();
}

void TusClient::setRequestTimeout(std::chrono::milliseconds ms) {
    m_requestTimeout = ms;
}

void TusClient::sanitizeUrl() {
    if (m_url.back() != '/') {
        m_url.push_back('/');
    }
}
