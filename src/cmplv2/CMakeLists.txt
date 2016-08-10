project (cmplv2)

FLEX_TARGET(cmplv2lex cmplv2.lex ${CMAKE_CURRENT_BINARY_DIR}/cmplv2.cpp
    COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/cmplv2.gen.h)

set (CMPLV2_SOURCES
    entry.cpp
    cmplv2.class.cpp
    ${FLEX_cmplv2lex_OUTPUTS}
)

include_directories(./)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${CMPLV2_SOURCES})
target_link_libraries(${PROJECT_NAME} zhvm)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
