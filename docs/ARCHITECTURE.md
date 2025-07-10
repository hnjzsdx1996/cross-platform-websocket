# 跨平台 WebSocket 框架架构设计

## 整体架构图

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                   外部调用层                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│  Java (JNI)  │  Python (SWIG)  │  Go (CGO)  │  JavaScript (Emscripten)  │  C++  │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                  平台胶水层                                      │
├─────────────────────────────────────────────────────────────────────────────────┤
│ platform/java/    │ platform/python/   │ platform/go/      │ platform/web/      │
│ - JNIWrapper.cpp  │ - swig_interface.i │ - cgo_bridge.go   │ - emscripten.cpp   │
│ - JavaBridge.java │ - python_bridge.py │ - cgo_wrapper.c   │ - js_bridge.js     │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                   C API 层                                      │
├─────────────────────────────────────────────────────────────────────────────────┤
│                    cross_platform_websocket.h (C 风格接口)                      │
│  - websocket_create_context()                                                   │
│  - websocket_connect()                                                          │
│  - websocket_send_message()                                                     │
│  - websocket_close()                                                            │
│  - websocket_destroy_context()                                                  │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                  C++ 接口层                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│                    CrossPlatformWebSocket (C++ 类接口)                          │
│  - class WebSocketManager                                                       │
│  - class ConnectionManager                                                      │
│  - class MessageHandler                                                         │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                 业务管理层                                       │
├─────────────────────────────────────────────────────────────────────────────────┤
│                              BusinessManager                                    │
│  - ConnectionManager: 连接管理                                                   │
│  - MessageManager: 消息收发管理                                                 │
│  - EventManager: 事件管理                                                       │
│  - ConfigManager: 配置管理                                                      │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                  基础组件层                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│  DataLink  │  Logger  │  ThreadPool  │  Config  │  EventLoop  │  Utils  │  ...  │
│            │          │              │          │             │         │       │
│ ┌─────────┐│ ┌───────┐│ ┌───────────┐│ ┌───────┐│ ┌─────────┐│ ┌──────┐│       │
│ │WebSocket││ │Plog   ││ │ThreadPool ││ │JSON   ││ │EventLoop││ │Base64││       │
│ │Client   ││ │Logger ││ │Manager    ││ │Parser ││ │Manager ││ │Utils ││       │
│ │Server   ││ │JS     ││ │TaskQueue  ││ │Config ││ │Timer   ││ │Crypto││       │
│ │SSL      ││ │Logger ││ │Worker     ││ │Loader ││ │Async   ││ │Utils ││       │
│ └─────────┘│ └───────┘│ └───────────┘│ └───────┘│ └─────────┘│ └──────┘│       │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                  平台抽象层                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│  Platform Abstraction Layer (PAL)                                              │
│  - PlatformInterface: 平台能力注入接口                                          │
│  - JSPlatformBridge: JavaScript 平台桥接                                        │
│  - NativePlatformBridge: 原生平台桥接                                           │
└─────────────────────────────────────────────────────────────────────────────────┘
                                        │
