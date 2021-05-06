PROJECT(RTC)

add_library(${PROJECT_NAME} SHARED src/rtc.c)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  hdr
)
