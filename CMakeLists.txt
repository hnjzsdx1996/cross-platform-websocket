cmake_minimum_required(VERSION 3.10)
project(cross_platform_websocket VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 构建选项
option(BUILD_FRAMEWORK "Build WebSocket framework library" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_LIBWEBSOCKETS "Build libwebsockets from source" OFF)

# 打印构建信息
message(STATUS "=== Cross-Platform WebSocket Framework ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build framework: ${BUILD_FRAMEWORK}")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "Build libwebsockets: ${BUILD_LIBWEBSOCKETS}")

# 构建 libwebsockets（默认启用）
message(STATUS "Building libwebsockets from source...")
set(LWS_WITH_SSL ON CACHE BOOL "Enable SSL for libwebsockets" FORCE)
set(LWS_WITH_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(LWS_WITH_STATIC ON CACHE BOOL "Build static library" FORCE)
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "Disable test apps" FORCE)
set(LWS_WITHOUT_TEST_SERVER ON CACHE BOOL "Disable test server" FORCE)
set(LWS_WITHOUT_TEST_PING ON CACHE BOOL "Disable test ping" FORCE)
set(LWS_WITHOUT_TEST_CLIENT ON CACHE BOOL "Disable test client" FORCE)
# 禁用严格的编译器警告
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter -Wno-error=unused-variable -Wno-error=format-pedantic -Wno-format-pedantic")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/libwebsockets)

# 构建框架库
if(BUILD_FRAMEWORK)
    message(STATUS "Building WebSocket framework...")
    add_subdirectory(src)
endif()

# 构建示例程序
if(BUILD_EXAMPLES)
    message(STATUS "Building example programs...")
    add_subdirectory(example)
endif()

# 安装规则
if(BUILD_FRAMEWORK)
    install(TARGETS websocket_framework
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/websocket_framework
    )
    
    # 安装头文件
    install(FILES
        src/platform/platform_interface.h
        src/platform/native_platform.h
        src/core/logger/logger.h
        src/core/datalink/datalink.h
        src/business/websocket_manager.h
        src/api/cpp/websocket_api.h
        src/api/c/websocket_c_api.h
        DESTINATION include/websocket_framework
    )
    
    # 安装 pkg-config 文件
    install(FILES
        ${CMAKE_BINARY_DIR}/src/websocket_framework.pc
        DESTINATION lib/pkgconfig
    )
endif()

# 安装示例程序
if(BUILD_EXAMPLES)
    install(TARGETS 
        websocket_framework_example
        websocket_framework_c_api_test
        DESTINATION bin
    )
endif()

# 打印完成信息
message(STATUS "")
message(STATUS "=== Build Configuration Complete ===")
if(BUILD_FRAMEWORK)
    message(STATUS "Framework library: libwebsocket_framework.a")
    message(STATUS "Headers: include/websocket_framework/")
endif()
if(BUILD_EXAMPLES)
    message(STATUS "Example programs: bin/websocket_framework_example")
    message(STATUS "C API test: bin/websocket_framework_c_api_test")
endif()
message(STATUS "")
message(STATUS "To build: make -j$(nproc)")
message(STATUS "To install: make install")
message(STATUS "To run examples: ./bin/websocket_framework_example")



