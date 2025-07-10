#include "websocket_api.h"
#include <memory>

namespace cross_platform_websocket {

WebSocketAPI::WebSocketAPI(std::shared_ptr<PlatformInterface> platform)
    : platform_(platform) {
}

WebSocketAPI::~WebSocketAPI() {
    disconnect();
}

bool WebSocketAPI::initialize() {
    try {
        // 创建日志器
        logger_ = std::shared_ptr<Logger>(new Logger(platform_));
        
        // 设置全局日志实例
        g_logger = logger_;
        
        // 创建 WebSocket 管理器
        manager_ = std::unique_ptr<WebSocketManager>(new WebSocketManager(platform_, logger_));
        
        // 初始化管理器
        if (!manager_->initialize()) {
            LOG_ERROR("WebSocket 管理器初始化失败");
            return false;
        }
        
        // 设置内部回调
        manager_->setConnectionCallback(
            [this](ConnectionState state) { onConnectionStateChanged(state); });
        manager_->setMessageCallback(
            [this](const WebSocketMessage& msg) { onMessageReceived(msg); });
        manager_->setErrorCallback(
            [this](const std::string& error) { onError(error); });
        
        LOG_INFO("WebSocket API 初始化成功");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("WebSocket API 初始化失败: " + std::string(e.what()));
        return false;
    }
}

bool WebSocketAPI::connect(const std::string& url, bool auto_reconnect) {
    if (!manager_) {
        LOG_ERROR("WebSocket API 未初始化");
        return false;
    }
    
    LOG_INFO("API: 连接到 " + url);
    return manager_->connect(url, auto_reconnect);
}

void WebSocketAPI::disconnect() {
    if (manager_) {
        manager_->disconnect();
    }
    LOG_INFO("API: 断开连接");
}

bool WebSocketAPI::sendText(const std::string& message) {
    if (!manager_) {
        LOG_ERROR("WebSocket API 未初始化");
        return false;
    }
    
    LOG_DEBUG("API: 发送文本消息: " + message);
    return manager_->sendText(message);
}

bool WebSocketAPI::sendBinary(const std::vector<uint8_t>& data) {
    if (!manager_) {
        LOG_ERROR("WebSocket API 未初始化");
        return false;
    }
    
    LOG_DEBUG("API: 发送二进制消息，大小: " + std::to_string(data.size()) + " 字节");
    return manager_->sendBinary(data);
}

bool WebSocketAPI::sendPing() {
    if (!manager_) {
        LOG_ERROR("WebSocket API 未初始化");
        return false;
    }
    
    LOG_DEBUG("API: 发送 Ping 消息");
    return manager_->sendPing();
}

bool WebSocketAPI::isConnected() const {
    return manager_ && manager_->isConnected();
}

ConnectionState WebSocketAPI::getConnectionState() const {
    return manager_ ? manager_->getConnectionState() : ConnectionState::DISCONNECTED;
}

void WebSocketAPI::setConnectionCallback(std::function<void(ConnectionState)> callback) {
    user_connection_callback_ = callback;
}

void WebSocketAPI::setMessageCallback(std::function<void(const std::string&)> callback) {
    user_message_callback_ = callback;
}

void WebSocketAPI::setErrorCallback(std::function<void(const std::string&)> callback) {
    user_error_callback_ = callback;
}

void WebSocketAPI::enableMessageQueue(bool enabled, size_t max_size) {
    if (manager_) {
        manager_->enableMessageQueue(enabled, max_size);
    }
}

void WebSocketAPI::enableHeartbeat(bool enabled, int interval_ms) {
    if (manager_) {
        if (enabled) {
            manager_->setHeartbeatInterval(interval_ms);
        }
        manager_->enableHeartbeat(enabled);
    }
}

std::string WebSocketAPI::getStatistics() const {
    if (manager_) {
        return manager_->getStatistics();
    }
    return "WebSocket API 未初始化";
}

void WebSocketAPI::setLogLevel(LogLevel level) {
    if (logger_) {
        logger_->setLogLevel(level);
    }
}

void WebSocketAPI::setConfig(const std::string& key, const std::string& value) {
    if (manager_) {
        manager_->setConfig(key, value);
    }
}

std::string WebSocketAPI::getConfig(const std::string& key) const {
    return manager_ ? manager_->getConfig(key) : "";
}

void WebSocketAPI::onConnectionStateChanged(ConnectionState state) {
    LOG_INFO("API: 连接状态变化: " + std::to_string(static_cast<int>(state)));
    
    if (user_connection_callback_) {
        user_connection_callback_(state);
    }
}

void WebSocketAPI::onMessageReceived(const WebSocketMessage& message) {
    LOG_DEBUG("API: 接收消息: " + message.data);
    
    if (user_message_callback_) {
        user_message_callback_(message.data);
    }
}

void WebSocketAPI::onError(const std::string& error) {
    LOG_ERROR("API: 错误: " + error);
    
    if (user_error_callback_) {
        user_error_callback_(error);
    }
}

} // namespace cross_platform_websocket 