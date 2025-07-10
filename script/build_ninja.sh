#!/bin/bash
set -e

BUILD_TYPE=${1:-Release}
BUILD_DIR=../build

echo "=== 跨平台 WebSocket 框架构建脚本 (Ninja) ==="
echo "构建类型: $BUILD_TYPE"
echo "构建目录: $BUILD_DIR"

# 检查 Ninja 是否安装
if ! command -v ninja &> /dev/null; then
    echo "错误: 未找到 ninja 命令"
    echo "请安装 ninja: brew install ninja"
    exit 1
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 生成 Ninja 构建文件
echo "生成 Ninja 构建文件..."
cmake -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# 构建 websocket_framework 库
echo "构建 websocket_framework 库..."
ninja websocket_framework

# 构建示例程序
echo "构建示例程序..."
ninja websocket_framework_example
ninja websocket_framework_c_api_test
ninja hello_world

# 构建 libwebsockets 示例
echo "构建 websocket_client..."
ninja websocket_client

echo "构建 websocket_client_ssl..."
ninja websocket_client_ssl

# 回到原目录
cd -

echo ""
echo "=== 构建完成 ==="
echo "框架库: $BUILD_DIR/lib/libwebsocket_framework.a"
echo "示例程序:"
echo "  - $BUILD_DIR/bin/websocket_framework_example"
echo "  - $BUILD_DIR/bin/websocket_framework_c_api_test"
echo "  - $BUILD_DIR/bin/hello_world"
echo "  - $BUILD_DIR/bin/websocket_client"
echo "  - $BUILD_DIR/bin/websocket_client_ssl"

echo ""
echo "运行示例:"
echo "  cd $BUILD_DIR/bin"
echo "  ./websocket_framework_example"
echo "  ./websocket_framework_c_api_test"
echo "  ./hello_world" 