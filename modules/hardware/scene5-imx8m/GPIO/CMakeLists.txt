PROJECT(GPIO)

add_library(${PROJECT_NAME} SHARED src/gpio.c)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  hdr
  PRIVATE
  LIBGPIO
)

target_link_libraries(
  ${PROJECT_NAME}
  LIBGPIO
)
