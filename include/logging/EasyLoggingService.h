#ifndef INCLUDE_EASYLOGGINGSERVICE_
#define INCLUDE_EASYLOGGINGSERVICE_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include "ILogger.h"
#include <easylogging++.h>


namespace TUS::Logging {
class EXPORT_LIBTUSCLIENT EasyLoggingService : public ILogger {
  public:
    explicit EasyLoggingService(LogLevel level);
    ~EasyLoggingService() override;
    void setLevel(LogLevel level) override;
    void log(const std::string &message, LogLevel level) override;
    void debug(const std::string &message) override;
    void info(const std::string &message) override;
    void warning(const std::string &message) override;
    void error(const std::string &message) override;
    void critical(const std::string &message) override;
    void init(LogLevel level) override;

  private:
    LogLevel m_level=LogLevel::_INFO_;
};

} // namespace TUS::Logging

#endif // INCLUDE_EASYLOGGINGSERVICE_