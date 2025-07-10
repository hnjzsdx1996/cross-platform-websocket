#pragma once

#include <string>
#include <functional>

namespace cross_platform_websocket {

/**
 * @brief 平台能力注入接口
 * 
 * 这个接口定义了跨平台 WebSocket 框架需要的平台能力，
 * 包括日志、WebSocket、线程、配置等功能。
 * 不同平台（如 JS、Native）需要实现这个接口。
 */
class PlatformInterface {
public:
    virtual ~PlatformInterface() = default;
    
    // ==================== 日志接口 ====================
    
    /**
     * @brief 记录信息日志
     * @param message 日志消息
     */
    virtual void logInfo(const std::string& message) = 0;
    
    /**
     * @brief 记录错误日志
     * @param message 日志消息
     */
    virtual void logError(const std::string& message) = 0;
    
    /**
     * @brief 记录调试日志
     * @param message 日志消息
     */
    virtual void logDebug(const std::string& message) = 0;
    
    /**
     * @brief 记录警告日志
     * @param message 日志消息
     */
    virtual void logWarning(const std::string& message) = 0;
    
    // ==================== WebSocket 接口 ====================
    
    /**
     * @brief 建立 WebSocket 连接
     * @param url WebSocket 服务器地址
     * @return 是否连接成功
     */
    virtual bool websocketConnect(const std::string& url) = 0;
    
    /**
     * @brief 发送 WebSocket 消息
     * @param message 要发送的消息
     * @return 是否发送成功
     */
    virtual bool websocketSend(const std::string& message) = 0;
    
    /**
     * @brief 关闭 WebSocket 连接
     */
    virtual void websocketClose() = 0;
    
    /**
     * @brief 检查 WebSocket 连接状态
     * @return 是否已连接
     */
    virtual bool websocketIsConnected() = 0;
    
    // ==================== 线程接口 ====================
    
    /**
     * @brief 创建线程
     * @param func 线程函数
     * @param arg 线程参数
     * @return 线程句柄
     */
    virtual void* createThread(void (*func)(void*), void* arg) = 0;
    
    /**
     * @brief 等待线程结束
     * @param thread 线程句柄
     */
    virtual void joinThread(void* thread) = 0;
    
    /**
     * @brief 获取当前线程ID
     * @return 线程ID
     */
    virtual unsigned long getCurrentThreadId() = 0;
    
    // ==================== 配置接口 ====================
    
    /**
     * @brief 获取配置值
     * @param key 配置键
     * @return 配置值
     */
    virtual std::string getConfig(const std::string& key) = 0;
    
    /**
     * @brief 设置配置值
     * @param key 配置键
     * @param value 配置值
     */
    virtual void setConfig(const std::string& key, const std::string& value) = 0;
    
    /**
     * @brief 检查配置键是否存在
     * @param key 配置键
     * @return 是否存在
     */
    virtual bool hasConfig(const std::string& key) = 0;
    
    // ==================== 工具接口 ====================
    
    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 时间戳
     */
    virtual uint64_t getCurrentTimestamp() = 0;
    
    /**
     * @brief 生成随机数
     * @param min 最小值
     * @param max 最大值
     * @return 随机数
     */
    virtual int generateRandomNumber(int min, int max) = 0;
    
    /**
     * @brief 休眠指定毫秒数
     * @param milliseconds 毫秒数
     */
    virtual void sleep(int milliseconds) = 0;
};

} // namespace cross_platform_websocket 