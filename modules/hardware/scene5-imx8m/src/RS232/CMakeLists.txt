 PROJECT(RS232)

 include_directories( 
   ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS232/
   ${CMAKE_SOURCE_DIR}/modules/handler/usb/hdr/
    )

 add_library(${PROJECT_NAME} SHARED
 	rs232.c
     )

    
     TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        USB

    )

