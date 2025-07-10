#include "native_platform.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef USE_MOCK_WEBSOCKET
// 使用模拟实现，不包含 libwebsockets
#else
#ifdef _WIN32
#include <libwebsockets.h>
#else
#include <libwebsockets.h>
#endif
#endif

namespace cross_platform_websocket {

NativePlatform::NativePlatform() 
    : websocket_context_(nullptr)
    , websocket_connection_(nullptr)
    , is_connected_(false)
    , random_generator_(random_device_()) {
    
    initializeNetwork();
}

NativePlatform::~NativePlatform() {
    websocketClose();
    cleanupNetwork();
}

// ==================== 日志接口实现 ====================

void NativePlatform::logInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void NativePlatform::logError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void NativePlatform::logDebug(const std::string& message) {
    std::cout << "[DEBUG] " << message << std::endl;
}

void NativePlatform::logWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

// ==================== WebSocket 接口实现 ====================

bool NativePlatform::websocketConnect(const std::string& url) {
    std::lock_guard<std::mutex> lock(websocket_mutex_);
    
    if (is_connected_) {
        logWarning("WebSocket 已经连接");
        return true;
    }
    
    // 这里应该使用 libwebsockets 实现 WebSocket 连接
    // 为了演示，我们只是模拟连接过程
    logInfo("正在连接到: " + url);
    
    // 模拟连接延迟
    sleep(100);
    
    is_connected_ = true;
    logInfo("WebSocket 连接成功");
    return true;
}

bool NativePlatform::websocketSend(const std::string& message) {
    std::lock_guard<std::mutex> lock(websocket_mutex_);
    
    if (!is_connected_) {
        logError("WebSocket 未连接，无法发送消息");
        return false;
    }
    
    // 这里应该使用 libwebsockets 发送消息
    logInfo("发送消息: " + message);
    return true;
}

void NativePlatform::websocketClose() {
    std::lock_guard<std::mutex> lock(websocket_mutex_);
    
    if (!is_connected_) {
        return;
    }
    
    // 这里应该使用 libwebsockets 关闭连接
    logInfo("关闭 WebSocket 连接");
    is_connected_ = false;
}

bool NativePlatform::websocketIsConnected() {
    std::lock_guard<std::mutex> lock(websocket_mutex_);
    return is_connected_;
}

// ==================== 线程接口实现 ====================

void* NativePlatform::createThread(void (*func)(void*), void* arg) {
#ifdef _WIN32
    return CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, nullptr);
#else
    pthread_t* thread = new pthread_t;
    if (pthread_create(thread, nullptr, (void* (*)(void*))func, arg) != 0) {
        delete thread;
        return nullptr;
    }
    return thread;
#endif
}

void NativePlatform::joinThread(void* thread) {
#ifdef _WIN32
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
#else
    if (thread) {
        pthread_t* pthread = static_cast<pthread_t*>(thread);
        pthread_join(*pthread, nullptr);
        delete pthread;
    }
#endif
}

unsigned long NativePlatform::getCurrentThreadId() {
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return (unsigned long)pthread_self();
#endif
}

// ==================== 配置接口实现 ====================

std::string NativePlatform::getConfig(const std::string& key) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    auto it = config_map_.find(key);
    return (it != config_map_.end()) ? it->second : "";
}

void NativePlatform::setConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_map_[key] = value;
}

bool NativePlatform::hasConfig(const std::string& key) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    return config_map_.find(key) != config_map_.end();
}

// ==================== 工具接口实现 ====================

uint64_t NativePlatform::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int NativePlatform::generateRandomNumber(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(random_generator_);
}

void NativePlatform::sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

// ==================== 私有方法实现 ====================

bool NativePlatform::initializeNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        logError("WSAStartup 失败");
        return false;
    }
#endif
    
    // 初始化 libwebsockets
    // 这里应该调用 lws_set_log_level 等初始化函数
    logInfo("网络库初始化成功");
    return true;
}

void NativePlatform::cleanupNetwork() {
#ifdef _WIN32
    WSACleanup();
#endif
    logInfo("网络库清理完成");
}

} // namespace cross_platform_websocket 