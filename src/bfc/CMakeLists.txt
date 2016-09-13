project(bfc)

if(UNIX)

    FLEX_TARGET(bfclex bfc.lex ${CMAKE_CURRENT_BINARY_DIR}/bfc.gen.cpp
        COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/bfc.gen.h)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(./)

endif(UNIX)

set (BFC_SOURCES
    bfc.cpp
    ${FLEX_bfclex_OUTPUTS}
)

add_executable(${PROJECT_NAME} ${BFC_SOURCES})

target_link_libraries(${PROJECT_NAME} zhvm)

if (ZHVM_COMPILE_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${ZHVM_COMPILE_FLAGS})
endif()

if (ZHVM_LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${ZHVM_LINK_FLAGS})
endif()

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
