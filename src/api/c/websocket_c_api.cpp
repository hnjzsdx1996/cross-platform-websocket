#include "websocket_c_api.h"
#include "../cpp/websocket_api.h"
#include "../../platform/native_platform.h"
#include <memory>
#include <string>
#include <cstring>

// WebSocket 句柄结构体
struct websocket_handle {
    std::shared_ptr<cross_platform_websocket::PlatformInterface> platform;
    std::unique_ptr<cross_platform_websocket::WebSocketAPI> api;
    
    // C 回调函数
    ws_connection_callback_t connection_callback;
    ws_message_callback_t message_callback;
    ws_error_callback_t error_callback;
    void* user_data;
    
    websocket_handle() 
        : connection_callback(nullptr)
        , message_callback(nullptr)
        , error_callback(nullptr)
        , user_data(nullptr) {}
};

// 状态转换函数
static ws_connection_state_t convert_state(cross_platform_websocket::ConnectionState state) {
    switch (state) {
        case cross_platform_websocket::ConnectionState::DISCONNECTED:
            return WS_STATE_DISCONNECTED;
        case cross_platform_websocket::ConnectionState::CONNECTING:
            return WS_STATE_CONNECTING;
        case cross_platform_websocket::ConnectionState::CONNECTED:
            return WS_STATE_CONNECTED;
        case cross_platform_websocket::ConnectionState::RECONNECTING:
            return WS_STATE_RECONNECTING;
        case cross_platform_websocket::ConnectionState::ERROR:
            return WS_STATE_ERROR;
        default:
            return WS_STATE_DISCONNECTED;
    }
}

// C++ 到 C 的回调包装函数
static void on_connection_state_changed(cross_platform_websocket::ConnectionState state, void* user_data) {
    websocket_handle_t handle = static_cast<websocket_handle_t>(user_data);
    if (handle && handle->connection_callback) {
        handle->connection_callback(handle, convert_state(state), handle->user_data);
    }
}

static void on_message_received(const std::string& message, void* user_data) {
    websocket_handle_t handle = static_cast<websocket_handle_t>(user_data);
    if (handle && handle->message_callback) {
        handle->message_callback(handle, message.c_str(), message.length(), handle->user_data);
    }
}

static void on_error(const std::string& error, void* user_data) {
    websocket_handle_t handle = static_cast<websocket_handle_t>(user_data);
    if (handle && handle->error_callback) {
        handle->error_callback(handle, error.c_str(), handle->user_data);
    }
}

