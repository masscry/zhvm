project(zhvm)

set (ZHVM_HEADERS
  ../../include/zhvm.h
  ../../include/zhvm/interpreter.h
  ../../include/zhvm/sman.h
  ../../include/zhvm/zmem.class.h
  ../../include/zhvm/zconst.h
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
