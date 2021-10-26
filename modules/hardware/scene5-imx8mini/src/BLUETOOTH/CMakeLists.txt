PROJECT(BLUETOOTH)

find_package(PkgConfig REQUIRED)
pkg_check_modules (BLUEZ REQUIRED bluez)
link_directories(${BLUEZ_LIBRARY_DIRS})
include_directories(
    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/BLUETOOTH
    ${BLUEZ_INCLUDEDIR}
    )

add_library(${PROJECT_NAME} SHARED
	bluetooth.c
    )
    target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE
    ${BLUEZ_LIBRARY}
    )