// C API 实现
extern "C" {

websocket_handle_t ws_create(void) {
    try {
        websocket_handle_t handle = new websocket_handle();
        
        // 创建 Native 平台实现
        handle->platform = std::make_shared<cross_platform_websocket::NativePlatform>();
        
        // 创建 WebSocket API
        handle->api = std::unique_ptr<cross_platform_websocket::WebSocketAPI>(
            new cross_platform_websocket::WebSocketAPI(handle->platform));
        
        return handle;
    } catch (...) {
        return nullptr;
    }
}

void ws_destroy(websocket_handle_t handle) {
    if (handle) {
        delete handle;
    }
}

int ws_initialize(websocket_handle_t handle) {
    if (!handle || !handle->api) {
        return -1;
    }
    
    try {
        return handle->api->initialize() ? 0 : -1;
    } catch (...) {
        return -1;
    }
}

int ws_connect(websocket_handle_t handle, const char* url, int auto_reconnect) {
    if (!handle || !handle->api || !url) {
        return -1;
    }
    
    try {
        return handle->api->connect(url, auto_reconnect != 0) ? 0 : -1;
    } catch (...) {
        return -1;
    }
}

void ws_disconnect(websocket_handle_t handle) {
    if (handle && handle->api) {
        try {
            handle->api->disconnect();
        } catch (...) {
            // 忽略异常
        }
    }
}

int ws_send_text(websocket_handle_t handle, const char* message) {
    if (!handle || !handle->api || !message) {
        return -1;
    }
    
    try {
        return handle->api->sendText(message) ? 0 : -1;
    } catch (...) {
        return -1;
    }
}

int ws_send_binary(websocket_handle_t handle, const uint8_t* data, size_t length) {
    if (!handle || !handle->api || !data) {
        return -1;
    }
    
    try {
        std::vector<uint8_t> binary_data(data, data + length);
        return handle->api->sendBinary(binary_data) ? 0 : -1;
    } catch (...) {
        return -1;
    }
}

int ws_send_ping(websocket_handle_t handle) {
    if (!handle || !handle->api) {
        return -1;
    }
    
    try {
        return handle->api->sendText("PING") ? 0 : -1;
    } catch (...) {
        return -1;
    }
}

int ws_is_connected(websocket_handle_t handle) {
    if (!handle || !handle->api) {
        return 0;
    }
    
    try {
        return handle->api->isConnected() ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

ws_connection_state_t ws_get_connection_state(websocket_handle_t handle) {
    if (!handle || !handle->api) {
        return WS_STATE_DISCONNECTED;
    }
    
    try {
        return convert_state(handle->api->getConnectionState());
    } catch (...) {
        return WS_STATE_ERROR;
    }
}

void ws_set_connection_callback(websocket_handle_t handle, ws_connection_callback_t callback, void* user_data) {
    if (handle) {
        handle->connection_callback = callback;
        handle->user_data = user_data;
        
        if (handle->api) {
            try {
                handle->api->setConnectionCallback(
                    [handle](cross_platform_websocket::ConnectionState state) {
                        on_connection_state_changed(state, handle);
                    });
            } catch (...) {
                // 忽略异常
            }
        }
    }
}

void ws_set_message_callback(websocket_handle_t handle, ws_message_callback_t callback, void* user_data) {
    if (handle) {
        handle->message_callback = callback;
        handle->user_data = user_data;
        
        if (handle->api) {
            try {
                handle->api->setMessageCallback(
                    [handle](const std::string& message) {
                        on_message_received(message, handle);
                    });
            } catch (...) {
                // 忽略异常
            }
        }
    }
}

void ws_set_error_callback(websocket_handle_t handle, ws_error_callback_t callback, void* user_data) {
    if (handle) {
        handle->error_callback = callback;
        handle->user_data = user_data;
        
        if (handle->api) {
            try {
                handle->api->setErrorCallback(
                    [handle](const std::string& error) {
                        on_error(error, handle);
                    });
            } catch (...) {
                // 忽略异常
            }
        }
    }
}

void ws_enable_message_queue(websocket_handle_t handle, int enabled, size_t max_size) {
    if (handle && handle->api) {
        try {
            handle->api->enableMessageQueue(enabled != 0, max_size);
        } catch (...) {
            // 忽略异常
        }
    }
}

void ws_enable_heartbeat(websocket_handle_t handle, int enabled, int interval_ms) {
    if (handle && handle->api) {
        try {
            handle->api->enableHeartbeat(enabled != 0, interval_ms);
        } catch (...) {
            // 忽略异常
        }
    }
}

size_t ws_get_statistics(websocket_handle_t handle, char* buffer, size_t buffer_size) {
    if (!handle || !handle->api || !buffer || buffer_size == 0) {
        return 0;
    }
    
    try {
        std::string stats = handle->api->getStatistics();
        size_t copy_size = std::min(stats.length(), buffer_size - 1);
        std::strncpy(buffer, stats.c_str(), copy_size);
        buffer[copy_size] = '\0';
        return copy_size;
    } catch (...) {
        return 0;
    }
}

void ws_set_config(websocket_handle_t handle, const char* key, const char* value) {
    if (handle && handle->api && key && value) {
        try {
            handle->api->setConfig(key, value);
        } catch (...) {
            // 忽略异常
        }
    }
}

size_t ws_get_config(websocket_handle_t handle, const char* key, char* buffer, size_t buffer_size) {
    if (!handle || !handle->api || !key || !buffer || buffer_size == 0) {
        return 0;
    }
    
    try {
        std::string value = handle->api->getConfig(key);
        size_t copy_size = std::min(value.length(), buffer_size - 1);
        std::strncpy(buffer, value.c_str(), copy_size);
        buffer[copy_size] = '\0';
        return copy_size;
    } catch (...) {
        return 0;
    }
}

} // extern "C" 