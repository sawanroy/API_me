PROJECT(WATCHDOG)


include_directories(../../hdr/WATCHDOG/)

add_library(${PROJECT_NAME} SHARED
	watchdog.c
    )

    
