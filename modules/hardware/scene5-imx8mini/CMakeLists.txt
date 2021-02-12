
PROJECT(SCENCE5_IMX8_MINI)

include_directories(
    hdr/SIM/
    hdr/RS485/
    hdr/RS232/
    hdr/GPS/
    hdr/SIM/
    hdr/WATCHDOG/
    hdr/GPIO/
    ${CMAKE_SOURCE_DIR}/modules/handler/libgpio/hdr/
    ${CMAKE_SOURCE_DIR}/modules/handler/serial/hdr/
    ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/
    )

add_library(${PROJECT_NAME} SHARED
    src/RS485/rs485.c
    src/RS232/rs232.c
    src/GPS/gps.c
    src/SIM/sim.c
    src/WATCHDOG/watchdog.c
    src/GPIO/gpio.c
	
    ) 
    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)


    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        LIBGPIO
        USB
        SERIAL
    )
