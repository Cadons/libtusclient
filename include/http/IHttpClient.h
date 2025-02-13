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
        class EXPORT_LIBTUSCLIENT IHttpClient
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
             * @brief Abort all requests.
             */
            virtual IHttpClient* abortAll() = 0;

            virtual IHttpClient* execute() = 0;
          
            virtual ~IHttpClient() {};
            /**
             * @brief set the token for the authentication
             */
            virtual void setAuthorization(const std::string& token)=0;
            /**
             * @brief return if token is set
             */
            virtual bool isAuthenticated()=0;


        };
    }

}

#endif // INCLUDE_HTTP_IHTTPCLIENT_H_