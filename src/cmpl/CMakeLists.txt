project (cmpl)

FLEX_TARGET(cmpllex zhlex.l ${CMAKE_CURRENT_BINARY_DIR}/zhlex.cpp)

set (CMPL_SOURCES
    entry.cpp
    ${FLEX_cmpllex_OUTPUTS}
)

add_executable(${PROJECT_NAME} ${CMPL_SOURCES})
target_link_libraries(${PROJECT_NAME} zhvm)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
