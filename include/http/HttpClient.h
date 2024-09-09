#ifndef INCLUDE_HTTP_HTTPCLIENT_H_
#define INCLUDE_HTTP_HTTPCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include <map>
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

            int get(Request request) override;
            int post(Request request) override;
            int put(Request request) override;
            int patch(Request request) override;
            int del(Request request) override;
            int head(Request request) override;
            int options(Request request) override;
            void abortRequest(int requestID) override;

            static string convertHttpMethodToString(HttpMethod method);

        private:
            void setupCURLRequest(CURL *curl, HttpMethod method, Request request);
            int sendRequest(HttpMethod method, Request request);
            int m_requestID = 0;
            std::map<int, CURL *> m_requests;
        };
    }
}
#endif // INCLUDE_HTTP_HTTPCLIENT_H_