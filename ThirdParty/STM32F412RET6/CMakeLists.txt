cmake_minimum_required(VERSION 3.6)
project(STM32F412RET6_HAL C)
enable_language(ASM)

set(STM32_CUBE_MODULE "STM32CubeF4")
set(STM32_MCU_TYPE "STM32F4xx")
set(STM32_HAL_DRIVER "STM32F4xx_HAL_Driver")
set(STM32_MCU_SRC_PREFIX "stm32f4xx")

execute_process(COMMAND git init WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git remote add origin git@github.com:STMicroelectronics/${STM32_CUBE_MODULE}  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git config core.sparsecheckout true  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
FILE(WRITE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/.git/info/sparse-checkout
"Drivers/CMSIS/Core
Drivers/CMSIS/Device/*
Drivers/${STM32_HAL_DRIVER}\r\n")
execute_process(COMMAND git pull --depth 1 origin master  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git submodule update --init --recursive  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set(SOURCE_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/${STM32_HAL_DRIVER}/Src)
set(INCLUDE_DIR
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Core/Include
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/${STM32_MCU_TYPE}/Include
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/${STM32_HAL_DRIVER}/Inc
)

file(COPY_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/${STM32_HAL_DRIVER}/Inc/${STM32_MCU_SRC_PREFIX}_hal_conf_template.h
       ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/${STM32_HAL_DRIVER}/Inc/${STM32_MCU_SRC_PREFIX}_hal_conf.h)

file(GLOB ${STM32_MCU_TYPE}_HAL_DRIVER_SOURCES  
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/${STM32_HAL_DRIVER}/Src/*.c)
file(GLOB ${STM32_MCU_TYPE}_HAL_CMSIS_SOURCES  
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/${STM32_MCU_TYPE}/Source/Templates/system_${STM32_MCU_SRC_PREFIX}.c)

add_library(${PROJECT_NAME} "")

if(CMAKE_C_COMPILER_ID MATCHES GNU)
  list(APPEND ${STM32_MCU_TYPE}_HAL_CMSIS_SOURCES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/${STM32_MCU_TYPE}/Source/Templates/gcc/startup_stm32f412rx.s)
  set(LINKER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/gcc/stm32f4xx_flash_CM4.ld")

  set(CPU_FLAGS  -mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16)
  set(COMPILER_FLAGS
          -Wall -Wextra -Wpedantic
          -fmessage-length=0
          -ffunction-sections
          -fdata-sections
          -fno-common
          #            -fshort-enums
          #            -ffreestanding
          #            -ffunction-sections
          #            -fdata-sections
          #            -fsigned-char
          #            -fmessage-length=0
          #            -fshort-enums
  )
  set(LINK_FLAGS -lc -lm -Wl,--gc-sections
          -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--cref -Wl,--print-memory-usage
          -specs=nano.specs
          #            -specs=nosys.specs -lnosys
  )

  #    add_compile_options(${CPU_FLAGS} ${COMPILER_FLAGS})
  #    add_link_options(${CPU_FLAGS} -T${LINKER_FILE})
  target_link_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS} -T${LINKER_FILE} ${LINK_FLAGS})
endif()   


set(HAL_SOURCES ${${STM32_MCU_TYPE}_HAL_DRIVER_SOURCES} ${${STM32_MCU_TYPE}_HAL_CMSIS_SOURCES})
list(FILTER HAL_SOURCES EXCLUDE REGEX template.c)

foreach (item IN LISTS HAL_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()


target_sources(${PROJECT_NAME} PUBLIC ${HAL_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_compile_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS} ${COMPILER_FLAGS})
target_compile_definitions(${PROJECT_NAME} PUBLIC USE_HAL_DRIVER STM32F412Rx)
