
PROJECT(SCENCE5_IMX8)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
link_directories(${GLIB_LIBRARY_DIRS})

PKG_SEARCH_MODULE(LIBNM REQUIRED libnm)
PKG_CHECK_MODULES(LIBNM libnm>=1.0)


#PKG_SEARCH_MODULE(NM-CORE_INCLUDE_DIRS  libnm-core)
#PKG_CHECK_MODULES(NM-CORE_INCLUDE_DIRS libnm-core)
#LINK_DIRECTORIES(${LIBNM_LIBRARY_DIRS})
#ADD_DEFINITIONS(${LIBNM_CFLAGS})

# configure libnm variable for config.h file
IF( LIBNM_FOUND )
    SET(USE_LIBNM 1)
ELSE()
    SET(USE_LIBNM 0)
ENDIF()



add_library( ${PROJECT_NAME} SHARED
    src/RS485/rs485.c
    src/RS232/rs232.c
    src/GPS/gps.c
    src/SIM/sim.c
    src/WATCHDOG/watchdog.c
    src/GPIO/gpio.c
    src/BLUETOOTH/bluetooth.c
    src/WIFI/network.c
    src/WIFI/common.c
    src/WIFI/main.c 
    ) 
target_include_directories(${PROJECT_NAME} PUBLIC
hdr/
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WIFI/ >
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/WIFI/>  # <prefix>/include/mylib
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WATCHDOG/ >
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/WATCHDOG/ >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/SIM/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/SIM/ >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS485/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/RS485/ >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/RS232/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/RS232/ >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPS/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/GPS/ >  # <prefix>/include/mylib
$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/GPIO/>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/GPIO >  # <prefix>/include/mylib
$<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/BLUETOOTH>
$<INSTALL_INTERFACE:/modules/hardware/scene5-imx8m/hdr/BLUETOOTH>  # <prefix>/include/mylib
)


    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        LIBGPIO
        USB
        SERIAL
        ${GLIB_LIBRARIES}
    ${LIBNM_LIBRARIES}
    ${LIBNM_GLIB_LIBRARIES}
    )
