cmake_minimum_required(VERSION 3.0)
project(zhvm)

add_definitions(-std=c++11 -O2 -pedantic -Wall)

add_subdirectory(./zhvm)
add_subdirectory(./repl)
