 PROJECT(LIBGPIO)


add_library(${PROJECT_NAME} SHARED
 	src/libgpio.c
     )
set_property(TARGET "${PROJECT_NAME}" PROPERTY VERSION 0.0.1)
     
target_include_directories(${PROJECT_NAME} PUBLIC hdr/)

INSTALL(TARGETS "${PROJECT_NAME}")