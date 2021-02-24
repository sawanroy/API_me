 PROJECT(GPS)


#  include_directories(
#    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPS/
#    ${CMAKE_SOURCE_DIR}/modules/handler/serial/hdr/
 
#  )

 add_library(${PROJECT_NAME} SHARED
 	gps.c
     )
     target_include_directories(${PROJECT_NAME} PUBLIC
     $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPS/>
     $<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/GPS/ >  # <prefix>/include/mylib
     $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/handler/serial/hdr/>
     $<INSTALL_INTERFACE:/modules/handler/serial/hdr/ >  # <prefix>/include/mylib
     )
     TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        SERIAL

    )

