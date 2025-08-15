#ifndef INCLUDE_ILOGGER_H_
#define INCLUDE_ILOGGER_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include "libtusclient.h"

namespace TUS::Logging {
    enum class LogLevel {
        _WARNING_ = 0,
        _ERROR_ = 1,
        _CRITICAL_ = 2,
        _INFO_ = 3,
        _DEBUG_ = 4,
        _NONE_ = 5
    };

    /**
     * @brief Interface for logging
     */
    class EXPORT_LIBTUSCLIENT ILogger {
    public:
        virtual void setLevel(LogLevel level) = 0;

        virtual void log(const std::string &message, LogLevel level) = 0;

        virtual void debug(const std::string &message) = 0;

        virtual void info(const std::string &message) = 0;

        virtual void warning(const std::string &message) = 0;

        virtual void error(const std::string &message) = 0;

        virtual void critical(const std::string &message) = 0;

        virtual void init(LogLevel level) =0;

        virtual ~ILogger() = default;
    };
} // namespace TUS::Logging


#endif // INCLUDE_ILOGGER_H_
