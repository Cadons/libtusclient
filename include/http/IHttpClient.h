#ifndef INCLUDE_HTTP_IHTTPCLIENT_H_
#define INCLUDE_HTTP_IHTTPCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include "libtusclient.h"
#include <string>
#include <list>
using std::list;
using std::string;
namespace TUS
{
    namespace Http
    {
        class Request;

        /**
         * @brief Interface for an HTTP client.
         *
         * This interface defines the methods that an HTTP client must implement.
         * It provides methods for performing various HTTP methods such as GET, POST, PUT, PATCH, DELETE, HEAD, and OPTIONS.
         * It also provides a method for aborting a request.
         */
        class LIBTUSAPI_EXPORT IHttpClient
        {
        public:
            /**
             * @brief Perform a GET request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *get(Request request) = 0;
            /**
             * @brief Perform a POST request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *post(Request request) = 0;
            /**
             * @brief Perform a PUT request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *put(Request request) = 0;
            /**
             * @brief Perform a PATCH request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *patch(Request request) = 0;
            /**
             * @brief Perform a DELETE request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *del(Request request) = 0;
            /**
             * @brief Perform a HEAD request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *head(Request request) = 0;
            /**
             * @brief Perform an OPTIONS request.
             * @param request The request to perform.
             * @return A pointer to the HTTP client.
             */
            virtual IHttpClient *options(Request request) = 0;
            /**
             * @brief Abort the current request.
             * @return True if there are other requests to abort, false otherwise.
             */
            virtual bool abortRequest() = 0;
            /**
             * @brief Pause the current request.
             * @return True if the request was paused, false otherwise.
             */
            virtual bool pauseRequest() = 0;
            /**
             * @brief Resume the current request.
             * @return True if the request was resumed, false otherwise.
             */
            virtual bool resumeRequest() = 0;
            virtual ~IHttpClient() {}
        };
    }

}

#endif // INCLUDE_HTTP_IHTTPCLIENT_H_