#!/bin/bash
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

# 编译静态库
ninja cross_platform_websocket_static

# 编译动态库
ninja cross_platform_websocket_shared

# 编译测试程序
ninja hello_world
ninja websocket_client
ninja websocket_client_ssl

cd -
echo "编译完成"
echo "静态库: $BUILD_DIR/libcross_platform_websocket_static.a"
echo "动态库: $BUILD_DIR/libcross_platform_websocket_shared.dylib"
echo "测试程序: $BUILD_DIR/hello_world $BUILD_DIR/websocket_client $BUILD_DIR/websocket_client_ssl" 