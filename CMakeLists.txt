cmake_minimum_required(VERSION 3.17)
project(copyer)

set(CMAKE_CXX_STANDARD 20)

# json
add_subdirectory(json)
include_directories(json/include)

add_executable(copyer main.cpp)