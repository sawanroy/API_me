
PROJECT(SCENCE5_IMX8)

include_directories(
    hdr/SIM/
    hdr/RS485/
    hdr/RS232/
    hdr/GPS/
    hdr/SIM/
    hdr/WATCHDOG/
    hdr/GPIO/
    hdr/BLUETOOTH/
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
    src/BLUETOOTH/bluetooth.c
   

	
    ) 
    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/SIM/sim.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS485/rs485.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS232/rs232.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPS/gps.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WATCHDOG/watchdog.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPIO/gpio.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)

    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/BLUETOOTH/bluetooth.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)
    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WIFI/network.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)
    file(COPY  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WIFI/common.h
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/header/)


    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        LIBGPIO
        USB
        SERIAL
    )
