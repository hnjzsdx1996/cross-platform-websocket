#pragma once

#include "../../platform/platform_interface.h"
#include "../logger/logger.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace cross_platform_websocket {

/**
 * @brief WebSocket 连接状态枚举
 */
enum class ConnectionState {
    DISCONNECTED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    RECONNECTING = 3,
    ERROR = 4
};

/**
 * @brief 消息类型枚举
 */
enum class MessageType {
    TEXT = 0,
    BINARY = 1,
    PING = 2,
    PONG = 3,
    CLOSE = 4
};

/**
 * @brief WebSocket 消息结构
 */
struct WebSocketMessage {
    MessageType type;
    std::string data;
    uint64_t timestamp;
    
    WebSocketMessage(MessageType t, const std::string& d)
        : type(t), data(d), timestamp(0) {}
};

/**
 * @brief 连接回调函数类型
 */
using ConnectionCallback = std::function<void(ConnectionState state)>;

/**
 * @brief 消息回调函数类型
 */
using MessageCallback = std::function<void(const WebSocketMessage& message)>;

/**
 * @brief 错误回调函数类型
 */
using ErrorCallback = std::function<void(const std::string& error)>;

/**
 * @brief 数据链路层类
 * 
 * 负责 WebSocket 连接管理和消息传输
 */
class DataLink {
public:
    /**
     * @brief 构造函数
     * @param platform 平台接口指针
     * @param logger 日志器指针
     */
    DataLink(std::shared_ptr<PlatformInterface> platform, 
             std::shared_ptr<Logger> logger);
    
    /**
     * @brief 析构函数
     */
    ~DataLink();
    
    /**
     * @brief 连接到 WebSocket 服务器
     * @param url WebSocket 服务器地址
     * @return 是否成功发起连接
     */
    bool connect(const std::string& url);
    
    /**
     * @brief 断开 WebSocket 连接
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
     * @brief 获取当前连接状态
     * @return 连接状态
     */
    ConnectionState getConnectionState() const;
    
    /**
     * @brief 检查是否已连接
     * @return 是否已连接
     */
    bool isConnected() const;
    
    /**
     * @brief 设置连接状态回调
     * @param callback 回调函数
     */
    void setConnectionCallback(ConnectionCallback callback);
    
    /**
     * @brief 设置消息接收回调
     * @param callback 回调函数
     */
    void setMessageCallback(MessageCallback callback);
    
    /**
     * @brief 设置错误回调
     * @param callback 回调函数
     */
    void setErrorCallback(ErrorCallback callback);
    
    /**
     * @brief 设置自动重连
     * @param enabled 是否启用自动重连
     * @param max_attempts 最大重连次数
     * @param interval_ms 重连间隔（毫秒）
     */
    void setAutoReconnect(bool enabled, int max_attempts = 5, int interval_ms = 1000);
    
    /**
     * @brief 获取连接统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    std::shared_ptr<PlatformInterface> platform_;
    std::shared_ptr<Logger> logger_;
    
    // 连接相关
    std::string server_url_;
    ConnectionState connection_state_;
    bool auto_reconnect_enabled_;
    int max_reconnect_attempts_;
    int reconnect_interval_ms_;
    int current_reconnect_attempts_;
    
    // 回调函数
    ConnectionCallback connection_callback_;
    MessageCallback message_callback_;
    ErrorCallback error_callback_;
    
    // 统计信息
    uint64_t messages_sent_;
    uint64_t messages_received_;
    uint64_t bytes_sent_;
    uint64_t bytes_received_;
    uint64_t connection_start_time_;
    
    // 内部方法
    void updateConnectionState(ConnectionState new_state);
    void handleConnectionSuccess();
    void handleConnectionError(const std::string& error);
    void handleMessageReceived(const WebSocketMessage& message);
    void startReconnectTimer();
    void stopReconnectTimer();
    void attemptReconnect();
    
    /**
     * @brief 重连线程函数
     * @param arg 线程参数
     */
    static void reconnectThread(void* arg);
    
    // 重连相关
    void* reconnect_thread_;
    bool reconnect_thread_running_;
};

} // namespace cross_platform_websocket 