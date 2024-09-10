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

#include "libtusclient.h"
#include "IHttpClient.h"

#include "Request.h"
namespace TUS
{
    namespace Http
    {

        /**
         * @brief Represents a HTTP client
         */
        class LIBTUSAPI_EXPORT HttpClient : public IHttpClient
        {

        public:
            HttpClient();
            ~HttpClient();
            static std::queue<CURL*>m_requests;
            IHttpClient* get(Request request) override;
            IHttpClient* post(Request request) override;
            IHttpClient* put(Request request) override;
            IHttpClient* patch(Request request) override;
            IHttpClient* del(Request request) override;
            IHttpClient* head(Request request) override;
            IHttpClient* options(Request request) override;
            
            bool abortRequest() override;
            bool pauseRequest() override;
            bool resumeRequest() override;

            static string convertHttpMethodToString(HttpMethod method);

        private:
            void setupCURLRequest(CURL *curl, HttpMethod method, Request request);
            IHttpClient* sendRequest(HttpMethod method, Request request);
            
        };
    }
}
#endif // INCLUDE_HTTP_HTTPCLIENT_H_