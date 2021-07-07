 PROJECT(LIBGPIO)


add_library(${PROJECT_NAME} SHARED
 	src/libgpio.c
     )
set_property(TARGET "${PROJECT_NAME}" PROPERTY VERSION 0.0.1)
     
target_include_directories(${PROJECT_NAME} PUBLIC hdr/)

target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/handler/libgpio/hdr/ >
$<INSTALL_INTERFACE:/modules/handler/libgpio/hdr/>  # <prefix>/include/mylib
)

INSTALL(TARGETS "${PROJECT_NAME}")