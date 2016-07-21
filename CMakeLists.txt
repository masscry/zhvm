cmake_minimum_required(VERSION 3.0)
project(zhvm)

add_definitions(-std=c++11 -O2 -pedantic -Wall -ggdb)

include_directories(./include)

add_subdirectory(./src/zhvm)
add_subdirectory(./src/repl)
add_subdirectory(./src/cmpl)