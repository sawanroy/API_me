PROJECT(WATCHDOG)


add_library(${PROJECT_NAME} SHARED
	watchdog.c
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WATCHDOG/ >
    $<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/WATCHDOG/ >  # <prefix>/include/mylib
    )  
