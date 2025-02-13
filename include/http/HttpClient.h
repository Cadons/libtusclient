#ifndef INCLUDE_HTTP_HTTPCLIENT_H_
#define INCLUDE_HTTP_HTTPCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include <queue>
#include <functional>
#include <curl/curl.h>
#include <memory>
#include <mutex>

#include "libtusclient.h"
#include "IHttpClient.h"
#include "http/RequestTask.h"
#include "logging/ILogger.h"
#include "Request.h"

namespace TUS
{
    namespace Http
    {

        struct EXPORT_LIBTUSCLIENT Progress
        {
            char * data;
            size_t size;
        };

        /**
         * @brief Represents a HTTP client
         */
        class EXPORT_LIBTUSCLIENT HttpClient : public IHttpClient
        {

        public:
            HttpClient();
            HttpClient(std::unique_ptr<TUS::Logging::ILogger> logger);
            virtual ~HttpClient();
            IHttpClient *get(Request request) override;
            IHttpClient *post(Request request) override;
            IHttpClient *put(Request request) override;
            IHttpClient *patch(Request request) override;
            IHttpClient *del(Request request) override;
            IHttpClient *head(Request request) override;
            IHttpClient *options(Request request) override;
          
            IHttpClient* abortAll() override;
            /**
             * @brief Execute the requests in the queue
             * it start a new thread that will execute the requests, if the thread is already running it will do nothing
             */
            IHttpClient* execute()override;

            void setAuthorization(const std::string& token) override;
            bool isAuthenticated() override;
            
            static string convertHttpMethodToString(HttpMethod method);
            static int getHttpReturnCode(const std::string& header);

        private:
            void setupCURLRequest(CURL *curl, HttpMethod method, Request request);
            IHttpClient *sendRequest(HttpMethod method, Request request);
            std::queue<RequestTask> m_requestsQueue;
            bool m_abort = false;
            std::mutex m_queueMutex;  // Mutex to protect shared resources
            std::shared_ptr<TUS::Logging::ILogger> m_logger;
            std::string m_token="";

            /**
             * @brief Callback function for the write data of the request
             * @param ptr is the pointer to the data
             * @param size is the size of the data
             * @param nmemb  the size of the data
             * @param data is the data
             * @return size_t
             */
            static size_t writeDataCallback(void *ptr, size_t size, size_t nmemb, std::string *data);
            /**
             * @brief Callback function for the progress of the request
             * @param clientp is a pointer to the client
             * @param dltotal is the total size of the download
             * @param dlnow is the current size of the download
             * @param ultotal is the total size of the upload
             * @param ulnow is the current size of the upload
             * @return int (0=ok, 1=abort)
             */
            static int progressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
            std::shared_ptr<string> m_buffer;
        };
    }
}
#endif // INCLUDE_HTTP_HTTPCLIENT_H_