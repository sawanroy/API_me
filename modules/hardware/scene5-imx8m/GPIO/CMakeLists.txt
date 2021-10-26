PROJECT(GPIO)

add_library(${PROJECT_NAME} SHARED src/gpio.c)
set_property(TARGET "${PROJECT_NAME}" PROPERTY VERSION 0.0.1)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  hdr
  PRIVATE
  LIBGPIO
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  LIBGPIO
)

install(TARGETS "${PROJECT_NAME}")