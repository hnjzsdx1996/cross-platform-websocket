#include "api/c/websocket_c_api.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

// C 回调函数
void on_connection_state_changed(websocket_handle_t handle, ws_connection_state_t state, void* user_data) {
    std::cout << "C API: 连接状态变化: ";
    switch (state) {
        case WS_STATE_DISCONNECTED:
            std::cout << "已断开";
            break;
        case WS_STATE_CONNECTING:
            std::cout << "连接中";
            break;
        case WS_STATE_CONNECTED:
            std::cout << "已连接";
            break;
        case WS_STATE_RECONNECTING:
            std::cout << "重连中";
            break;
        case WS_STATE_ERROR:
            std::cout << "错误";
            break;
    }
    std::cout << std::endl;
}

void on_message_received(websocket_handle_t handle, const char* message, size_t length, void* user_data) {
    std::cout << "C API: 收到消息: " << std::string(message, length) << std::endl;
}

void on_error(websocket_handle_t handle, const char* error, void* user_data) {
    std::cout << "C API: 错误: " << error << std::endl;
}

int main() {
    std::cout << "=== C API 测试程序 ===" << std::endl;
    
    // 创建 WebSocket 句柄
    websocket_handle_t handle = ws_create();
    if (!handle) {
        std::cerr << "创建 WebSocket 句柄失败" << std::endl;
        return -1;
    }
    
    // 初始化
    if (ws_initialize(handle) != 0) {
        std::cerr << "初始化失败" << std::endl;
        ws_destroy(handle);
        return -1;
    }
    
    // 设置回调函数
    ws_set_connection_callback(handle, on_connection_state_changed, nullptr);
    ws_set_message_callback(handle, on_message_received, nullptr);
    ws_set_error_callback(handle, on_error, nullptr);
    
    // 启用消息队列和心跳
    ws_enable_message_queue(handle, 1, 100);
    ws_enable_heartbeat(handle, 1, 30000);
    
    // 连接到 WebSocket 服务器
    const char* url = "ws://echo.websocket.org";
    std::cout << "正在连接到: " << url << std::endl;
    
    if (ws_connect(handle, url, 1) != 0) {
        std::cerr << "连接失败" << std::endl;
        ws_destroy(handle);
        return -1;
    }
    
    // 等待连接建立
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    if (ws_is_connected(handle)) {
        std::cout << "连接成功！" << std::endl;
        
        // 发送一些测试消息
        ws_send_text(handle, "Hello from C API!");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        ws_send_text(handle, "这是一条中文消息");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 发送二进制数据
        uint8_t binary_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
        ws_send_binary(handle, binary_data, sizeof(binary_data));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 发送 Ping
        ws_send_ping(handle);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 显示统计信息
        char stats_buffer[1024];
        size_t stats_len = ws_get_statistics(handle, stats_buffer, sizeof(stats_buffer));
        if (stats_len > 0) {
            std::cout << "\n=== 统计信息 ===" << std::endl;
            std::cout << std::string(stats_buffer, stats_len) << std::endl;
        }
        
        // 等待一段时间接收消息
        std::cout << "\n等待接收消息..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // 断开连接
        std::cout << "断开连接..." << std::endl;
        ws_disconnect(handle);
        
    } else {
        std::cout << "连接失败" << std::endl;
    }
    
    // 销毁句柄
    ws_destroy(handle);
    
    std::cout << "C API 测试程序结束" << std::endl;
    return 0;
} 