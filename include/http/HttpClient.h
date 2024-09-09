#ifndef INCLUDE_HTTP_HTTPCLIENT_H_
#define INCLUDE_HTTP_HTTPCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
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

            void get(Request request) override;
            void post(Request request) override;
            void put(Request request) override;
            void patch(Request request) override;
            void del(Request request) override;
            void head(Request request) override;
            void options(Request request) override;

            static string convertHttpMethodToString(HttpMethod method);

        private:
            void setupCURLRequest(CURL *curl, HttpMethod method, Request request);
            void sendRequest(HttpMethod method, Request request);
        };
    }
}
#endif // INCLUDE_HTTP_HTTPCLIENT_H_