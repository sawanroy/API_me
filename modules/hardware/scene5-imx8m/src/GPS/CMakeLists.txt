 PROJECT(GPS)


 include_directories(
   ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPS/
   ${CMAKE_SOURCE_DIR}/modules/handler/serial/hdr/
 
 )

 add_library(${PROJECT_NAME} SHARED
 	gps.c
     )

     TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        SERIAL

    )

