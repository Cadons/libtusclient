/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#ifndef INCLUDE_TUSCLIENT_H_
#define INCLUDE_TUSCLIENT_H_

#include <atomic>
#include <boost/uuid/uuid.hpp>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include "TusStatus.h"
#include "libtusclient.h"
#include "logging/ILogger.h"

using std::string;
using std::unique_ptr;
using std::filesystem::path;

/**
 * @brief The TusClient class represents a client for uploading files using the
 * TUS protocol.
 *
 * This class provides methods for uploading, canceling, resuming, stopping, and
 * retrying file uploads. It also provides methods for retrieving the upload
 * progress and status.
 */
namespace TUS {
    namespace Repository {
        template<typename T>
        class IRepository;
    } // namespace Repository
    namespace Cache {
        class TUSFile;
    } // namespace Cache

    namespace Chunk {
        template<typename T>
        class IFileChunker;
        class TUSChunk;
    } // namespace Chunk

    namespace Http {
        class IHttpClient;
    } // namespace Http

    /*
     * @brief The ITusClient class represents an interface for a client for
     * uploading files using the TUS protocol.
     */
    class EXPORT_LIBTUSCLIENT ITusClient {
    public:
        virtual ~ITusClient() = default;

        virtual bool upload() = 0;

        virtual void cancel() = 0;

        virtual bool resume() = 0;

        virtual void stop() = 0;

        virtual void pause() = 0;

        [[nodiscard]] virtual float progress() const = 0;

        virtual TusStatus status() = 0;

        virtual bool retry() = 0;

        /**
         * @brief set the time that TUS library have to wait between 2 chunk upload
         * @param ms
         */
        virtual void setRequestTimeout(std::chrono::milliseconds ms) = 0;

        // Getters
        [[nodiscard]] virtual path getFilePath() const = 0;

        [[nodiscard]] virtual string getUrl() const = 0;

        /**
         * @brief get the time that TUS library have to wait between 2 chunk upload
         * @return The time in ms
         */
        [[nodiscard]] virtual std::chrono::milliseconds getRequestTimeout() const = 0;
    };

    /**
     * @brief The TusClient class represents a client for uploading files using the
     * TUS protocol.
     */
    class EXPORT_LIBTUSCLIENT TusClient : public ITusClient {
    private:
        using OnSuccessCallback = std::function<void(std::string, std::string)>;
        using OnErrorCallback = std::function<void(std::string, std::string)>;
        string m_url;
        path m_filePath;
        std::atomic<TusStatus> m_status;

        int m_chunkNumber = 0;
        int m_uploadedChunks = 0;
        string m_tusLocation;
        int m_uploadOffset = 0;
        bool m_nextChunk = false;
        size_t m_uploadLength = 0;
        std::chrono::milliseconds m_requestTimeout = std::chrono::milliseconds(
            0); /*This timeout is the time waited between one requests, it is in ms,
                   and it can be changed by the user*/

        std::atomic<float> m_progress{0};
        std::unique_ptr<Http::IHttpClient> m_httpClient;
        std::shared_ptr<Cache::TUSFile> m_tusFile;
        std::unique_ptr<Repository::IRepository<Cache::TUSFile> > m_cacheManager;
        std::unique_ptr<Chunk::IFileChunker<Chunk::TUSChunk> > m_fileChunker;
        std::unique_ptr<Logging::ILogger> m_logger;

        std::string m_appName;

        void getUploadInfo();

        void createTusFile();

        boost::uuids::uuid m_uuid;

        string getUUIDString() const;

        bool uploadChunks();

        void uploadChunk(int chunkNumber);

        void initialize(int chunkSize);

        /**
         * @brief Sanitize the url string
         * @return
         */
        void sanitizeUrl();

    public:
        TusClient(string appName, string url, path filePath, int chunkSize,
                  Logging::LogLevel logLevel = Logging::LogLevel::_NONE_);

        TusClient(string appName, string url, path filePath, Logging::LogLevel logLevel = Logging::LogLevel::_NONE_);

        ~TusClient() override;

        /**
         * @brief Uploads the file to the server using the TUS protocol.
         */
        bool upload() override;

        /**
         * @brief Cancels the upload.
         */
        void cancel() override;

        /**
         * @brief Resumes the upload.
         */
        bool resume() override;

        /**
         * @brief Stops the upload.
         */
        void stop() override;

        /**
         * @brief Returns the progress of the upload.
         *
         * @return The progress of the upload as a percentage.
         */
        float progress() const override;

        /**
         * @brief Returns the status of the upload.
         *
         * @return The status of the upload.
         */
        TusStatus status() override;

        /**
         * @brief Retries the upload.
         */
        bool retry() override;

        /**
         * @brief Pauses the upload.
         */
        void pause() override;

        /**
         * @brief Returns the server information for the TUS server.
         * this function can be used to understand which extensions are supported by
         * the server.
         * @return The server information.
         */

        std::map<string, string> getTusServerInformation() const;

        path getFilePath() const override;

        string getUrl() const override;

        void setRequestTimeout(std::chrono::milliseconds ms) override;

        std::chrono::milliseconds getRequestTimeout() const override;

        /**
         * @brief set the authorization token, verify that token is valid,
         * if not update the token and pass the updated once
         */
        void setBearerToken(const std::string &token) const;

        /**
         * @brief This function return if token is set,
         * if token is set
         */
        bool isTokenSet() const;
    };
} // namespace TUS
#endif // INCLUDE_TUSCLIENT_H_
