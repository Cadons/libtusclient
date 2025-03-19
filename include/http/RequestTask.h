#ifndef INCLUDE_HTTP_REQUESTTASK_H_
#define INCLUDE_HTTP_REQUESTTASK_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <curl/curl.h>

#include "Request.h"
#include "libtusclient.h"


namespace TUS::Http {
    struct EXPORT_LIBTUSCLIENT RequestTask : public Request {
        CURL *curl;

        RequestTask(const Request &request, CURL *curl);

        ~RequestTask();
    };
}


#endif // INCLUDE_HTTP_REQUESTTASK_H_
