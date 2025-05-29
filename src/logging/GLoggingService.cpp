#include "logging/GLoggingService.h"
namespace TUS::Logging {
    GLoggingService::GLoggingService(LogLevel level) {
        std::scoped_lock<std::mutex> lock(initMutex);
        if (!isInitialized) {
            google::InitGoogleLogging("libtusclient");
            FLAGS_logtostderr = true; // Log to stderr by default
            isInitialized = true;
        }
        instanceCount++;
        GLoggingService::init(level);
    }

    GLoggingService::~GLoggingService() {
        std::scoped_lock<std::mutex> lock(initMutex);
        if (--instanceCount == 0 && isInitialized) {
            google::ShutdownGoogleLogging();
            isInitialized = false;
        }
    }

    void GLoggingService::log(const std::string &message, LogLevel level) {
        switch (level) {
            using enum LogLevel;
            case _DEBUG_:
                DLOG(INFO) << message; // Debug logs are only enabled in debug mode
                break;
            case _INFO_:
                LOG(INFO) << message;
                break;
            case _WARNING_:
                LOG(WARNING) << message;
                break;
            case _ERROR_:
                LOG(ERROR) << message;
                break;
            case _CRITICAL_:
                LOG(FATAL) << message; // FATAL will terminate the program

            default:
                break;
        }
    }

    void GLoggingService::setLevel(LogLevel level) {
        m_level = level;
    }

    void GLoggingService::init(LogLevel level) {
        GLoggingService::setLevel(level);
    }

    void GLoggingService::debug(const std::string &message) {
        if (m_level == LogLevel::_DEBUG_) {
            log(message, LogLevel::_DEBUG_);
        }
    }

    void GLoggingService::info(const std::string &message) {
        if (m_level <= LogLevel::_INFO_ || m_level == LogLevel::_DEBUG_) {
            log(message, LogLevel::_INFO_);
        }
    }

    void GLoggingService::warning(const std::string &message) {
        if (m_level <= LogLevel::_WARNING_ || m_level == LogLevel::_DEBUG_) {
            log(message, LogLevel::_WARNING_);
        }
    }

    void GLoggingService::error(const std::string &message) {
        if (m_level <= LogLevel::_ERROR_ || m_level == LogLevel::_DEBUG_) {
            log(message, LogLevel::_ERROR_);
        }
    }

    void GLoggingService::critical(const std::string &message) {
        if (m_level <= LogLevel::_CRITICAL_ || m_level == LogLevel::_DEBUG_) {
            log(message, LogLevel::_CRITICAL_);
        }
    }
} // namespace TUS::Logging
