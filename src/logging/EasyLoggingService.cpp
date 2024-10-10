#include "logging/EasyLoggingService.h"
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

namespace TUS::Logging {
EasyLoggingService::EasyLoggingService(LogLevel level) { init(level); }

EasyLoggingService::~EasyLoggingService() {
    // Finalize EasyLoggingPP
    el::Loggers::flushAll();
}

void EasyLoggingService::log(const std::string &message, LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG:
        LOG(DEBUG) << message;
        break;
    case LogLevel::INFO:
        LOG(INFO) << message;
        break;
    case LogLevel::WARNING:
        LOG(WARNING) << message;
        break;
    case LogLevel::ERROR:
        LOG(ERROR) << message;
        break;
    case LogLevel::CRITICAL:
        LOG(FATAL) << message;
        break;
    default:
        LOG(INFO) << message;
        break;
    }
}

void EasyLoggingService::setLevel(LogLevel level) { m_level = level; }

void EasyLoggingService::init(LogLevel level) { setLevel(level); }

void EasyLoggingService::debug(const std::string &message) {
    log(message, LogLevel::DEBUG);
}

void EasyLoggingService::info(const std::string &message) {
    log(message, LogLevel::INFO);
}

void EasyLoggingService::warning(const std::string &message) {
    log(message, LogLevel::WARNING);
}

void EasyLoggingService::error(const std::string &message) {
    log(message, LogLevel::ERROR);
}
void EasyLoggingService::critical(const std::string &message) {
    log(message, LogLevel::CRITICAL);
}

} // namespace TUS::Logging