PROJECT(WIFI)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
link_directories(${GLIB_LIBRARY_DIRS})

PKG_SEARCH_MODULE(LIBNM REQUIRED libnm)
PKG_CHECK_MODULES(LIBNM libnm>=1.0)


PKG_SEARCH_MODULE(NM-CORE_INCLUDE_DIRS  libnm-core)
PKG_CHECK_MODULES(NM-CORE_INCLUDE_DIRS libnm-core)

include_directories(
       
    ${CMAKE_SOURCE_DIR}/modules/hardware/scene5-imx8m/hdr/WIFI/
    ${NM-UTIL_INCLUDE_DIRS}
    ${LIBNM_INCLUDE_DIRS}
    ${LIBNM_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)
LINK_DIRECTORIES(${LIBNM_LIBRARY_DIRS})
ADD_DEFINITIONS(${LIBNM_CFLAGS})

# configure libnm variable for config.h file
IF( LIBNM_FOUND )
    SET(USE_LIBNM 1)
ELSE()
    SET(USE_LIBNM 0)
ENDIF()

add_library(${PROJECT_NAME} SHARED
    common.c
    network.c
    main.c
    vector.c

)
    
target_link_libraries(${PROJECT_NAME} 
    ${GLIB_LIBRARIES}
    ${LIBNM_LIBRARIES}
    ${LIBNM_GLIB_LIBRARIES}
   
    )