┌─────────────────────────────────────────────────────────────────────────────────┐
│                                  底层实现层                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│  libwebsockets (C++)  │  Emscripten (JS)  │  System APIs  │  Third Party       │
│  - WebSocket Client   │  - WebSocket API  │  - Threading  │  - OpenSSL         │
│  - WebSocket Server   │  - Console API    │  - File I/O   │  - PLOG            │
│  - SSL/TLS Support    │  - DOM API        │  - Network    │  - JSON Library    │
└─────────────────────────────────────────────────────────────────────────────────┘
```

## 详细目录结构

```
cross-platform-websocket/
├── src/
│   ├── core/                           # 核心组件
│   │   ├── datalink/                   # 数据链路层
│   │   │   ├── websocket_client.cpp    # WebSocket 客户端
│   │   │   ├── websocket_server.cpp    # WebSocket 服务端
│   │   │   ├── ssl_manager.cpp         # SSL 管理
│   │   │   └── datalink_interface.h    # 数据链路接口
│   │   ├── logger/                     # 日志系统
│   │   │   ├── plog_logger.cpp         # PLOG 日志实现
│   │   │   ├── js_logger.cpp           # JS 日志实现
│   │   │   └── logger_interface.h      # 日志接口
│   │   ├── thread/                     # 线程管理
│   │   │   ├── thread_pool.cpp         # 线程池
│   │   │   ├── task_queue.cpp          # 任务队列
│   │   │   └── thread_interface.h      # 线程接口
│   │   ├── config/                     # 配置管理
│   │   │   ├── config_manager.cpp      # 配置管理器
│   │   │   ├── json_parser.cpp         # JSON 解析
│   │   │   └── config_interface.h      # 配置接口
│   │   └── utils/                      # 工具类
│   │       ├── base64.cpp              # Base64 编码
│   │       ├── crypto_utils.cpp        # 加密工具
│   │       └── utils_interface.h       # 工具接口
│   ├── business/                       # 业务逻辑层
│   │   ├── connection_manager.cpp      # 连接管理
│   │   ├── message_manager.cpp         # 消息管理
│   │   ├── event_manager.cpp           # 事件管理
│   │   └── business_manager.h          # 业务管理器
│   ├── api/                            # API 层
│   │   ├── cpp/                        # C++ API
│   │   │   ├── websocket_manager.cpp   # WebSocket 管理器
│   │   │   └── websocket_manager.h     # WebSocket 管理器头文件
│   │   └── c/                          # C API
│   │       ├── websocket_api.cpp       # C 风格 API 实现
│   │       └── websocket_api.h         # C 风格 API 头文件
│   └── platform/                       # 平台抽象层
│       ├── platform_interface.h        # 平台接口定义
│       ├── js_platform_bridge.cpp      # JS 平台桥接
│       └── native_platform_bridge.cpp  # 原生平台桥接
├── platform/                           # 平台胶水层
│   ├── java/                           # Java 平台
│   │   ├── jni_wrapper.cpp             # JNI 包装器
│   │   ├── java_bridge.java            # Java 桥接类
│   │   └── build.gradle                # Gradle 构建脚本
│   ├── python/                         # Python 平台
│   │   ├── swig_interface.i            # SWIG 接口文件
│   │   ├── python_bridge.py            # Python 桥接
│   │   └── setup.py                    # Python 构建脚本
│   ├── go/                             # Go 平台
│   │   ├── cgo_bridge.go               # CGO 桥接
│   │   ├── cgo_wrapper.c               # CGO 包装器
│   │   └── go.mod                      # Go 模块文件
│   └── web/                            # Web 平台
│       ├── emscripten_bridge.cpp       # Emscripten 桥接
│       ├── js_bridge.js                # JavaScript 桥接
│       └── webpack.config.js           # Webpack 配置
├── examples/                           # 示例代码
│   ├── cpp_example.cpp                 # C++ 示例
│   ├── java_example.java               # Java 示例
│   ├── python_example.py               # Python 示例
│   ├── go_example.go                   # Go 示例
│   └── web_example.html                # Web 示例
├── tests/                              # 测试代码
│   ├── unit_tests/                     # 单元测试
│   ├── integration_tests/              # 集成测试
│   └── platform_tests/                 # 平台测试
├── docs/                               # 文档
│   ├── api_reference.md                # API 参考
│   ├── platform_guides/                # 平台使用指南
│   └── examples/                       # 示例文档
├── scripts/                            # 构建脚本
│   ├── build.sh                        # 主构建脚本
│   ├── build_java.sh                   # Java 构建脚本
│   ├── build_python.sh                 # Python 构建脚本
│   ├── build_go.sh                     # Go 构建脚本
│   └── build_web.sh                    # Web 构建脚本
├── third_party/                        # 第三方库
│   ├── libwebsockets/                  # libwebsockets 库
│   ├── plog/                           # PLOG 日志库
│   └── json/                           # JSON 库
├── CMakeLists.txt                      # CMake 配置
├── README.md                           # 项目说明
└── ARCHITECTURE.md                     # 架构文档
```

## 核心接口设计

### 1. 平台抽象接口 (PlatformInterface)

```cpp
// src/platform/platform_interface.h
class PlatformInterface {
public:
    virtual ~PlatformInterface() = default;
    
    // 日志接口
    virtual void log_info(const std::string& message) = 0;
    virtual void log_error(const std::string& message) = 0;
    virtual void log_debug(const std::string& message) = 0;
    
