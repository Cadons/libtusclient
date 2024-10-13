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
    case LogLevel::_DEBUG_:
        LOG(DEBUG) << message;
        break;
    case LogLevel::_INFO_:
        LOG(INFO) << message;
        break;
    case LogLevel::_WARNING_:
        LOG(WARNING) << message;
        break;
    case LogLevel::_ERROR_:
        LOG(ERROR) << message;
        break;
    case LogLevel::_CRITICAL_:
        LOG(FATAL) << message;
        break;
    default:

        break;
    }
}

void EasyLoggingService::setLevel(LogLevel level) { m_level = level; }

void EasyLoggingService::init(LogLevel level) { setLevel(level); }

void EasyLoggingService::debug(const std::string &message) {
    if (m_level == LogLevel::_DEBUG_) {
        log(message, LogLevel::_DEBUG_);
        return;
    }
}

void EasyLoggingService::info(const std::string &message) {

    if (m_level <= LogLevel::_INFO_ || m_level == LogLevel::_DEBUG_) {
        log(message, LogLevel::_INFO_);
        return;
    }
}

void EasyLoggingService::warning(const std::string &message) {
    if (m_level <= LogLevel::_WARNING_ || m_level == LogLevel::_DEBUG_) {
        log(message, LogLevel::_WARNING_);
        return;
    }
}

void EasyLoggingService::error(const std::string &message) {
    if (m_level <= LogLevel::_ERROR_ || m_level == LogLevel::_DEBUG_) {
        log(message, LogLevel::_ERROR_);
        return;
    }
}
void EasyLoggingService::critical(const std::string &message) {
    if (m_level <= LogLevel::_CRITICAL_ || m_level == LogLevel::_DEBUG_) {
        log(message, LogLevel::_CRITICAL_);
        return;
    }
}

} // namespace TUS::Logging