PROJECT(BLUETOOTH)

find_package(PkgConfig REQUIRED)
pkg_check_modules (BLUEZ REQUIRED bluez)
include_directories(${BLUEZ_INCLUDE_DIRS})
link_directories(${BLUEZ_LIBRARY_DIRS})
ADD_DEFINITIONS(${BLUEZ_CFLAGS})



if(PKG_CONFIG_FOUND)
      pkg_check_modules(PC_BLUETOOTH bluez bluetooth QUIET)
    endif()
    
    find_path(BLUETOOTH_INCLUDE_DIR NAMES bluetooth/bluetooth.h
                                    PATHS ${PC_BLUETOOTH_INCLUDEDIR})
    find_library(BLUETOOTH_LIBRARY NAMES bluetooth libbluetooth
                                   PATHS ${PC_BLUETOOTH_LIBDIR})
    
    set(BLUETOOTH_VERSION ${PC_BLUETOOTH_VERSION})
    
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Bluetooth
                                      REQUIRED_VARS BLUETOOTH_LIBRARY BLUETOOTH_INCLUDE_DIR
                                      VERSION_VAR BLUETOOTH_VERSION)
    
    if(BLUETOOTH_FOUND)
      set(BLUETOOTH_INCLUDE_DIRS ${BLUETOOTH_INCLUDE_DIR})
      set(BLUETOOTH_LIBRARIES ${BLUETOOTH_LIBRARY})
    
      if(NOT TARGET Bluetooth::Bluetooth)
        add_library(Bluetooth::Bluetooth UNKNOWN IMPORTED)
        set_target_properties(Bluetooth::Bluetooth PROPERTIES
                                                   IMPORTED_LOCATION "${BLUETOOTH_LIBRARY}"
                                                   INTERFACE_INCLUDE_DIRECTORIES "${BLUETOOTH_INCLUDE_DIR}")
      endif()
    endif()
    
    mark_as_advanced(BLUETOOTH_INCLUDE_DIR BLUETOOTH_LIBRARY)



add_library(${PROJECT_NAME} SHARED src/bluetooth.c)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    hdr/
    PRIVATE
    ${BLUEZ_INCLUDEDIR}
)
    
    target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    ${BLUEZ_LIBRARY}
    ${BLUETOOTH_LIBRARY}
   
    )
