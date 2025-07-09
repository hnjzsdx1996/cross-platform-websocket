cmake_minimum_required(VERSION 3.10)
project(cross_platform_websocket CXX)

set(CMAKE_CXX_STANDARD 11)

# include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

# source files
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/fake.cpp
)

# generate static library
add_library(${PROJECT_NAME}_static STATIC ${SRC_FILES})

# generate shared library
add_library(${PROJECT_NAME}_shared SHARED ${SRC_FILES})

# example
add_executable(hello_world example/hello_world.cpp)



