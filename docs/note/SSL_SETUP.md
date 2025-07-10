# SSL WebSocket 设置说明

## 问题描述

在使用 libwebsockets 进行 SSL WebSocket 连接时，如果不进行 SSL 全局初始化，会出现以下错误：

```
E: SSL_new failed: error:00000063:lib(0):func(0):reason(99)
```

## 解决方案

在 `lws_context_creation_info` 结构体中添加 `LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT` 选项：

```cpp
struct lws_context_creation_info info;
memset(&info, 0, sizeof info);
info.port = CONTEXT_PORT_NO_LISTEN;
info.protocols = protocols;

// 添加 SSL 全局初始化选项，避免 OpenSSL 错误
info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

context = lws_create_context(&info);
```

## 示例程序

项目包含两个 WebSocket 客户端示例：

### 1. 普通 WebSocket 客户端 (`websocket_client`)
- 连接到 `ws.ifelse.io:80`
- 使用明文 WebSocket 连接
- 文件：`example/websocket_client.cpp`

### 2. SSL WebSocket 客户端 (`websocket_client_ssl`)
- 连接到 `echo.websocket.org:443`
- 使用 SSL/TLS 加密连接
- 文件：`example/websocket_client_ssl.cpp`

## 编译和运行

```bash
# 编译所有程序
./script/build.sh

# 运行普通 WebSocket 客户端
./build/websocket_client

# 运行 SSL WebSocket 客户端
./build/websocket_client_ssl
```

## SSL 连接配置

对于 SSL 连接，需要设置以下参数：

```cpp
ccinfo.ssl_connection = LCCSCF_USE_SSL | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
```

- `LCCSCF_USE_SSL`: 启用 SSL 连接
- `LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK`: 跳过服务器证书主机名检查（用于测试）

## 注意事项

1. 确保 CMakeLists.txt 中启用了 SSL 支持：
   ```cmake
   set(LWS_WITH_SSL ON CACHE BOOL "Enable SSL for libwebsockets" FORCE)
   ```

2. 在生产环境中，应该移除 `LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK` 标志，并正确验证服务器证书。

3. SSL 全局初始化只需要在创建 context 时设置一次，所有后续的 SSL 连接都会使用这个初始化。 