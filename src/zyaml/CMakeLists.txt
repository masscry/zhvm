project (zyaml)

include (CTest)

find_package(FLEX REQUIRED)

FLEX_TARGET(zyaml_gen zyaml.lex ${CMAKE_CURRENT_BINARY_DIR}/zyaml.gen.cpp
    COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/zyaml.gen.h)

set (ZYAML_SOURCES
    ../../include/zyaml.h
    zyaml.cpp
    ${FLEX_zyaml_gen_OUTPUTS}
)

include_directories(./include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_CXX_STANDARD 11)

add_definitions(-pedantic -ggdb -Wall)

add_library (${PROJECT_NAME} ${ZYAML_SOURCES})

