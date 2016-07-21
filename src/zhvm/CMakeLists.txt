project(zhvm)

set (ZHVM_HEADERS_DIR ../../include)

set (ZHVM_HEADERS
  ${ZHVM_HEADERS_DIR}/zhvm.h
  ${ZHVM_HEADERS_DIR}/zhvm/interpreter.h
  ${ZHVM_HEADERS_DIR}/zhvm/sman.h
  ${ZHVM_HEADERS_DIR}/zhvm/zmem.class.h
  ${ZHVM_HEADERS_DIR}/zhvm/zconst.h
)

set (ZHVM_SOURCES
  interpreter.cpp
  sman.cpp
  zmem.class.cpp
)

add_library(${PROJECT_NAME}
  ${ZHVM_HEADERS}
  ${ZHVM_SOURCES}
)

install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} DESTINATION lib)
install (EXPORT ${PROJECT_NAME} DESTINATION lib)
install (FILES ${ZHVM_HEADERS_DIR}/zhvm.h DESTINATION include)
install (DIRECTORY ${ZHVM_HEADERS_DIR}/zhvm DESTINATION include)