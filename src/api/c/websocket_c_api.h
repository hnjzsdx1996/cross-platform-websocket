#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @brief WebSocket 连接状态枚举
 */
typedef enum {
    WS_STATE_DISCONNECTED = 0,
    WS_STATE_CONNECTING = 1,
    WS_STATE_CONNECTED = 2,
    WS_STATE_RECONNECTING = 3,
    WS_STATE_ERROR = 4
} ws_connection_state_t;

/**
 * @brief WebSocket 句柄类型
 */
typedef struct websocket_handle* websocket_handle_t;

/**
 * @brief 连接状态回调函数类型
 */
typedef void (*ws_connection_callback_t)(websocket_handle_t handle, ws_connection_state_t state, void* user_data);

/**
 * @brief 消息接收回调函数类型
 */
typedef void (*ws_message_callback_t)(websocket_handle_t handle, const char* message, size_t length, void* user_data);

/**
 * @brief 错误回调函数类型
 */
typedef void (*ws_error_callback_t)(websocket_handle_t handle, const char* error, void* user_data);

/**
 * @brief 创建 WebSocket 句柄
 * @return WebSocket 句柄，失败返回 NULL
 */
websocket_handle_t ws_create(void);

/**
 * @brief 销毁 WebSocket 句柄
 * @param handle WebSocket 句柄
 */
void ws_destroy(websocket_handle_t handle);

/**
 * @brief 初始化 WebSocket
 * @param handle WebSocket 句柄
 * @return 0 表示成功，非 0 表示失败
 */
int ws_initialize(websocket_handle_t handle);

/**
 * @brief 连接到 WebSocket 服务器
 * @param handle WebSocket 句柄
 * @param url 服务器地址
 * @param auto_reconnect 是否自动重连（1 表示是，0 表示否）
 * @return 0 表示成功，非 0 表示失败
 */
int ws_connect(websocket_handle_t handle, const char* url, int auto_reconnect);

/**
 * @brief 断开 WebSocket 连接
 * @param handle WebSocket 句柄
 */
void ws_disconnect(websocket_handle_t handle);

/**
 * @brief 发送文本消息
 * @param handle WebSocket 句柄
 * @param message 消息内容
 * @return 0 表示成功，非 0 表示失败
 */
int ws_send_text(websocket_handle_t handle, const char* message);

/**
 * @brief 发送二进制消息
 * @param handle WebSocket 句柄
 * @param data 二进制数据
 * @param length 数据长度
 * @return 0 表示成功，非 0 表示失败
 */
int ws_send_binary(websocket_handle_t handle, const uint8_t* data, size_t length);

/**
 * @brief 发送 Ping 消息
 * @param handle WebSocket 句柄
 * @return 0 表示成功，非 0 表示失败
 */
int ws_send_ping(websocket_handle_t handle);

/**
 * @brief 检查是否已连接
 * @param handle WebSocket 句柄
 * @return 1 表示已连接，0 表示未连接
 */
int ws_is_connected(websocket_handle_t handle);

/**
 * @brief 获取连接状态
 * @param handle WebSocket 句柄
 * @return 连接状态
 */
ws_connection_state_t ws_get_connection_state(websocket_handle_t handle);

/**
 * @brief 设置连接状态回调
 * @param handle WebSocket 句柄
 * @param callback 回调函数
 * @param user_data 用户数据
 */
void ws_set_connection_callback(websocket_handle_t handle, ws_connection_callback_t callback, void* user_data);

/**
 * @brief 设置消息接收回调
 * @param handle WebSocket 句柄
 * @param callback 回调函数
 * @param user_data 用户数据
 */
void ws_set_message_callback(websocket_handle_t handle, ws_message_callback_t callback, void* user_data);

/**
 * @brief 设置错误回调
 * @param handle WebSocket 句柄
 * @param callback 回调函数
 * @param user_data 用户数据
 */
void ws_set_error_callback(websocket_handle_t handle, ws_error_callback_t callback, void* user_data);

/**
 * @brief 启用消息队列
 * @param handle WebSocket 句柄
 * @param enabled 是否启用（1 表示是，0 表示否）
 * @param max_size 最大队列大小
 */
void ws_enable_message_queue(websocket_handle_t handle, int enabled, size_t max_size);

/**
 * @brief 启用心跳
 * @param handle WebSocket 句柄
 * @param enabled 是否启用（1 表示是，0 表示否）
 * @param interval_ms 心跳间隔（毫秒）
 */
void ws_enable_heartbeat(websocket_handle_t handle, int enabled, int interval_ms);

/**
 * @brief 获取统计信息
 * @param handle WebSocket 句柄
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 实际写入的字符数
 */
size_t ws_get_statistics(websocket_handle_t handle, char* buffer, size_t buffer_size);

/**
 * @brief 设置配置
 * @param handle WebSocket 句柄
 * @param key 配置键
 * @param value 配置值
 */
void ws_set_config(websocket_handle_t handle, const char* key, const char* value);

/**
 * @brief 获取配置
 * @param handle WebSocket 句柄
 * @param key 配置键
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 实际写入的字符数
 */
size_t ws_get_config(websocket_handle_t handle, const char* key, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif 