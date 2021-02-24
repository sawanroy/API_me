 PROJECT(GPIO)

#  include_directories(
#   ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPIO/
#   ${CMAKE_SOURCE_DIR}/modules/handler/libgpio/hdr
#     )

 add_library(${PROJECT_NAME} SHARED
 	gpio.c
     )

target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPIO/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/GPIO >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/handler/libgpio/hdr>
$<INSTALL_INTERFACE:/modules/handler/libgpio/hdr >  # <prefix>/include/mylib
)
     TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
        LIBGPIO

    )