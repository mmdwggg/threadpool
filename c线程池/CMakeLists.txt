cmake_minimum_required(VERSION 3.16)
project(c C)

set(CMAKE_C_STANDARD 11)

add_executable(c main.c
        threadpool.c
        threadpool.h)