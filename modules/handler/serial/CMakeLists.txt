PROJECT(SERIAL)


add_library(${PROJECT_NAME} SHARED
	src/serial.c
    )

target_include_directories(${PROJECT_NAME} PUBLIC hdr/
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/handler/serial/hdr/ >
$<INSTALL_INTERFACE:/modules/handler/serial/hdr/>  # <prefix>/include/mylib
)

install(TARGETS "${PROJECT_NAME}")