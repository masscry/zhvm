cmake_minimum_required(VERSION 3.0)
project(zhvm)

find_package (BISON REQUIRED)
find_package (FLEX REQUIRED)

add_definitions(-std=c++11 -O2 -pedantic -Wall -Wextra -ggdb)

include_directories(./include)

add_subdirectory(./src/zhvm)
add_subdirectory(./src/repl)
add_subdirectory(./src/cmpl)
