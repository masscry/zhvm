project(zhvm)

set (ZHVM_HEADERS
  zhvm.h
  sman.h
  zmem.class.h
  zconst.h
)

set (ZHVM_SOURCES
  zhvm.cpp
  sman.cpp
  zmem.class.cpp
)

add_library(${PROJECT_NAME}
  ${ZHVM_HEADERS}
  ${ZHVM_SOURCES}
)
