#ifndef INCLUDE_GLOGGINGSERVICE_
#define INCLUDE_GLOGGINGSERVICE_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include "ILogger.h"
#include <glog/logging.h>
#include <atomic>
#include <mutex>



namespace TUS::Logging {
class EXPORT_LIBTUSCLIENT GLoggingService : public ILogger {
  public:
    explicit GLoggingService(LogLevel level);
    ~GLoggingService() override;
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
  static inline bool isInitialized = false;

  static inline  std::atomic<int> instanceCount{0};
  static inline  std::mutex initMutex;

};

} // namespace TUS::Logging

#endif // INCLUDE_GLOGGINGSERVICE_