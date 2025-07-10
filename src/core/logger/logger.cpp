#include "logger.h"
#include <sstream>
#include <iomanip>
#include <chrono>

namespace cross_platform_websocket {

// 全局日志实例
std::shared_ptr<Logger> g_logger;

Logger::Logger(std::shared_ptr<PlatformInterface> platform)
    : platform_(platform)
    , current_level_(LogLevel::INFO) {
}

void Logger::setLogLevel(LogLevel level) {
    current_level_ = level;
}

LogLevel Logger::getLogLevel() const {
    return current_level_;
}

void Logger::debug(const std::string& message, const std::string& file, int line) {
    if (shouldLog(LogLevel::DEBUG)) {
        std::string formatted = formatMessage(LogLevel::DEBUG, message, file, line);
        platform_->logDebug(formatted);
    }
}

void Logger::info(const std::string& message, const std::string& file, int line) {
    if (shouldLog(LogLevel::INFO)) {
        std::string formatted = formatMessage(LogLevel::INFO, message, file, line);
        platform_->logInfo(formatted);
    }
}

void Logger::warning(const std::string& message, const std::string& file, int line) {
    if (shouldLog(LogLevel::WARNING)) {
        std::string formatted = formatMessage(LogLevel::WARNING, message, file, line);
        platform_->logWarning(formatted);
    }
}

void Logger::error(const std::string& message, const std::string& file, int line) {
    if (shouldLog(LogLevel::ERROR)) {
        std::string formatted = formatMessage(LogLevel::ERROR, message, file, line);
        platform_->logError(formatted);
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message, 
                                 const std::string& file, int line) {
    std::ostringstream oss;
    
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm* tm = std::localtime(&time_t);
    
    // 格式化时间戳
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // 添加日志级别
    oss << " [" << getLevelString(level) << "] ";
    
    // 添加文件位置信息
    if (!file.empty() && line > 0) {
        size_t pos = file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? file.substr(pos + 1) : file;
        oss << "[" << filename << ":" << line << "] ";
    }
    
    // 添加消息内容
    oss << message;
    
    return oss.str();
}

bool Logger::shouldLog(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(current_level_);
}

std::string Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

} // namespace cross_platform_websocket 