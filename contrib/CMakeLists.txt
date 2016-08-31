project (cutest)

set (CUTEST_SOURCES
    ./cutest-1.5/CuTest.c
    ./cutest-1.5/CuTest.h
)

add_library(${PROJECT_NAME} ${CUTEST_SOURCES})