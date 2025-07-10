# WebSocket 框架示例程序

本目录包含跨平台 WebSocket 框架的示例程序和测试代码。

## 文件说明

- `websocket_framework_example.cpp` - C++ API 使用示例
- `websocket_framework_c_api_test.cpp` - C API 使用示例
- `websocket_client.cpp` - 原始 libwebsockets 客户端示例
- `websocket_client_ssl.cpp` - 原始 libwebsockets SSL 客户端示例
- `hello_world.cpp` - 简单的 Hello World 示例

## 构建和运行

### 构建所有示例

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 运行示例程序
./bin/websocket_framework_example
./bin/websocket_framework_c_api_test
```

### 单独构建框架示例

```bash
# 进入 example 目录
cd example

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 运行示例
./bin/websocket_framework_example
./bin/websocket_framework_c_api_test
```

## 示例程序说明

### C++ API 示例 (`websocket_framework_example.cpp`)

演示如何使用 C++ API 进行 WebSocket 通信：

- 创建平台实现和 API 实例
- 设置回调函数
- 连接到 WebSocket 服务器
- 发送文本和二进制消息
- 处理连接状态变化
- 获取统计信息

### C API 示例 (`websocket_framework_c_api_test.cpp`)

演示如何使用 C API 进行 WebSocket 通信：

- 创建 WebSocket 句柄
- 设置 C 风格回调函数
- 连接和消息发送
- 错误处理
- 资源清理

## 测试服务器

示例程序连接到 `ws://echo.websocket.org`，这是一个公共的 WebSocket 回显服务器，会将发送的消息原样返回。

## 注意事项

1. 示例程序使用模拟的 WebSocket 实现，实际网络连接会被模拟
2. 如需真实网络连接，请安装 libwebsockets 库
3. 示例程序包含详细的日志输出，便于调试和学习

## 自定义示例

您可以基于这些示例程序创建自己的 WebSocket 客户端：

1. 复制示例文件
2. 修改服务器地址
3. 实现自定义的消息处理逻辑
4. 添加业务特定的功能

## 故障排除

如果遇到编译问题：

1. 确保已安装 CMake 3.10 或更高版本
2. 检查 C++11 编译器支持
3. 确保 src 目录中的框架代码完整
4. 查看构建日志中的错误信息 