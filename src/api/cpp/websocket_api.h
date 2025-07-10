#pragma once

#include "../../business/websocket_manager.h"
#include "../../core/logger/logger.h"
#include "../../platform/platform_interface.h"
#include <string>
#include <memory>
#include <functional>

namespace cross_platform_websocket {

/**
 * @brief WebSocket API 类
 * 
 * 提供简洁易用的 C++ API 接口
 */
class WebSocketAPI {
public:
    /**
     * @brief 构造函数
     * @param platform 平台接口指针
     */
    explicit WebSocketAPI(std::shared_ptr<PlatformInterface> platform);
    
    /**
     * @brief 析构函数
     */
    ~WebSocketAPI();
    
    /**
     * @brief 初始化 API
     * @return 是否初始化成功
     */
    bool initialize();
    
    /**
     * @brief 连接到 WebSocket 服务器
     * @param url 服务器地址
     * @param auto_reconnect 是否自动重连
     * @return 是否成功发起连接
     */
    bool connect(const std::string& url, bool auto_reconnect = true);
    
    /**
     * @brief 断开连接
     */
    void disconnect();
    
    /**
     * @brief 发送文本消息
     * @param message 消息内容
     * @return 是否发送成功
     */
    bool sendText(const std::string& message);
    
    /**
     * @brief 发送二进制消息
     * @param data 二进制数据
     * @return 是否发送成功
     */
    bool sendBinary(const std::vector<uint8_t>& data);
    
    /**
     * @brief 发送 Ping 消息
     * @return 是否发送成功
     */
    bool sendPing();
    
    /**
     * @brief 检查是否已连接
     * @return 是否已连接
     */
    bool isConnected() const;
    
    /**
     * @brief 获取连接状态
     * @return 连接状态
     */
    ConnectionState getConnectionState() const;
    
    /**
     * @brief 设置连接状态回调
     * @param callback 回调函数
     */
    void setConnectionCallback(std::function<void(ConnectionState)> callback);
    
    /**
     * @brief 设置消息接收回调
     * @param callback 回调函数
     */
    void setMessageCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief 设置错误回调
     * @param callback 回调函数
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief 启用消息队列
     * @param enabled 是否启用
     * @param max_size 最大队列大小
     */
    void enableMessageQueue(bool enabled, size_t max_size = 1000);
    
    /**
     * @brief 启用心跳
     * @param enabled 是否启用
     * @param interval_ms 心跳间隔（毫秒）
     */
    void enableHeartbeat(bool enabled, int interval_ms = 30000);
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;
    
    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief 设置配置
     * @param key 配置键
     * @param value 配置值
     */
    void setConfig(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取配置
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfig(const std::string& key) const;

private:
    std::shared_ptr<PlatformInterface> platform_;
    std::shared_ptr<Logger> logger_;
    std::unique_ptr<WebSocketManager> manager_;
    
    // 内部回调处理
    void onConnectionStateChanged(ConnectionState state);
    void onMessageReceived(const WebSocketMessage& message);
    void onError(const std::string& error);
    
    // 用户回调函数
    std::function<void(ConnectionState)> user_connection_callback_;
    std::function<void(const std::string&)> user_message_callback_;
    std::function<void(const std::string&)> user_error_callback_;
};

} // namespace cross_platform_websocket 