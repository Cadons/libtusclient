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
using std::string;
using std::list;
namespace TUS{
    namespace Http{
    class Request;
    
    class LIBTUSAPI_EXPORT IHttpClient
    {
    public:
        virtual void get(Request request) = 0;
        virtual void post(Request request) = 0;
        virtual void put(Request request) = 0;
        virtual void patch(Request request) = 0;
        virtual void del(Request request) = 0;
        virtual void head(Request request) = 0;
        virtual void options(Request request) = 0;
        virtual ~IHttpClient() {}
    };
    }

}

#endif // INCLUDE_HTTP_IHTTPCLIENT_H_