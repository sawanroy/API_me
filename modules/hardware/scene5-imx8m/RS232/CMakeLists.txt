PROJECT(RS232)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/rs232.c
)

target_include_directories( 
  ${PROJECT_NAME}
  PUBLIC
  hdr
  PRIVATE
  USB
)
    
target_link_libraries(
  ${PROJECT_NAME}
  USB
)
