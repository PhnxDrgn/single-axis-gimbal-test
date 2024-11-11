# referenced https://github.com/prtzl/stm32-cmake

cmake_minimum_required(VERSION 3.12)
###############################################################################
# set toolchin file
set(CMAKE_TOOLCHAIN_FILE "../arm-none-eabi-gcc.cmake")

# set project name
project(blink)

# Set microcontroller information
set(MCU_FAMILY STM32F3xx)
set(MCU_MODEL STM32F303x8)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bsp/nucleo_f3/startup_stm32f303x8.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bsp/nucleo_f3/STM32F303K8Tx_FLASH.ld)

###############################################################################
set(EXECUTABLE ${CMAKE_PROJECT_NAME}.elf)

enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

###############################################################################
set(BSP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bsp/nucleo_f3)
set(BSP_INC_DIR
    ${BSP_DIR}/Core/Inc
    ${BSP_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${BSP_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${BSP_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${BSP_DIR}/Drivers/CMSIS/Include)

set(PROJECT_INCLUDE_DIRECTORIES ${BSP_DIR})

file(GLOB_RECURSE BSP_SOURCES CONFIGURE_DEPENDS
    ${BSP_DIR}/Core/*.c
    ${BSP_DIR}/Drivers/*.c)

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app)
set(APP_INC_DIR
    ${APP_DIR}/inc)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${APP_DIR}/src/*.c)

add_executable(${EXECUTABLE}
    ${BSP_SOURCES}
    ${APP_SOURCES}
    ${STARTUP_SCRIPT})

target_compile_definitions(${EXECUTABLE} PRIVATE
    #$<$<CONFIG:Debug>:DEBUG>
    ${MCU_MODEL}
    USE_HAL_DRIVER)

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
    ${BSP_INC_DIR}
    ${APP_INC_DIR})

target_include_directories(${EXECUTABLE} PRIVATE
    ${PROJECT_INCLUDE_DIRECTORIES})

###############################################################################
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -fdata-sections
    -ffunction-sections

    -Wall

    $<$<CONFIG:Debug>:-Og>)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -specs=nano.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE}
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex # generate .hex file of executable
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin # generate .bin file. Depending on setup, the start address can change. Default is 0x08000000
        COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE} debug.elf) # make a copy named debug so debug setup can be consistant 