#include "datalink.h"
#include <sstream>
#include <algorithm>

namespace cross_platform_websocket {

DataLink::DataLink(std::shared_ptr<PlatformInterface> platform, 
                   std::shared_ptr<Logger> logger)
    : platform_(platform)
    , logger_(logger)
    , connection_state_(ConnectionState::DISCONNECTED)
    , auto_reconnect_enabled_(false)
    , max_reconnect_attempts_(5)
    , reconnect_interval_ms_(1000)
    , current_reconnect_attempts_(0)
    , messages_sent_(0)
    , messages_received_(0)
    , bytes_sent_(0)
    , bytes_received_(0)
    , connection_start_time_(0)
    , reconnect_thread_(nullptr)
    , reconnect_thread_running_(false) {
    
    LOG_INFO("数据链路层初始化完成");
}

DataLink::~DataLink() {
    disconnect();
    stopReconnectTimer();
}

bool DataLink::connect(const std::string& url) {
    if (connection_state_ == ConnectionState::CONNECTED) {
        LOG_WARNING("WebSocket 已经连接");
        return true;
    }
    
    if (connection_state_ == ConnectionState::CONNECTING) {
        LOG_WARNING("WebSocket 正在连接中");
        return false;
    }
    
    server_url_ = url;
    updateConnectionState(ConnectionState::CONNECTING);
    
    LOG_INFO("正在连接到: " + url);
    
    // 使用平台接口建立连接
    if (platform_->websocketConnect(url)) {
        handleConnectionSuccess();
        return true;
    } else {
        handleConnectionError("连接失败");
        return false;
    }
}

void DataLink::disconnect() {
    if (connection_state_ == ConnectionState::DISCONNECTED) {
        return;
    }
    
    LOG_INFO("断开 WebSocket 连接");
    
    // 停止重连
    stopReconnectTimer();
    
    // 使用平台接口关闭连接
    platform_->websocketClose();
    
    updateConnectionState(ConnectionState::DISCONNECTED);
}

bool DataLink::sendText(const std::string& message) {
    if (!isConnected()) {
        LOG_ERROR("WebSocket 未连接，无法发送消息");
        return false;
    }
    
    if (platform_->websocketSend(message)) {
        messages_sent_++;
        bytes_sent_ += message.length();
        LOG_DEBUG("发送文本消息: " + message);
        return true;
    } else {
        LOG_ERROR("发送文本消息失败");
        return false;
    }
}

bool DataLink::sendBinary(const std::vector<uint8_t>& data) {
    if (!isConnected()) {
        LOG_ERROR("WebSocket 未连接，无法发送二进制消息");
        return false;
    }
    
    // 将二进制数据转换为字符串（这里简化处理）
    std::string binary_str(data.begin(), data.end());
    
    if (platform_->websocketSend(binary_str)) {
        messages_sent_++;
        bytes_sent_ += data.size();
        LOG_DEBUG("发送二进制消息，大小: " + std::to_string(data.size()) + " 字节");
        return true;
    } else {
        LOG_ERROR("发送二进制消息失败");
        return false;
    }
}

bool DataLink::sendPing() {
    if (!isConnected()) {
        LOG_ERROR("WebSocket 未连接，无法发送 Ping");
        return false;
    }
    
    // 发送 Ping 消息（这里简化处理）
    if (platform_->websocketSend("PING")) {
        LOG_DEBUG("发送 Ping 消息");
        return true;
    } else {
        LOG_ERROR("发送 Ping 消息失败");
        return false;
    }
}

ConnectionState DataLink::getConnectionState() const {
    return connection_state_;
}

bool DataLink::isConnected() const {
    return connection_state_ == ConnectionState::CONNECTED;
}

void DataLink::setConnectionCallback(ConnectionCallback callback) {
    connection_callback_ = callback;
}

void DataLink::setMessageCallback(MessageCallback callback) {
    message_callback_ = callback;
}

void DataLink::setErrorCallback(ErrorCallback callback) {
    error_callback_ = callback;
}

void DataLink::setAutoReconnect(bool enabled, int max_attempts, int interval_ms) {
    auto_reconnect_enabled_ = enabled;
    max_reconnect_attempts_ = max_attempts;
    reconnect_interval_ms_ = interval_ms;
    
    if (enabled) {
        LOG_INFO("启用自动重连，最大重连次数: " + std::to_string(max_attempts) + 
                 "，重连间隔: " + std::to_string(interval_ms) + "ms");
    } else {
        LOG_INFO("禁用自动重连");
        stopReconnectTimer();
    }
}

std::string DataLink::getStatistics() const {
    std::ostringstream oss;
    oss << "连接统计信息:\n";
    oss << "  连接状态: ";
    
    switch (connection_state_) {
        case ConnectionState::DISCONNECTED: oss << "已断开"; break;
        case ConnectionState::CONNECTING: oss << "连接中"; break;
        case ConnectionState::CONNECTED: oss << "已连接"; break;
        case ConnectionState::RECONNECTING: oss << "重连中"; break;
        case ConnectionState::ERROR: oss << "错误"; break;
    }
    
    oss << "\n";
    oss << "  发送消息数: " << messages_sent_ << "\n";
    oss << "  接收消息数: " << messages_received_ << "\n";
    oss << "  发送字节数: " << bytes_sent_ << "\n";
    oss << "  接收字节数: " << bytes_received_ << "\n";
    
    if (connection_start_time_ > 0) {
        uint64_t current_time = platform_->getCurrentTimestamp();
        uint64_t duration = current_time - connection_start_time_;
        oss << "  连接时长: " << duration << "ms\n";
    }
    
    return oss.str();
}

void DataLink::updateConnectionState(ConnectionState new_state) {
    if (connection_state_ != new_state) {
        connection_state_ = new_state;
        LOG_INFO("连接状态更新: " + std::to_string(static_cast<int>(new_state)));
        
        if (connection_callback_) {
            connection_callback_(new_state);
        }
    }
}

void DataLink::handleConnectionSuccess() {
    connection_start_time_ = platform_->getCurrentTimestamp();
    current_reconnect_attempts_ = 0;
    updateConnectionState(ConnectionState::CONNECTED);
    LOG_INFO("WebSocket 连接成功");
}

void DataLink::handleConnectionError(const std::string& error) {
    updateConnectionState(ConnectionState::ERROR);
    LOG_ERROR("WebSocket 连接错误: " + error);
    
    if (error_callback_) {
        error_callback_(error);
    }
    
    // 如果启用了自动重连，尝试重连
    if (auto_reconnect_enabled_ && current_reconnect_attempts_ < max_reconnect_attempts_) {
        startReconnectTimer();
    }
}

void DataLink::handleMessageReceived(const WebSocketMessage& message) {
    messages_received_++;
    bytes_received_ += message.data.length();
    
    LOG_DEBUG("接收消息: " + message.data);
    
    if (message_callback_) {
        message_callback_(message);
    }
}

void DataLink::startReconnectTimer() {
    if (reconnect_thread_running_) {
        return;
    }
    
    current_reconnect_attempts_++;
    updateConnectionState(ConnectionState::RECONNECTING);
    
    LOG_INFO("开始第 " + std::to_string(current_reconnect_attempts_) + 
             " 次重连尝试");
    
    reconnect_thread_running_ = true;
    reconnect_thread_ = platform_->createThread(reconnectThread, this);
}

void DataLink::stopReconnectTimer() {
    reconnect_thread_running_ = false;
    if (reconnect_thread_) {
        platform_->joinThread(reconnect_thread_);
        reconnect_thread_ = nullptr;
    }
}

void DataLink::attemptReconnect() {
    if (!reconnect_thread_running_) {
        return;
    }
    
    // 等待重连间隔
    platform_->sleep(reconnect_interval_ms_);
    
    if (!reconnect_thread_running_) {
        return;
    }
    
    LOG_INFO("尝试重连到: " + server_url_);
    
    if (platform_->websocketConnect(server_url_)) {
        handleConnectionSuccess();
        reconnect_thread_running_ = false;
    } else {
        if (current_reconnect_attempts_ >= max_reconnect_attempts_) {
            LOG_ERROR("达到最大重连次数，停止重连");
            reconnect_thread_running_ = false;
            updateConnectionState(ConnectionState::ERROR);
        } else {
            // 继续下一次重连
            current_reconnect_attempts_++;
            attemptReconnect();
        }
    }
}

void DataLink::reconnectThread(void* arg) {
    DataLink* datalink = static_cast<DataLink*>(arg);
    datalink->attemptReconnect();
}

} // namespace cross_platform_websocket 