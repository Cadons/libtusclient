#ifndef INCLUDE_ILOGGER_H_
#define INCLUDE_ILOGGER_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include "libtusclient.h"
namespace TUS {
    namespace Logging {
        enum class LogLevel {
            
            
            WARNING = 0,
            ERROR = 1,
            CRITICAL = 2,
            INFO = 3,
            DEBUG = 4,
            NONE = 5
        };
        /**
         * @brief Interface for logging
         */
        class LIBTUSAPI_EXPORT ILogger {
        public:
            virtual void setLevel(LogLevel level) = 0;
            virtual void log(const std::string &message, LogLevel level) = 0;
            virtual void debug(const std::string &message) = 0;
            virtual void info(const std::string &message) = 0;
            virtual void warning(const std::string &message) = 0;
            virtual void error(const std::string &message) = 0;
            virtual void critical(const std::string &message) = 0;
            virtual void init(LogLevel level)=0;
            virtual ~ILogger(){};
        };
    } // namespace Logging
} // namespace TUS

#endif // INCLUDE_ILOGGER_H_