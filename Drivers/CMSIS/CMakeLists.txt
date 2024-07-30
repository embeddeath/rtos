cmake_minimum_required(VERSION 3.22)

project(Drivers)
add_library(Drivers INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Not used (Hal not implemented yet)
target_compile_definitions(Drivers INTERFACE 
	USE_HAL_DRIVER 
	STM32G0B1xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(Drivers INTERFACE
    ./Device
    ./Include
    ../../Inc
)

target_sources(Drivers INTERFACE
    #Source Files Go here
    ../../Src/main.c
    ../../Src/led.c
    ../../Src/osKernel.c
    ../../../startup_stm32g0b1retx.s

)

target_link_directories(Drivers INTERFACE
)

target_link_libraries(Drivers INTERFACE
)

# Validate that Drivers code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


