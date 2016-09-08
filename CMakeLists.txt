cmake_minimum_required(VERSION 3.0)

project(zhvm)

configure_file(CTestCustom.cmake CTestCustom.cmake)

include (CTest)

if(UNIX)
  find_package (FLEX REQUIRED)
endif(UNIX)

set(CMAKE_CXX_STANDARD 11)

if(UNIX)
  add_definitions(-O2)
  add_definitions(-DNDEBUG)

  add_definitions(-DUNIX)
#  add_definitions(-DWITHGPERFTOOLS)

 set (ZHVM_COMPILE_FLAGS "-pedantic -Wall -m32")
#  set (ZHVM_COMPILE_FLAGS "-ggdb -g3 -pedantic -Wall -m32")
  set (ZHVM_LINK_FLAGS "-m32")
endif(UNIX)

include_directories(./include)

add_subdirectory(./src/zyaml)
add_subdirectory(./src/zhvm)
add_subdirectory(./src/repl)
add_subdirectory(./src/cmplv2)
add_subdirectory(./src/exec)

include_directories(./contrib/cutest-1.5)
add_subdirectory(./contrib)
add_subdirectory(./tests)
