#ifndef INCLUDE_EXCEPTIONS_TUSEXCEPTION_
#define INCLUDE_EXCEPTIONS_TUSEXCEPTION_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <stdexcept>
#include <string>
#include "libtusclient.h"


namespace TUS::Exceptions {
    /**
     * @brief Exception class for TUS client errors.
     */

    class EXPORT_LIBTUSCLIENT TUSException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
} // namespace TUS::Exceptions


#endif // INCLUDE_EXCEPTIONS_TUSEXCEPTION_
