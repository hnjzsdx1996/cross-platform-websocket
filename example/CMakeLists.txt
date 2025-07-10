cmake_minimum_required(VERSION 3.10)
project(websocket_framework_examples)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查 websocket_framework target 是否存在
if(NOT TARGET websocket_framework)
    message(FATAL_ERROR "websocket_framework target not found. Please build from the root directory using: cmake .. && make")
endif()

message(STATUS "Using websocket_framework target from main project")

# 创建 C++ API 示例程序
add_executable(websocket_framework_example websocket_framework_example.cpp)
target_link_libraries(websocket_framework_example websocket_framework)
target_include_directories(websocket_framework_example PRIVATE ../src)

# 创建 C API 测试程序
add_executable(websocket_framework_c_api_test websocket_framework_c_api_test.cpp)
target_link_libraries(websocket_framework_c_api_test websocket_framework)
target_include_directories(websocket_framework_c_api_test PRIVATE ../src)

# 创建 Hello World 示例程序
add_executable(hello_world hello_world.cpp)

# 创建原始 libwebsockets 客户端示例
add_executable(websocket_client websocket_client.cpp)
target_include_directories(websocket_client PRIVATE ../third_party/libwebsockets/include)
target_link_libraries(websocket_client websockets)
message(STATUS "链接 libwebsockets 库: websockets")

# 创建原始 libwebsockets SSL 客户端示例
add_executable(websocket_client_ssl websocket_client_ssl.cpp)
target_include_directories(websocket_client_ssl PRIVATE ../third_party/libwebsockets/include)
target_link_libraries(websocket_client_ssl websockets)

# 设置输出目录
set_target_properties(websocket_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(websocket_client_ssl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 设置输出目录
set_target_properties(websocket_framework_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(websocket_framework_c_api_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(hello_world PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(TARGET websocket_client)
    set_target_properties(websocket_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

if(TARGET websocket_client_ssl)
    set_target_properties(websocket_client_ssl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# 打印构建信息
message(STATUS "构建 WebSocket 框架示例程序")
message(STATUS "C++ 示例: websocket_framework_example")
message(STATUS "C API 示例: websocket_framework_c_api_test")
message(STATUS "Hello World 示例: hello_world")
message(STATUS "原始 libwebsockets 客户端: websocket_client")
message(STATUS "原始 libwebsockets SSL 客户端: websocket_client_ssl") 