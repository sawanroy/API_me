 PROJECT(RS232)

#  include_directories( 
#    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS232/
#    ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/
#     )

 add_library(${PROJECT_NAME} SHARED
 	rs232.c
     )
     target_include_directories(${PROJECT_NAME} PUBLIC
     $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS232/>
     $<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/RS232/ >  # <prefix>/include/mylib
     $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/>
     $<INSTALL_INTERFACE:/modules/handler/usb/hdr/ >  # <prefix>/include/mylib
     )
    
     TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        USB

    )

