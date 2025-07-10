#include "api/cpp/websocket_api.h"
#include "platform/native_platform.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace cross_platform_websocket;

// 示例回调函数
void onConnectionStateChanged(ConnectionState state) {
    std::cout << "连接状态变化: ";
    switch (state) {
        case ConnectionState::DISCONNECTED:
            std::cout << "已断开";
            break;
        case ConnectionState::CONNECTING:
            std::cout << "连接中";
            break;
        case ConnectionState::CONNECTED:
            std::cout << "已连接";
            break;
        case ConnectionState::RECONNECTING:
            std::cout << "重连中";
            break;
        case ConnectionState::ERROR:
            std::cout << "错误";
            break;
    }
    std::cout << std::endl;
}

void onMessageReceived(const std::string& message) {
    std::cout << "收到消息: " << message << std::endl;
}

void onError(const std::string& error) {
    std::cout << "错误: " << error << std::endl;
}

int main() {
    std::cout << "=== 跨平台 WebSocket 框架示例 ===" << std::endl;
    
    try {
        // 创建平台实现
        auto platform = std::make_shared<NativePlatform>();
        
        // 创建 WebSocket API
        WebSocketAPI api(platform);
        
        // 初始化
        if (!api.initialize()) {
            std::cerr << "初始化失败" << std::endl;
            return -1;
        }
        
        // 设置回调函数
        api.setConnectionCallback(onConnectionStateChanged);
        api.setMessageCallback(onMessageReceived);
        api.setErrorCallback(onError);
        
        // 启用消息队列和心跳
        api.enableMessageQueue(true, 100);
        api.enableHeartbeat(true, 30000);
        
        // 设置日志级别
        api.setLogLevel(LogLevel::DEBUG);
        
        // 连接到 WebSocket 服务器（这里使用一个测试服务器）
        std::string url = "ws://echo.websocket.org";
        std::cout << "正在连接到: " << url << std::endl;
        
        if (!api.connect(url, true)) {
            std::cerr << "连接失败" << std::endl;
            return -1;
        }
        
        // 等待连接建立
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        if (api.isConnected()) {
            std::cout << "连接成功！" << std::endl;
            
            // 发送一些测试消息
            api.sendText("Hello, WebSocket!");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            api.sendText("这是一条中文消息");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // 发送二进制数据
            std::vector<uint8_t> binary_data = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
            api.sendBinary(binary_data);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // 发送 Ping
            api.sendPing();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // 显示统计信息
            std::cout << "\n=== 统计信息 ===" << std::endl;
            std::cout << api.getStatistics() << std::endl;
            
            // 等待一段时间接收消息
            std::cout << "\n等待接收消息..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            // 断开连接
            std::cout << "断开连接..." << std::endl;
            api.disconnect();
            
        } else {
            std::cout << "连接失败" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "示例程序结束" << std::endl;
    return 0;
} 