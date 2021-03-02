PROJECT(IWLIB)

add_library(${PROJECT_NAME} SHARED src/iwlib.c)

target_include_directories(${PROJECT_NAME} PUBLIC hdr)
