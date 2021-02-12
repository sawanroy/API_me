 PROJECT(LIBGPIO)

 include_directories(hdr/
 
 )

 add_library(${PROJECT_NAME} SHARED
 	src/libgpio.c
     )