    // WebSocket 接口 (JS 平台注入)
    virtual bool websocket_connect(const std::string& url) = 0;
    virtual bool websocket_send(const std::string& message) = 0;
    virtual void websocket_close() = 0;
    
    // 线程接口
    virtual void* create_thread(void (*func)(void*), void* arg) = 0;
    virtual void join_thread(void* thread) = 0;
    
    // 配置接口
    virtual std::string get_config(const std::string& key) = 0;
    virtual void set_config(const std::string& key, const std::string& value) = 0;
};
```

### 2. C++ API 接口

```cpp
// src/api/cpp/websocket_manager.h
class WebSocketManager {
public:
    static WebSocketManager* create();
    static void destroy(WebSocketManager* manager);
    
    // 连接管理
    bool connect(const std::string& url, const std::string& protocol = "");
    void disconnect();
    bool is_connected() const;
    
    // 消息收发
    bool send_message(const std::string& message);
    bool send_binary(const void* data, size_t size);
    
    // 事件回调
    void set_on_open(std::function<void()> callback);
    void set_on_message(std::function<void(const std::string&)> callback);
    void set_on_close(std::function<void(int, const std::string&)> callback);
    void set_on_error(std::function<void(const std::string&)> callback);
    
    // 配置
    void set_config(const std::string& key, const std::string& value);
    std::string get_config(const std::string& key) const;
};
```

### 3. C API 接口

```cpp
// src/api/c/websocket_api.h
#ifdef __cplusplus
extern "C" {
#endif

// 上下文管理
typedef struct websocket_context* websocket_context_t;
websocket_context_t websocket_create_context();
void websocket_destroy_context(websocket_context_t ctx);

// 连接管理
bool websocket_connect(websocket_context_t ctx, const char* url, const char* protocol);
void websocket_disconnect(websocket_context_t ctx);
bool websocket_is_connected(websocket_context_t ctx);

// 消息收发
bool websocket_send_message(websocket_context_t ctx, const char* message);
bool websocket_send_binary(websocket_context_t ctx, const void* data, size_t size);

// 事件回调
typedef void (*websocket_on_open_callback)(websocket_context_t ctx);
typedef void (*websocket_on_message_callback)(websocket_context_t ctx, const char* message);
typedef void (*websocket_on_close_callback)(websocket_context_t ctx, int code, const char* reason);
typedef void (*websocket_on_error_callback)(websocket_context_t ctx, const char* error);

void websocket_set_on_open(websocket_context_t ctx, websocket_on_open_callback callback);
void websocket_set_on_message(websocket_context_t ctx, websocket_on_message_callback callback);
void websocket_set_on_close(websocket_context_t ctx, websocket_on_close_callback callback);
void websocket_set_on_error(websocket_context_t ctx, websocket_on_error_callback callback);

// 配置
void websocket_set_config(websocket_context_t ctx, const char* key, const char* value);
const char* websocket_get_config(websocket_context_t ctx, const char* key);

#ifdef __cplusplus
}
#endif
```

## 平台桥接设计

### 1. JavaScript 平台桥接

```cpp
// src/platform/js_platform_bridge.cpp
class JSPlatformBridge : public PlatformInterface {
private:
    // Emscripten 注入的 JS 函数
    EM_JS(void, js_log_info, (const char* message), {
        console.info(UTF8ToString(message));
    });
    
    EM_JS(void, js_log_error, (const char* message), {
        console.error(UTF8ToString(message));
    });
    
    EM_JS(bool, js_websocket_connect, (const char* url), {
        return Module._js_websocket_connect(UTF8ToString(url));
    });
    
    EM_JS(bool, js_websocket_send, (const char* message), {
        return Module._js_websocket_send(UTF8ToString(message));
    });

public:
    void log_info(const std::string& message) override {
        js_log_info(message.c_str());
    }
    
    void log_error(const std::string& message) override {
        js_log_error(message.c_str());
    }
    
    bool websocket_connect(const std::string& url) override {
        return js_websocket_connect(url.c_str());
    }
    
    bool websocket_send(const std::string& message) override {
        return js_websocket_send(message.c_str());
    }
    
