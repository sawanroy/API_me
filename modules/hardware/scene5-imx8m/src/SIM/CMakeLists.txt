PROJECT(SIM)


# include_directories(
#     ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/SIM/
#     ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/
#     )

add_library(${PROJECT_NAME} SHARED
    sim.c
    
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/SIM/>
    $<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/SIM/ >  # <prefix>/include/mylib
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/>
    $<INSTALL_INTERFACE:/modules/handler/usb/hdr/ >  # <prefix>/include/mylib
    )
   
    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        USB

    )
