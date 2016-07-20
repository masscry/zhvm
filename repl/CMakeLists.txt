project(repl)

include_directories(../zhvm)

set (REPL_SOURCES
  main.cpp
)

add_executable(${PROJECT_NAME}
  ${REPL_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  zhvm
)
