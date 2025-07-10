#pragma once

#include "platform_interface.h"
#include <thread>
#include <mutex>
#include <map>
#include <chrono>
#include <random>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#endif

namespace cross_platform_websocket {

/**
 * @brief Native 平台实现
 * 
 * 使用 libwebsockets 和标准 C++ 库实现平台能力
 */
class NativePlatform : public PlatformInterface {
public:
    NativePlatform();
    ~NativePlatform() override;
    
    // ==================== 日志接口实现 ====================
    void logInfo(const std::string& message) override;
    void logError(const std::string& message) override;
    void logDebug(const std::string& message) override;
    void logWarning(const std::string& message) override;
    
    // ==================== WebSocket 接口实现 ====================
    bool websocketConnect(const std::string& url) override;
    bool websocketSend(const std::string& message) override;
    void websocketClose() override;
    bool websocketIsConnected() override;
    
    // ==================== 线程接口实现 ====================
    void* createThread(void (*func)(void*), void* arg) override;
    void joinThread(void* thread) override;
    unsigned long getCurrentThreadId() override;
    
    // ==================== 配置接口实现 ====================
    std::string getConfig(const std::string& key) override;
    void setConfig(const std::string& key, const std::string& value) override;
    bool hasConfig(const std::string& key) override;
    
    // ==================== 工具接口实现 ====================
    uint64_t getCurrentTimestamp() override;
    int generateRandomNumber(int min, int max) override;
    void sleep(int milliseconds) override;

private:
    // WebSocket 相关
    void* websocket_context_;
    void* websocket_connection_;
    bool is_connected_;
    std::mutex websocket_mutex_;
    
    // 配置相关
    std::map<std::string, std::string> config_map_;
    std::mutex config_mutex_;
    
    // 随机数生成器
    std::random_device random_device_;
    std::mt19937 random_generator_;
    
    // 初始化网络库
    bool initializeNetwork();
    void cleanupNetwork();
};

} // namespace cross_platform_websocket 