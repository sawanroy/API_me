PROJECT(SIM)


include_directories(
    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/SIM/
    ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/
    )

add_library(${PROJECT_NAME} SHARED
    sim.c
    
    )

    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        USB

    )
