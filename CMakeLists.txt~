
cmake_minimum_required(VERSION 3.5.1) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY})


#SELECT DEVICE
OPTION(SCENCE5-IMX8M				"Compile library for scene5-imx8m " OFF)
OPTION(SCENCE5-imx8mini				"Compile library for scene5-imx8mini " OFF)

#SELECT MODULES
OPTION(BLUETOOTH					"Compile library for bluetooth " ON)
OPTION(WIFI							"Compile library for wifi " OFF)
OPTION(RS232						"Compile library for rs232 " ON)
OPTION(RS485						"Compile library for rs485 " ON)
OPTION(GPIO							"Compile library for gpio " ON)
OPTION(SIM							"Compile library for sim " ON)
OPTION(GPS							"Compile library for gps " ON)
OPTION(WATCHDOG                     "Compile library for watchdog" ON)


IF(SCENCE5-IMX8M AND SCENCE5-IMX8MINI)
    MESSAGE(FATAL_ERROR "Can not select both SCENCE5-IMX8M and SCENCE5-IMX8MINI")
ENDIF()

PROJECT(TrunexaAPis)
IF(BLUETOOTH)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/BLUETOOTH)
ENDIF()
IF(WIFI)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/WIFI)
ENDIF()
IF(RS232)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/RS232)
ENDIF()
IF(RS485)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/RS485)
ENDIF()
IF(GPIO)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/GPIO)
ENDIF()
IF(SIM)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/SIM)
ENDIF()
IF(GPS)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/GPS)
ENDIF()
IF(WATCHDOG)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m/src/WATCHDOG)
ENDIF()

ADD_SUBDIRECTORY(modules/handler/usb)
ADD_SUBDIRECTORY(modules/handler/libgpio)
ADD_SUBDIRECTORY(modules/handler/serial)




IF(SCENCE5-IMX8M)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8m)
ENDIF()

IF(SCENCE5-IMX8MINI)
	ADD_SUBDIRECTORY(modules/hardware/scene5-imx8mini)
ENDIF()


INCLUDE_DIRECTORIES(
	modules/handler/usb/hdr/SIM/
)

