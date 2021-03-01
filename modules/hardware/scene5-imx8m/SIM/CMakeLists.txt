PROJECT(SIM)

add_library(
    ${PROJECT_NAME}
    SHARED
    src/sim.c    
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    hdr
    PRIVATE
    USB
)

target_link_libraries(
    ${PROJECT_NAME}
    USB
)
