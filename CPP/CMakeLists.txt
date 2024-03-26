cmake_minimum_required(VERSION 3.16)
project(threadpool_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(threadpool_server main.cpp
        threadpool.cpp
        threadpool.h)
target_link_libraries(threadpool_server pthread)