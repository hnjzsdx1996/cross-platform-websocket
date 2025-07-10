#pragma once

#include "../../platform/platform_interface.h"
#include <string>
#include <memory>

namespace cross_platform_websocket {

/**
 * @brief 日志级别枚举
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

/**
 * @brief 日志库类
 * 
 * 提供统一的日志记录功能，支持不同级别的日志输出
 */
class Logger {
public:
    /**
     * @brief 构造函数
     * @param platform 平台接口指针
     */
    explicit Logger(std::shared_ptr<PlatformInterface> platform);
    
    /**
     * @brief 析构函数
     */
    ~Logger() = default;
    
    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief 获取当前日志级别
     * @return 日志级别
     */
    LogLevel getLogLevel() const;
    
    /**
     * @brief 记录调试日志
     * @param message 日志消息
     * @param file 文件名（可选）
     * @param line 行号（可选）
     */
    void debug(const std::string& message, const std::string& file = "", int line = 0);
    
    /**
     * @brief 记录信息日志
     * @param message 日志消息
     * @param file 文件名（可选）
     * @param line 行号（可选）
     */
    void info(const std::string& message, const std::string& file = "", int line = 0);
    
    /**
     * @brief 记录警告日志
     * @param message 日志消息
     * @param file 文件名（可选）
     * @param line 行号（可选）
     */
    void warning(const std::string& message, const std::string& file = "", int line = 0);
    
    /**
     * @brief 记录错误日志
     * @param message 日志消息
     * @param file 文件名（可选）
     * @param line 行号（可选）
     */
    void error(const std::string& message, const std::string& file = "", int line = 0);
    
    /**
     * @brief 格式化日志消息
     * @param level 日志级别
     * @param message 原始消息
     * @param file 文件名
     * @param line 行号
     * @return 格式化后的消息
     */
    std::string formatMessage(LogLevel level, const std::string& message, 
                             const std::string& file, int line);

private:
    std::shared_ptr<PlatformInterface> platform_;
    LogLevel current_level_;
    
    /**
     * @brief 检查是否应该输出指定级别的日志
     * @param level 日志级别
     * @return 是否应该输出
     */
    bool shouldLog(LogLevel level) const;
    
    /**
     * @brief 获取日志级别字符串
     * @param level 日志级别
     * @return 级别字符串
     */
    std::string getLevelString(LogLevel level) const;
};

// 全局日志实例
extern std::shared_ptr<Logger> g_logger;

// 便捷宏定义
#define LOG_DEBUG(msg) if (g_logger) g_logger->debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) if (g_logger) g_logger->info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) if (g_logger) g_logger->warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) if (g_logger) g_logger->error(msg, __FILE__, __LINE__)

} // namespace cross_platform_websocket 