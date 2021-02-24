PROJECT(USB)


add_library(${PROJECT_NAME} SHARED
	src/usb.c
    )

target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/ >
$<INSTALL_INTERFACE:/modules/handler/usb/hdr/>  # <prefix>/include/mylib
)
