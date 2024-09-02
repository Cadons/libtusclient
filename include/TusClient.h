#ifndef TUSCLIENT_H
#define TUSCLIENT_H
#include "libtusclient.h"
#include <string>
#include "TusStatus.h"
#include "http/IHttpClient.h"
#include <memory>
using namespace std;
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
        virtual string getFilePath() const = 0;
        virtual string getUrl() const = 0;
    };
    /**
     * @brief The TusClient class represents a client for uploading files using the TUS protocol.
     */
    class LIBTUSAPI_EXPORT TusClient: public ITusClient
    {
    private:
        string m_url; 
        string m_filePath;
        TusStatus m_status;
        std::unique_ptr<Http::IHttpClient> m_httpClient;

        const int CHUNK_SIZE = 1024;
        int m_chunkNumber=0;
        
        
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

        string getFilePath() const override;
        string getUrl() const override;

    };

}

#endif // TUSCLIENT_H