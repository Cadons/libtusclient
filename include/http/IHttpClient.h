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
        virtual int get(Request request) = 0;
        virtual int post(Request request) = 0;
        virtual int put(Request request) = 0;
        virtual int patch(Request request) = 0;
        virtual int del(Request request) = 0;
        virtual int head(Request request) = 0;
        virtual int options(Request request) = 0;
        virtual void abortRequest(int requestID) = 0;
        virtual ~IHttpClient() {}
    };
    }

}

#endif // INCLUDE_HTTP_IHTTPCLIENT_H_