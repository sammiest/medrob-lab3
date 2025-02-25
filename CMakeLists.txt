cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(CMAKE_PROJECT_VERSION 1.2.0 CACHE STRING "Project version")

# Expose the version to the source code.
add_definitions(-DVERSION="${CMAKE_PROJECT_VERSION}")

message("Building code for MBot type ${MBOT_TYPE}")

set(NAME_SUFFIX "_v${CMAKE_PROJECT_VERSION}")

set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/lib/pico-sdk)
include(${CMAKE_SOURCE_DIR}/lib/pico-sdk/external/pico_sdk_import.cmake)

project(mbot_pico C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
  -Wno-format           # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-unused-function  # we have some for the docs that aren't called
  -Wno-maybe-uninitialized
)

add_subdirectory(mbot)
add_subdirectory(rc)
add_subdirectory(comms)

include_directories(
  include
  /usr/local/include
)

# Encoder Test - Both types.
add_executable(mbot_encoder_test
  tests/mbot_encoder_test.c
)

target_link_libraries(mbot_encoder_test
  pico_stdlib
  hardware_i2c
  hardware_pio
  hardware_pwm
  rclib
)

pico_enable_stdio_usb(mbot_encoder_test 1)
pico_enable_stdio_uart(mbot_encoder_test 0)
pico_add_extra_outputs(mbot_encoder_test)

 # Motor Test
add_executable(mbot_classic_motor_test
  tests/mbot_classic_motor_test.c
)
target_link_libraries(mbot_classic_motor_test
  pico_stdlib
  hardware_i2c
  hardware_pio
  hardware_pwm
  rclib
)
pico_enable_stdio_usb(mbot_classic_motor_test 1)
pico_enable_stdio_uart(mbot_classic_motor_test 0)
pico_add_extra_outputs(mbot_classic_motor_test)

# Teleoperation
add_executable(mbot_teleoperation
  tests/mbot_teleoperation.c
)
target_link_libraries(mbot_teleoperation
  pico_stdlib
  hardware_i2c
  hardware_pio
  hardware_pwm
  rclib
)
pico_enable_stdio_usb(mbot_teleoperation 1)
pico_enable_stdio_uart(mbot_teleoperation 0)
pico_add_extra_outputs(mbot_teleoperation)
