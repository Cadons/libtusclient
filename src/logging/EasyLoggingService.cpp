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

        break;
    }
}

void EasyLoggingService::setLevel(LogLevel level) { m_level = level; }

void EasyLoggingService::init(LogLevel level) { setLevel(level); }

void EasyLoggingService::debug(const std::string &message) {
    if (m_level == LogLevel::DEBUG) {
        log(message, LogLevel::DEBUG);
        return;
    }
}

void EasyLoggingService::info(const std::string &message) {

    if (m_level <= LogLevel::INFO || m_level == LogLevel::DEBUG) {
        log(message, LogLevel::INFO);
        return;
    }
}

void EasyLoggingService::warning(const std::string &message) {
    if (m_level <= LogLevel::WARNING || m_level == LogLevel::DEBUG) {
        log(message, LogLevel::WARNING);
        return;
    }
}

void EasyLoggingService::error(const std::string &message) {
    if (m_level <= LogLevel::ERROR || m_level == LogLevel::DEBUG) {
        log(message, LogLevel::ERROR);
        return;
    }
}
void EasyLoggingService::critical(const std::string &message) {
    if (m_level <= LogLevel::CRITICAL || m_level == LogLevel::DEBUG) {
        log(message, LogLevel::CRITICAL);
        return;
    }
}

} // namespace TUS::Logging