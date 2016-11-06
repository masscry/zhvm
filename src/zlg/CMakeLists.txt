project(zlg)

if(UNIX)

    find_package(BISON REQUIRED)

    BISON_TARGET(zlgy zlg.y ${CMAKE_CURRENT_BINARY_DIR}/zlgy.gen.cpp COMPILE_FLAGS -rall )

    FLEX_TARGET(zlglex zlg.lex ${CMAKE_CURRENT_BINARY_DIR}/zlg.gen.cpp
        COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/zlg.gen.h)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(./)

endif(UNIX)

set (ZLG_SOURCES
    ast.class.cpp
    context.class.cpp
    ${FLEX_zlglex_OUTPUTS}
    ${BISON_zlgy_OUTPUTS}
)

add_library(${PROJECT_NAME} STATIC ${ZLG_SOURCES})

target_link_libraries(${PROJECT_NAME} zhvm)

if (ZHVM_COMPILE_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${ZHVM_COMPILE_FLAGS})
endif()

if (ZHVM_LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${ZHVM_LINK_FLAGS})
endif()

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
