cmake_minimum_required(VERSION 3.10)
project(cross_platform_websocket VERSION 1.0.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找 libwebsockets
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBWEBSOCKETS QUIET libwebsockets)
endif()

if(NOT LIBWEBSOCKETS_FOUND)
    # 如果 pkg-config 找不到，尝试手动查找
    find_path(LIBWEBSOCKETS_INCLUDE_DIR libwebsockets.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_SOURCE_DIR}/../third_party/libwebsockets/include
    )
    
    find_library(LIBWEBSOCKETS_LIBRARY
        NAMES websockets libwebsockets
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_SOURCE_DIR}/../third_party/libwebsockets/lib
    )
    
    if(LIBWEBSOCKETS_INCLUDE_DIR AND LIBWEBSOCKETS_LIBRARY)
        set(LIBWEBSOCKETS_FOUND TRUE)
        set(LIBWEBSOCKETS_INCLUDE_DIRS ${LIBWEBSOCKETS_INCLUDE_DIR})
        set(LIBWEBSOCKETS_LIBRARIES ${LIBWEBSOCKETS_LIBRARY})
    endif()
endif()

if(NOT LIBWEBSOCKETS_FOUND)
    message(WARNING "libwebsockets not found. Using mock implementation.")
    add_definitions(-DUSE_MOCK_WEBSOCKET)
endif()

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/core/datalink
    ${CMAKE_CURRENT_SOURCE_DIR}/business
    ${CMAKE_CURRENT_SOURCE_DIR}/api/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/api/c
)

if(LIBWEBSOCKETS_FOUND)
    include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
endif()

# 平台特定设置
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(PLATFORM_SOURCES
        platform/native_platform.cpp
    )
else()
    set(PLATFORM_SOURCES
        platform/native_platform.cpp
    )
endif()

# 核心组件源文件
set(CORE_SOURCES
    core/logger/logger.cpp
    core/datalink/datalink.cpp
)

# 业务层源文件
set(BUSINESS_SOURCES
    business/websocket_manager.cpp
)

# API 层源文件
set(API_SOURCES
    api/cpp/websocket_api.cpp
    api/c/websocket_c_api.cpp
)

# 所有源文件
set(ALL_SOURCES
    ${PLATFORM_SOURCES}
    ${CORE_SOURCES}
    ${BUSINESS_SOURCES}
    ${API_SOURCES}
)

# 创建静态库
add_library(websocket_framework STATIC ${ALL_SOURCES})

# 设置库的属性
set_target_properties(websocket_framework PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    PUBLIC_HEADER
        platform/platform_interface.h
        platform/native_platform.h
        core/logger/logger.h
        core/datalink/datalink.h
        business/websocket_manager.h
        api/cpp/websocket_api.h
        api/c/websocket_c_api.h
)

# 链接库
if(LIBWEBSOCKETS_FOUND)
    target_link_libraries(websocket_framework ${LIBWEBSOCKETS_LIBRARIES})
endif()

# 平台特定链接
if(WIN32)
    target_link_libraries(websocket_framework ws2_32)
else()
    target_link_libraries(websocket_framework pthread)
endif()

# 注意：示例程序已移动到 ../example/ 目录
# 如需构建示例程序，请参考 ../example/ 目录下的构建说明

# 安装规则
install(TARGETS websocket_framework
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/websocket_framework
)

# 安装头文件
install(FILES
    platform/platform_interface.h
    platform/native_platform.h
    core/logger/logger.h
    core/datalink/datalink.h
    business/websocket_manager.h
    api/cpp/websocket_api.h
    api/c/websocket_c_api.h
    DESTINATION include/websocket_framework
)

# 创建 pkg-config 文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/websocket_framework.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/websocket_framework.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/websocket_framework.pc
    DESTINATION lib/pkgconfig
)

# 打印配置信息
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
if(LIBWEBSOCKETS_FOUND)
    message(STATUS "libwebsockets found: ${LIBWEBSOCKETS_LIBRARIES}")
else()
    message(STATUS "libwebsockets not found, using mock implementation")
endif() 