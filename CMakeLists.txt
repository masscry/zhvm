cmake_minimum_required(VERSION 3.0)
project(zhvm)

find_package (FLEX REQUIRED)

set(CMAKE_CXX_STANDARD 11)

if(UNIX)
add_definitions(-ggdb -pedantic -Wall)
add_definitions(-DUNIX)
endif(UNIX)

include_directories(./include)

add_subdirectory(./src/zyaml)
add_subdirectory(./src/zhvm)
add_subdirectory(./src/repl)
add_subdirectory(./src/cmplv2)
