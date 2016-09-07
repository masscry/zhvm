project(zhvm)

set (ZHVM_HEADERS_DIR ../../include)

if(UNIX)

    FLEX_TARGET(cmplv2lex cmplv2.lex ${CMAKE_CURRENT_BINARY_DIR}/cmplv2.gen.cpp
        COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/cmplv2.gen.h)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

endif(UNIX)
if(WIN32)

    add_definitions(-DYY_NO_UNISTD_H)

    set (FLEX_cmplv2lex_OUTPUTS win32/cmplv2.gen.cpp)

    include_directories(win32/)

endif(WIN32)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (ZHVM_HEADERS
    ${ZHVM_HEADERS_DIR}/zhvm.h
    ${ZHVM_HEADERS_DIR}/zhvm/interpreter.h
    ${ZHVM_HEADERS_DIR}/zhvm/assembler.h
    ${ZHVM_HEADERS_DIR}/zhvm/memory.class.h
    ${ZHVM_HEADERS_DIR}/zhvm/constants.h
    ${ZHVM_HEADERS_DIR}/zhvm/cmplv2.h
    ${ZHVM_HEADERS_DIR}/zhvm/cmplv2.class.h
)

set (ZHVM_SOURCES
    interpreter.cpp
    assembler.cpp
    memory.class.cpp
    cmplv2.class.cpp
    zhtime.cpp
    ${FLEX_cmplv2lex_OUTPUTS}
)

add_library(${PROJECT_NAME}
  ${ZHVM_HEADERS}
  ${ZHVM_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${ZHVM_COMPILE_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${ZHVM_LINK_FLAGS})

#target_link_libraries(${PROJECT_NAME} profiler)

install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION lib)
install (EXPORT ${PROJECT_NAME} DESTINATION lib)
install (FILES ${ZHVM_HEADERS_DIR}/zhvm.h DESTINATION include)
install (DIRECTORY ${ZHVM_HEADERS_DIR}/zhvm DESTINATION include)