project(repl)

include_directories(../library)

add_executable(repl main.cpp)
target_link_libraries(repl zhvm)
