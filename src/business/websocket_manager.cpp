#include "websocket_manager.h"
#include <sstream>
#include <algorithm>
#include <memory>
#include <mutex>

namespace cross_platform_websocket {

WebSocketManager::WebSocketManager(std::shared_ptr<PlatformInterface> platform,
                                   std::shared_ptr<Logger> logger)
    : platform_(platform)
    , logger_(logger)
    , queue_enabled_(false)
    , max_queue_size_(1000)
    , heartbeat_enabled_(false)
    , heartbeat_interval_ms_(30000)  // 30秒
    , heartbeat_thread_(nullptr)
    , heartbeat_thread_running_(false)
    , messages_sent_success_(0)
    , messages_sent_failed_(0)
    , messages_received_(0) {
    
    LOG_INFO("WebSocket 管理器创建");
}

WebSocketManager::~WebSocketManager() {
    disconnect();
    stopHeartbeat();
    LOG_INFO("WebSocket 管理器销毁");
}

bool WebSocketManager::initialize() {
    try {
        // 创建数据链路层
        datalink_ = std::unique_ptr<DataLink>(new DataLink(platform_, logger_));
        
        // 设置回调函数
        datalink_->setConnectionCallback(
            [this](ConnectionState state) { onConnectionStateChanged(state); });
        datalink_->setMessageCallback(
            [this](const WebSocketMessage& msg) { onMessageReceived(msg); });
        datalink_->setErrorCallback(
            [this](const std::string& error) { onError(error); });
        
        LOG_INFO("WebSocket 管理器初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("WebSocket 管理器初始化失败: " + std::string(e.what()));
        return false;
    }
}

bool WebSocketManager::connect(const std::string& url, bool auto_reconnect) {
    if (!datalink_) {
        LOG_ERROR("数据链路层未初始化");
        return false;
    }
    
    // 设置自动重连
    datalink_->setAutoReconnect(auto_reconnect);
    
    LOG_INFO("连接到 WebSocket 服务器: " + url);
    return datalink_->connect(url);
}

void WebSocketManager::disconnect() {
    if (datalink_) {
        datalink_->disconnect();
    }
    stopHeartbeat();
    LOG_INFO("WebSocket 连接已断开");
}

bool WebSocketManager::sendText(const std::string& message, MessagePriority priority) {
    if (!datalink_) {
        LOG_ERROR("数据链路层未初始化");
        return false;
    }
    
    if (!isConnected()) {
        if (queue_enabled_) {
            // 将消息加入队列
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (message_queue_.size() < max_queue_size_) {
                QueuedMessage queued_msg(message, MessageType::TEXT, priority);
                queued_msg.timestamp = platform_->getCurrentTimestamp();
                message_queue_.push(queued_msg);
                LOG_DEBUG("消息已加入队列: " + message);
                return true;
            } else {
                LOG_WARNING("消息队列已满，丢弃消息: " + message);
                messages_sent_failed_++;
                if (send_failure_callback_) {
                    send_failure_callback_(message, "队列已满");
                }
                return false;
            }
        } else {
            LOG_ERROR("WebSocket 未连接，无法发送消息");
            messages_sent_failed_++;
            if (send_failure_callback_) {
                send_failure_callback_(message, "未连接");
            }
            return false;
        }
    }
    
    // 直接发送消息
    if (datalink_->sendText(message)) {
        messages_sent_success_++;
        LOG_DEBUG("消息发送成功: " + message);
        if (send_success_callback_) {
            send_success_callback_(message);
        }
        return true;
    } else {
        messages_sent_failed_++;
        LOG_ERROR("消息发送失败: " + message);
        if (send_failure_callback_) {
            send_failure_callback_(message, "发送失败");
        }
        return false;
    }
}

bool WebSocketManager::sendBinary(const std::vector<uint8_t>& data, MessagePriority priority) {
    if (!datalink_) {
        LOG_ERROR("数据链路层未初始化");
        return false;
    }
    
    if (!isConnected()) {
        if (queue_enabled_) {
            // 将二进制消息转换为字符串并加入队列
            std::string binary_str(data.begin(), data.end());
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (message_queue_.size() < max_queue_size_) {
                QueuedMessage queued_msg(binary_str, MessageType::BINARY, priority);
                queued_msg.timestamp = platform_->getCurrentTimestamp();
                message_queue_.push(queued_msg);
                LOG_DEBUG("二进制消息已加入队列，大小: " + std::to_string(data.size()) + " 字节");
                return true;
            } else {
                LOG_WARNING("消息队列已满，丢弃二进制消息");
                messages_sent_failed_++;
                return false;
            }
        } else {
            LOG_ERROR("WebSocket 未连接，无法发送二进制消息");
            messages_sent_failed_++;
            return false;
        }
    }
    
    // 直接发送二进制消息
    if (datalink_->sendBinary(data)) {
        messages_sent_success_++;
        LOG_DEBUG("二进制消息发送成功，大小: " + std::to_string(data.size()) + " 字节");
        return true;
    } else {
        messages_sent_failed_++;
        LOG_ERROR("二进制消息发送失败");
        return false;
    }
}

bool WebSocketManager::sendPing() {
    if (!datalink_) {
        LOG_ERROR("数据链路层未初始化");
        return false;
    }
    
    return datalink_->sendPing();
}

ConnectionState WebSocketManager::getConnectionState() const {
    return datalink_ ? datalink_->getConnectionState() : ConnectionState::DISCONNECTED;
}

bool WebSocketManager::isConnected() const {
    return datalink_ && datalink_->isConnected();
}

void WebSocketManager::setConnectionCallback(std::function<void(ConnectionState)> callback) {
    connection_callback_ = callback;
}

void WebSocketManager::setMessageCallback(std::function<void(const WebSocketMessage&)> callback) {
    message_callback_ = callback;
}

void WebSocketManager::setErrorCallback(std::function<void(const std::string&)> callback) {
    error_callback_ = callback;
}

void WebSocketManager::setSendSuccessCallback(std::function<void(const std::string&)> callback) {
    send_success_callback_ = callback;
}

void WebSocketManager::setSendFailureCallback(std::function<void(const std::string&, const std::string&)> callback) {
    send_failure_callback_ = callback;
}

void WebSocketManager::enableMessageQueue(bool enabled, size_t max_queue_size) {
    queue_enabled_ = enabled;
    max_queue_size_ = max_queue_size;
    
    if (enabled) {
        LOG_INFO("启用消息队列，最大大小: " + std::to_string(max_queue_size));
    } else {
        LOG_INFO("禁用消息队列");
        clearMessageQueue();
    }
}

void WebSocketManager::processMessageQueue() {
    if (!queue_enabled_ || !isConnected()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    while (!message_queue_.empty()) {
        const QueuedMessage& queued_msg = message_queue_.top();
        
        bool sent = false;
        if (queued_msg.type == MessageType::TEXT) {
            sent = datalink_->sendText(queued_msg.data);
        } else if (queued_msg.type == MessageType::BINARY) {
            std::vector<uint8_t> binary_data(queued_msg.data.begin(), queued_msg.data.end());
            sent = datalink_->sendBinary(binary_data);
        }
        
        if (sent) {
            messages_sent_success_++;
            LOG_DEBUG("队列消息发送成功: " + queued_msg.data);
        } else {
            messages_sent_failed_++;
            LOG_ERROR("队列消息发送失败: " + queued_msg.data);
            break;  // 发送失败，停止处理队列
        }
        
        message_queue_.pop();
    }
}

size_t WebSocketManager::getQueuedMessageCount() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return message_queue_.size();
}

void WebSocketManager::clearMessageQueue() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!message_queue_.empty()) {
        message_queue_.pop();
    }
    LOG_INFO("消息队列已清空");
}

