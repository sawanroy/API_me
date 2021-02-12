PROJECT(USB)


include_directories(hdr)

add_library(${PROJECT_NAME} SHARED
	src/usb.c
    )
