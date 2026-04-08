cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in the Pico SDK (must be before project())
include(pico_sdk_import.cmake)

# Pull in FreeRTOS Kernel (must be before project() for SDK <= 1.3.1)
include(FreeRTOS_Kernel_import.cmake)

project(FreeRTOS-Pico C CXX ASM)

pico_sdk_init()

add_executable(FreeRTOS-Pico
    src/main.c
)

target_include_directories(FreeRTOS-Pico PRIVATE
    include/
) # for FreeRTOSConfig.h

target_link_libraries(FreeRTOS-Pico
    pico_stdlib
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# enable usb output, disable uart output
pico_enable_stdio_usb(FreeRTOS-Pico 1)
pico_enable_stdio_uart(FreeRTOS-Pico 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(FreeRTOS-Pico)