    // ... 其他实现
};
```

### 2. Java 平台桥接

```cpp
// platform/java/jni_wrapper.cpp
extern "C" {
    JNIEXPORT jlong JNICALL Java_com_example_WebSocketBridge_createContext(JNIEnv* env, jobject obj) {
        websocket_context_t ctx = websocket_create_context();
        return reinterpret_cast<jlong>(ctx);
    }
    
    JNIEXPORT jboolean JNICALL Java_com_example_WebSocketBridge_connect(JNIEnv* env, jobject obj, jlong ctx, jstring url) {
        const char* url_str = env->GetStringUTFChars(url, nullptr);
        bool result = websocket_connect(reinterpret_cast<websocket_context_t>(ctx), url_str);
        env->ReleaseStringUTFChars(url, url_str);
        return result;
    }
    
    JNIEXPORT jboolean JNICALL Java_com_example_WebSocketBridge_sendMessage(JNIEnv* env, jobject obj, jlong ctx, jstring message) {
        const char* msg_str = env->GetStringUTFChars(message, nullptr);
        bool result = websocket_send_message(reinterpret_cast<websocket_context_t>(ctx), msg_str);
        env->ReleaseStringUTFChars(message, msg_str);
        return result;
    }
    
    // ... 其他 JNI 函数
}
```

### 3. Python 平台桥接

```cpp
// platform/python/swig_interface.i
%module websocket_bridge

%{
#include "websocket_api.h"
%}

%include "websocket_api.h"

// Python 包装类
%pythoncode %{
class WebSocketClient:
    def __init__(self):
        self.ctx = websocket_create_context()
        self._setup_callbacks()
    
    def _setup_callbacks(self):
        def on_open_wrapper(ctx):
            if hasattr(self, 'on_open'):
                self.on_open()
        
        def on_message_wrapper(ctx, message):
            if hasattr(self, 'on_message'):
                self.on_message(message.decode('utf-8'))
        
        websocket_set_on_open(self.ctx, on_open_wrapper)
        websocket_set_on_message(self.ctx, on_message_wrapper)
    
    def connect(self, url, protocol=""):
        return websocket_connect(self.ctx, url.encode('utf-8'), protocol.encode('utf-8'))
    
    def send_message(self, message):
        return websocket_send_message(self.ctx, message.encode('utf-8'))
    
    def disconnect(self):
        websocket_disconnect(self.ctx)
    
    def __del__(self):
        websocket_destroy_context(self.ctx)
%}
```

### 4. Go 平台桥接

```go
// platform/go/cgo_bridge.go
package websocket

/*
#cgo CFLAGS: -I../../../src/api/c
#cgo LDFLAGS: -L../../../build -lwebsocket_api
#include "websocket_api.h"
*/
import "C"
import "unsafe"

type WebSocketClient struct {
    ctx C.websocket_context_t
}

func NewWebSocketClient() *WebSocketClient {
    return &WebSocketClient{
        ctx: C.websocket_create_context(),
    }
}

func (w *WebSocketClient) Connect(url, protocol string) bool {
    cUrl := C.CString(url)
    cProtocol := C.CString(protocol)
    defer C.free(unsafe.Pointer(cUrl))
    defer C.free(unsafe.Pointer(cProtocol))
    
    return bool(C.websocket_connect(w.ctx, cUrl, cProtocol))
}

func (w *WebSocketClient) SendMessage(message string) bool {
    cMessage := C.CString(message)
    defer C.free(unsafe.Pointer(cMessage))
    
    return bool(C.websocket_send_message(w.ctx, cMessage))
}

func (w *WebSocketClient) Disconnect() {
    C.websocket_disconnect(w.ctx)
}

func (w *WebSocketClient) Close() {
    C.websocket_destroy_context(w.ctx)
}
```

## 构建系统设计

### 1. 主构建脚本

```bash
#!/bin/bash
# scripts/build.sh

set -e

BUILD_TYPE=${1:-Release}
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 生成构建文件
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G Ninja "$PROJECT_DIR"

# 编译核心库
ninja cross_platform_websocket_static
ninja cross_platform_websocket_shared

# 编译示例程序
ninja hello_world
ninja websocket_client
ninja websocket_client_ssl

# 编译平台桥接
ninja java_bridge
ninja python_bridge
ninja go_bridge
ninja web_bridge

