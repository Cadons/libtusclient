#ifndef INCLUDE_EXCEPTIONS_HTTPEXCEPTION_
#define INCLUDE_EXCEPTIONS_HTTPEXCEPTION_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include "libtusclient.h"
#include <exception>
#include <string>

namespace TUS
{
    namespace Exceptions
    {
        /**
         * @brief Exception class for http client errors.
         */
        class LIBTUSAPI_EXPORT HttpException : public std::exception
        {
        public:
            HttpException(const std::string &message) : std::runtime_error(message) {}
        };
    } // namespace Exceptions

} // namespace libtusclient
