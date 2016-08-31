project(tests)

set (TESTS_SOURCES
    run-tests.cpp
)

add_executable(${PROJECT_NAME} ${TESTS_SOURCES})
target_link_libraries(${PROJECT_NAME} zhvm cutest)

add_test(NAME test000 COMMAND ${PROJECT_NAME})

set_tests_properties(test000
    PROPERTIES
    FAIL_REGULAR_EXPRESSION
    "[0-9]+[)] .+: .+:[0-9]+: .*&")