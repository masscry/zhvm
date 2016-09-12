project (cutest)

set (CUTEST_SOURCES
    ./cutest-1.5/CuTest.c
    ./cutest-1.5/CuTest.h
)

add_library(${PROJECT_NAME} ${CUTEST_SOURCES})

if (ZHVM_COMPILE_FLAGS)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${ZHVM_COMPILE_FLAGS})
endif()

if (ZHVM_LINK_FLAGS)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${ZHVM_LINK_FLAGS})
endif()