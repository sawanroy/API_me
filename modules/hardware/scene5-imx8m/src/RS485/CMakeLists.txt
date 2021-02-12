PROJECT(RS485)


include_directories(
    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS485/
    ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr
)

add_library(${PROJECT_NAME} SHARED
	rs485.c
)


    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        USB

    )

