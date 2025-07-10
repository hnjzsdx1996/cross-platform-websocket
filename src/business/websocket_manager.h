#pragma once

#include "../core/datalink/datalink.h"
#include "../core/logger/logger.h"
#include "../platform/platform_interface.h"
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <queue>
#include <mutex>

namespace cross_platform_websocket {

/**
 * @brief 消息优先级枚举
 */
enum class MessagePriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    URGENT = 3
};

/**
 * @brief 消息队列项
 */
struct QueuedMessage {
    std::string data;
    MessageType type;
    MessagePriority priority;
    uint64_t timestamp;
    
    QueuedMessage(const std::string& d, MessageType t, MessagePriority p)
        : data(d), type(t), priority(p), timestamp(0) {}
    
    // 用于优先级队列的比较
    bool operator<(const QueuedMessage& other) const {
        return static_cast<int>(priority) < static_cast<int>(other.priority);
    }
};

/**
 * @brief WebSocket 管理器类
 * 
 * 提供高级的 WebSocket 连接管理和消息处理功能
 */
class WebSocketManager {
public:
    /**
     * @brief 构造函数
     * @param platform 平台接口指针
     * @param logger 日志器指针
     */
    WebSocketManager(std::shared_ptr<PlatformInterface> platform,
                     std::shared_ptr<Logger> logger);
    
    /**
     * @brief 析构函数
     */
    ~WebSocketManager();
    
    /**
     * @brief 初始化管理器
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
     * @param priority 消息优先级
     * @return 是否发送成功
     */
    bool sendText(const std::string& message, MessagePriority priority = MessagePriority::NORMAL);
    
    /**
     * @brief 发送二进制消息
     * @param data 二进制数据
     * @param priority 消息优先级
     * @return 是否发送成功
     */
    bool sendBinary(const std::vector<uint8_t>& data, MessagePriority priority = MessagePriority::NORMAL);
    
    /**
     * @brief 发送 Ping 消息
     * @return 是否发送成功
     */
    bool sendPing();
    
    /**
     * @brief 获取连接状态
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
    void setConnectionCallback(std::function<void(ConnectionState)> callback);
    
    /**
     * @brief 设置消息接收回调
     * @param callback 回调函数
     */
    void setMessageCallback(std::function<void(const WebSocketMessage&)> callback);
    
    /**
     * @brief 设置错误回调
     * @param callback 回调函数
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief 设置消息发送成功回调
     * @param callback 回调函数
     */
    void setSendSuccessCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief 设置消息发送失败回调
     * @param callback 回调函数
     */
    void setSendFailureCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    /**
     * @brief 启用消息队列
     * @param enabled 是否启用
     * @param max_queue_size 最大队列大小
     */
    void enableMessageQueue(bool enabled, size_t max_queue_size = 1000);
    
    /**
     * @brief 处理消息队列
     */
    void processMessageQueue();
    
    /**
     * @brief 获取队列中的消息数量
     * @return 消息数量
     */
    size_t getQueuedMessageCount() const;
    
    /**
     * @brief 清空消息队列
     */
    void clearMessageQueue();
    
    /**
     * @brief 设置心跳间隔
     * @param interval_ms 心跳间隔（毫秒）
     */
    void setHeartbeatInterval(int interval_ms);
    
    /**
     * @brief 启用心跳
     * @param enabled 是否启用
     */
    void enableHeartbeat(bool enabled);
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;
    
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
    std::unique_ptr<DataLink> datalink_;
    
    // 消息队列相关
    std::priority_queue<QueuedMessage> message_queue_;
    mutable std::mutex queue_mutex_;
    bool queue_enabled_;
    size_t max_queue_size_;
    
    // 心跳相关
    bool heartbeat_enabled_;
    int heartbeat_interval_ms_;
    void* heartbeat_thread_;
    bool heartbeat_thread_running_;
    
    // 回调函数
    std::function<void(ConnectionState)> connection_callback_;
    std::function<void(const WebSocketMessage&)> message_callback_;
    std::function<void(const std::string&)> error_callback_;
    std::function<void(const std::string&)> send_success_callback_;
    std::function<void(const std::string&, const std::string&)> send_failure_callback_;
    
    // 统计信息
    uint64_t messages_sent_success_;
    uint64_t messages_sent_failed_;
    uint64_t messages_received_;
    
    // 内部方法
    void onConnectionStateChanged(ConnectionState state);
    void onMessageReceived(const WebSocketMessage& message);
    void onError(const std::string& error);
    void startHeartbeat();
    void stopHeartbeat();
    
    /**
     * @brief 心跳线程函数
     * @param arg 线程参数
     */
    static void heartbeatThread(void* arg);
    
    /**
     * @brief 执行心跳
     */
    void performHeartbeat();
};

} // namespace cross_platform_websocket 