PROJECT(ETH_SWITCH)
find_package(PkgConfig REQUIRED)
include(CheckIncludeFiles)
pkg_search_module(GLIB REQUIRED glib-2.0)

PKG_SEARCH_MODULE(LIBNM REQUIRED libnm)
PKG_CHECK_MODULES(LIBNM libnm>=1.0)

#PKG_SEARCH_MODULE(NM-CORE_INCLUDE_DIRS libnm-core)
#PKG_CHECK_MODULES(NM-CORE_INCLUDE_DIRS libnm-core)

add_definitions(${LIBNM_CFLAGS})


    # set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
    # check_include_files(iwlib.h IWLIB_H)
    # if(NOT IWLIB_H)
    #     message(FATAL_ERROR "Unable to find iwlib.h")
    # endif(NOT IWLIB_H)
    # find_library(IWLIB_LIB NAMES iw)
    # if(NOT IWLIB_LIB)
    #     message(FATAL_ERROR "Unable to find libiw.so")
    # endif(NOT IWLIB_LIB)
    # set(conky_libs ${conky_libs} ${IWLIB_LIB})
    # check_function_exists(iw_sockets_open IWLIB_SOCKETS_OPEN_FUNC)


# configure libnm variable for config.h file
IF( LIBNM_FOUND )
    SET(USE_LIBNM 1)
ELSE()
    SET(USE_LIBNM 0)
ENDIF()

add_library(
    ${PROJECT_NAME}
    SHARED
    src/switch.c
    src/nmcommon.c
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
    hdr/
    ${GLIB_INCLUDE_DIRS}
    ${NM-UTIL_INCLUDE_DIRS}
    ${LIBNM_INCLUDE_DIRS}
    PRIVATE
    USB
)
    
target_link_libraries(
    ${PROJECT_NAME} 
    ${GLIB_LIBRARIES}
    ${LIBNM_LIBRARIES}
    ${LIBNM_GLIB_LIBRARIES}
)
