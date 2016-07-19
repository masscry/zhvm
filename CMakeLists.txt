cmake_minimum_required(VERSION 3.0)
project(zhvm)

add_definitions(-std=c++11 -ggdb)

add_subdirectory(./library)
add_subdirectory(./repl)
