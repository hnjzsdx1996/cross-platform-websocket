cmake_minimum_required(VERSION 3.10)
project(cross_platform_websocket CXX)

set(CMAKE_CXX_STANDARD 11)

# third_party
set(LWS_WITH_SSL ON CACHE BOOL "Enable SSL for libwebsockets" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/libwebsockets)

# include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/libwebsockets/include
)

# source files
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/fake.cpp
)

# generate static library
add_library(${PROJECT_NAME}_static STATIC ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}_static websockets)

# generate shared library
add_library(${PROJECT_NAME}_shared SHARED ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}_shared websockets)

# example
add_executable(hello_world example/hello_world.cpp)

add_executable(websocket_client example/websocket_client.cpp)
target_link_libraries(websocket_client websockets)
target_include_directories(websocket_client PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libwebsockets/include)



