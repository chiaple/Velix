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
    USER/BSP/Display/ST7789/st7789.c
    USER/BSP/ENCODER/MT6701/MT6701.c
    USER/BSP/input/key.c
    USER/control/algorithm/FOC.c
    USER/control/algorithm/PID.c
    USER/control/command/command.c
    USER/control/estimation/encoder.c
    USER/control/estimation/observer.c
    USER/control/estimation/velocity.c
    USER/control/mode/identify.c
    USER/control/mode/motor_stop.c
    USER/control/mode/sensorless.c
    USER/control/mode/sensoruse.c
    USER/control/sampling/sample.c
    USER/model/motor.c
    USER/service/audio/motor_tone.c
    USER/service/audio/tone_presets.c
    USER/service/comm/serial.c
    USER/service/comm/serial_commands.c
    USER/service/comm/vofa_profile.c
    USER/service/indicator/key_actions.c
    USER/service/indicator/led_task.c
    USER/system/global.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    USER
    USER/BSP/Display/ST7789
    USER/BSP/ENCODER/MT6701
    USER/BSP/input
    USER/control/algorithm
    USER/control/command
    USER/control/estimation
    USER/control/mode
    USER/control/sampling
    USER/model
    USER/service/audio
    USER/service/comm
    USER/service/indicator
    USER/system
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
