project(exec)

set (EXEC_SOURCES
    exec.cpp
)

add_executable(${PROJECT_NAME} ${EXEC_SOURCES})

target_link_libraries(${PROJECT_NAME} zhvm)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${ZHVM_COMPILE_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${ZHVM_LINK_FLAGS})

install (TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
