PROJECT(RS485)

add_library(
    ${PROJECT_NAME}
    SHARED
	src/rs485.c
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    hdr
    private
    USB
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    USB
)
