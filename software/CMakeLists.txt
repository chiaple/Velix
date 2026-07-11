cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Velix)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Velix/BSP/Display/ST7789/fonts.c
    Velix/BSP/Display/ST7789/st7789.c
    Velix/BSP/ENCODER/MT6701/MT6701.c
    Velix/BSP/input/key.c
    Velix/BSP/input/rotary_encoder.c
    Velix/control/algorithm/FOC.c
    Velix/control/algorithm/PID.c
    Velix/control/command/command.c
    Velix/control/estimation/encoder.c
    Velix/control/estimation/observer.c
    Velix/control/estimation/velocity.c
    Velix/control/mode/identify.c
    Velix/control/mode/motor_stop.c
    Velix/control/mode/sensorless.c
    Velix/control/mode/sensoruse.c
    Velix/control/sampling/sample.c
    Velix/model/motor.c
    Velix/service/audio/motor_tone.c
    Velix/service/audio/tone_presets.c
    Velix/service/comm/serial.c
    Velix/service/comm/serial_commands.c
    Velix/service/comm/vofa_profile.c
    Velix/service/display/config_display.c
    Velix/service/indicator/key_actions.c
    Velix/service/indicator/led_task.c
    Velix/system/global.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Velix
    Velix/BSP/Display/ST7789
    Velix/BSP/ENCODER/MT6701
    Velix/BSP/input
    Velix/control/algorithm
    Velix/control/command
    Velix/control/estimation
    Velix/control/mode
    Velix/control/sampling
    Velix/model
    Velix/service/audio
    Velix/service/comm
    Velix/service/display
    Velix/service/indicator
    Velix/system
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
