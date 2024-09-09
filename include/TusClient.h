#ifndef INCLUDE_TUSCLIENT_H_
#define INCLUDE_TUSCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include <memory>
#include <filesystem>

#include "libtusclient.h"
#include "TusStatus.h"
#include "http/IHttpClient.h"
using std::string;
using std::unique_ptr;
using std::filesystem::path;

//temporary directory for the chunks for each os 
#ifdef WIN32
    #define TEMP_DIR R"(C:\Users\AppData\Local\Temp\TUS\)"
#elif __linux__
    #define TEMP_DIR "/tmp/TUS/"

#elif __APPLE__
    #define TEMP_DIR getenv("TMPDIR")
#elif __ANDROID__
    #define TEMP_DIR "/data/local/tmp/TUS/"
#else
    #define TEMP_DIR "/tmp/TUS/"
#endif

/**
 * @brief The TusClient class represents a client for uploading files using the TUS protocol.
 * 
 * This class provides methods for uploading, canceling, resuming, stopping, and retrying file uploads.
 * It also provides methods for retrieving the upload progress and status.
 */
namespace TUS{
    /*
    * @brief The ITusClient class represents an interface for a client for uploading files using the TUS protocol.
    */
    class LIBTUSAPI_EXPORT ITusClient
    {
    public:
        virtual void upload() = 0;
        virtual void cancel() = 0;
        virtual void resume() = 0;
        virtual void stop() = 0;
        
        virtual int progress() = 0;
        virtual TusStatus status() = 0;
        virtual void retry() = 0;
        // Getters
        virtual path getFilePath() const = 0;
        virtual string getUrl() const = 0;
    };
    /**
     * @brief The TusClient class represents a client for uploading files using the TUS protocol.
     */
    class LIBTUSAPI_EXPORT TusClient: public ITusClient
    {
    private:
        string m_url; 
        path m_filePath;
        TusStatus m_status;
        path m_tempDir;
        
        std::unique_ptr<Http::IHttpClient> m_httpClient;
        
        const int CHUNK_SIZE = 1024;
        const string CHUNK_FILE_NAME_PREFIX = "chunk_";
        const string CHUNK_FILE_EXTENSION = ".bin";
        int m_chunkNumber=0;

        /**
         * @brief Divides the file into chunks.
         * 
         * @param path The path of the file to divide.
         * @return The number of chunks the file was divided into.
         */

        int divideFileInChunks(path filePath);

        /**
         * @brief Removes the chunk files, this when a chunk is uploaded successfully.
         * 
         * @param path The path of the file to remove the chunks from.
         */
        bool removeChunkFiles(path filePath);
        
        
    public:
        TusClient(string url, string filePath);
        ~TusClient();
        /**
         * @brief Uploads the file to the server using the TUS protocol.
         */
        void upload() override;
        /**
         * @brief Cancels the upload.
         */
        void cancel() override;
        /**
         * @brief Resumes the upload.
         */
        void resume() override;
        /**
         * @brief Stops the upload.
         */
        void stop() override;
        /**
         * @brief Returns the progress of the upload.
         * 
         * @return The progress of the upload as a percentage.
         */
        int progress() override;
        /**
         * @brief Returns the status of the upload.
         * 
         * @return The status of the upload.
         */
        TusStatus status() override;
        /**
         * @brief Retries the upload.
         */
        void retry() override;

        path getFilePath() const override;
        string getUrl() const override;

    };

}

#endif // INCLUDE_TUSCLIENT_H_