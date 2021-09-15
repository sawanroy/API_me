PROJECT(GPS)

add_library(${PROJECT_NAME} SHARED src/gps.c)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC 
  hdr
  PRIVATE
  SERIAL 
)

target_link_libraries(
  ${PROJECT_NAME}
  SERIAL
)

install(TARGETS "${PROJECT_NAME}")