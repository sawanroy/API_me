 PROJECT(GPIO)

 include_directories(
  ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPIO/
  ${CMAKE_SOURCE_DIR}/modules/handler/libgpio/hdr
    )

 add_library(${PROJECT_NAME} SHARED
 	gpio.c
     )
     TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        LIBGPIO

    )