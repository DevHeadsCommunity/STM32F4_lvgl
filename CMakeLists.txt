cmake_minimum_required(VERSION 3.28)

project(stm32f4_lvgl LANGUAGES ASM C CXX VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Include the ARM toolchan
include(gcc-arm-none-eabi.cmake)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446RETX_FLASH.ld)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/core/Startup/*.c
    ${CMAKE_SOURCE_DIR}/lvgl/src/*.c
)

# message(STATUS ${FREERTOS_SOURCES})


# Create the main executable target
add_executable(${PROJECT_NAME} ${SOURCES})


# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/core/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/core/CMSIS/Device
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/lvgl
    ${CMAKE_SOURCE_DIR}/lvgl/src/font
)

target_compile_definitions(${PROJECT_NAME} PUBLIC STM32F446xx)

# Set linker options
target_link_options(${PROJECT_NAME} PUBLIC 
    -T${LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--print-memory-usage
    -Wl,--gc-sections
    -lc 
    -lstdc++
    -lm
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
)

add_custom_target(flash 
    COMMAND st-flash write ${PROJECT_NAME}.bin 0x08000000
    COMMAND st-flash reset
)