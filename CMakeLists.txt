cmake_minimum_required(VERSION 3.0)

project(PCT LANGUAGES C CXX)
add_definitions(-g -std=c++20)

add_executable(pct main.cpp reader.cpp writer.cpp TimeUtils.cpp Shared_Memory_Layout.cpp Shared_Memory_Utils.cpp)
add_executable(wt writer_test.cpp TimeUtils.cpp Shared_Memory_Layout.cpp Shared_Memory_Utils.cpp)
add_executable(rt reader_test.cpp TimeUtils.cpp Shared_Memory_Layout.cpp Shared_Memory_Utils.cpp)


