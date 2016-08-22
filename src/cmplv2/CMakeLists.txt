project (cmplv2)


set (CMPLV2_SOURCES
    entry.cpp
)


add_executable(${PROJECT_NAME} ${CMPLV2_SOURCES})
target_link_libraries(${PROJECT_NAME} zhvm)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)
