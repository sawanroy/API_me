PROJECT(CAN)

add_library(${PROJECT_NAME} SHARED src/can.c)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  hdr
)
