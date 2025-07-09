#!/bin/bash
set -e

BUILD_TYPE=${1:-Release}
BUILD_DIR=../build

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 生成Makefile
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# 编译静态库
make cross_platform_websocket_static

# 编译动态库
make cross_platform_websocket_shared

# 编译测试程序
make hello_world
make websocket_client

cd -
echo "编译完成"
echo "静态库: $BUILD_DIR/libcross_platform_websocket_static.a"
echo "动态库: $BUILD_DIR/libcross_platform_websocket_shared.so"
echo "测试程序: $BUILD_DIR/demo" 