void WebSocketManager::setHeartbeatInterval(int interval_ms) {
    heartbeat_interval_ms_ = interval_ms;
    LOG_INFO("设置心跳间隔: " + std::to_string(interval_ms) + "ms");
}

void WebSocketManager::enableHeartbeat(bool enabled) {
    heartbeat_enabled_ = enabled;
    
    if (enabled) {
        startHeartbeat();
        LOG_INFO("启用心跳，间隔: " + std::to_string(heartbeat_interval_ms_) + "ms");
    } else {
        stopHeartbeat();
        LOG_INFO("禁用心跳");
    }
}

std::string WebSocketManager::getStatistics() const {
    std::ostringstream oss;
    oss << "WebSocket 管理器统计信息:\n";
    oss << "  连接状态: ";
    
    switch (getConnectionState()) {
        case ConnectionState::DISCONNECTED: oss << "已断开"; break;
        case ConnectionState::CONNECTING: oss << "连接中"; break;
        case ConnectionState::CONNECTED: oss << "已连接"; break;
        case ConnectionState::RECONNECTING: oss << "重连中"; break;
        case ConnectionState::ERROR: oss << "错误"; break;
    }
    
    oss << "\n";
    oss << "  发送成功消息数: " << messages_sent_success_ << "\n";
    oss << "  发送失败消息数: " << messages_sent_failed_ << "\n";
    oss << "  接收消息数: " << messages_received_ << "\n";
    oss << "  队列消息数: " << getQueuedMessageCount() << "\n";
    oss << "  心跳状态: " << (heartbeat_enabled_ ? "启用" : "禁用") << "\n";
    
    if (datalink_) {
        oss << "\n" << datalink_->getStatistics();
    }
    
    return oss.str();
}