cd -
echo "编译完成"
```

### 2. 平台特定构建脚本

```bash
#!/bin/bash
# scripts/build_java.sh

# 编译 JNI 库
javac -cp . platform/java/JavaBridge.java
javah -cp . com.example.WebSocketBridge

# 编译 JNI 包装器
g++ -shared -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" \
    platform/java/jni_wrapper.cpp -o libwebsocket_jni.dylib \
    -Lbuild -lwebsocket_api
```

## 使用示例

### 1. C++ 使用示例

```cpp
// examples/cpp_example.cpp
#include "websocket_manager.h"

int main() {
    auto* manager = WebSocketManager::create();
    
    manager->set_on_open([]() {
        std::cout << "连接已建立" << std::endl;
    });
    
    manager->set_on_message([](const std::string& message) {
        std::cout << "收到消息: " << message << std::endl;
    });
    
    manager->connect("ws://echo.websocket.org");
    
    // 发送消息
    manager->send_message("Hello, WebSocket!");
    
    // 等待一段时间
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    manager->disconnect();
    WebSocketManager::destroy(manager);
    
    return 0;
}
```

### 2. Java 使用示例

```java
// examples/java_example.java
import com.example.WebSocketBridge;

public class WebSocketExample {
    public static void main(String[] args) {
        WebSocketBridge bridge = new WebSocketBridge();
        long ctx = bridge.createContext();
        
        bridge.setOnOpen(ctx, () -> {
            System.out.println("连接已建立");
        });
        
        bridge.setOnMessage(ctx, (message) -> {
            System.out.println("收到消息: " + message);
        });
        
        bridge.connect(ctx, "ws://echo.websocket.org", "");
        bridge.sendMessage(ctx, "Hello, WebSocket!");
        
        // 等待一段时间
        Thread.sleep(5000);
        
        bridge.disconnect(ctx);
        bridge.destroyContext(ctx);
    }
}
```

### 3. Python 使用示例

```python
# examples/python_example.py
import websocket_bridge

def on_open():
    print("连接已建立")

def on_message(message):
    print(f"收到消息: {message}")

client = websocket_bridge.WebSocketClient()
client.on_open = on_open
client.on_message = on_message

client.connect("ws://echo.websocket.org", "")
client.send_message("Hello, WebSocket!")

import time
time.sleep(5)

client.disconnect()
```

### 4. Go 使用示例

```go
// examples/go_example.go
package main

import (
    "fmt"
    "time"
    "./websocket"
)

func main() {
    client := websocket.NewWebSocketClient()
    defer client.Close()
    
    // 连接
    if client.Connect("ws://echo.websocket.org", "") {
        fmt.Println("连接成功")
        
        // 发送消息
        if client.SendMessage("Hello, WebSocket!") {
            fmt.Println("消息发送成功")
        }
        
        // 等待一段时间
        time.Sleep(5 * time.Second)
        
        client.Disconnect()
    }
}
```

### 5. JavaScript 使用示例

```html
<!-- examples/web_example.html -->
<!DOCTYPE html>
<html>
<head>
    <title>WebSocket Example</title>
</head>
<body>
    <div id="output"></div>
    
    <script src="websocket_bridge.js"></script>
    <script>
        const client = new WebSocketBridge();
        
        client.onOpen = function() {
            console.log("连接已建立");
            document.getElementById('output').innerHTML += "<p>连接已建立</p>";
        };
        
        client.onMessage = function(message) {
            console.log("收到消息:", message);
            document.getElementById('output').innerHTML += "<p>收到消息: " + message + "</p>";
        };
        
        client.connect("ws://echo.websocket.org", "");
        client.sendMessage("Hello, WebSocket!");
        
        setTimeout(() => {
            client.disconnect();
        }, 5000);
    </script>
</body>
</html>
```

这个架构设计提供了：

1. **清晰的层次结构**：从底层实现到高层 API 的完整分层
2. **跨平台支持**：通过统一的 C API 和平台胶水层支持多种语言
3. **可扩展性**：模块化设计便于添加新功能和平台支持
4. **易用性**：为每种语言提供了符合其习惯的 API 设计
5. **可维护性**：清晰的接口定义和文档化设计 