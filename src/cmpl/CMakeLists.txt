project (cmpl)

set (CMPL_SOURCES
    entry.cpp
)

add_executable(${PROJECT_NAME} ${CMPL_SOURCES})
target_link_libraries(${PROJECT_NAME} zhvm)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)