void WebSocketManager::setConfig(const std::string& key, const std::string& value) {
    if (platform_) {
        platform_->setConfig(key, value);
    }
}

std::string WebSocketManager::getConfig(const std::string& key) const {
    return platform_ ? platform_->getConfig(key) : "";
}

void WebSocketManager::onConnectionStateChanged(ConnectionState state) {
    LOG_INFO("连接状态变化: " + std::to_string(static_cast<int>(state)));
    
    if (state == ConnectionState::CONNECTED) {
        // 连接成功后处理消息队列
        processMessageQueue();
    }
    
    if (connection_callback_) {
        connection_callback_(state);
    }
}

void WebSocketManager::onMessageReceived(const WebSocketMessage& message) {
    messages_received_++;
    LOG_DEBUG("接收消息: " + message.data);
    
    if (message_callback_) {
        message_callback_(message);
    }
}

void WebSocketManager::onError(const std::string& error) {
    LOG_ERROR("WebSocket 错误: " + error);
    
    if (error_callback_) {
        error_callback_(error);
    }
}

void WebSocketManager::startHeartbeat() {
    if (heartbeat_thread_running_) {
        return;
    }
    
    heartbeat_thread_running_ = true;
    heartbeat_thread_ = platform_->createThread(heartbeatThread, this);
}

void WebSocketManager::stopHeartbeat() {
    heartbeat_thread_running_ = false;
    if (heartbeat_thread_) {
        platform_->joinThread(heartbeat_thread_);
        heartbeat_thread_ = nullptr;
    }
}

void WebSocketManager::heartbeatThread(void* arg) {
    WebSocketManager* manager = static_cast<WebSocketManager*>(arg);
    manager->performHeartbeat();
}

void WebSocketManager::performHeartbeat() {
    while (heartbeat_thread_running_) {
        platform_->sleep(heartbeat_interval_ms_);
        
        if (!heartbeat_thread_running_) {
            break;
        }
        
        if (isConnected()) {
            LOG_DEBUG("发送心跳");
            sendPing();
        }
    }
}

} // namespace cross_platform_websocket 