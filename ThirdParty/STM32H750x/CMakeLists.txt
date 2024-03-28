cmake_minimum_required(VERSION 3.6)
project(STM32H750x_HAL C)
enable_language(ASM)

execute_process(COMMAND git init WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git remote add origin git@github.com:STMicroelectronics/STM32CubeH7  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git config core.sparsecheckout true  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
FILE(WRITE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/.git/info/sparse-checkout
"Drivers/CMSIS/Core
Drivers/CMSIS/Device/*
Drivers/STM32H7xx_HAL_Driver\r\n")
execute_process(COMMAND git pull --depth 1 origin master  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
execute_process(COMMAND git submodule update --init --recursive  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set(SOURCE_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/STM32H7xx_HAL_Driver/Src)
set(INCLUDE_DIR
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Core/Include
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/STM32H7xx_HAL_Driver/Inc
)

file(COPY_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_conf_template.h
       ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_conf.h)

file(GLOB STM32H750_HAL_DRIVER_SOURCES  
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/STM32H7xx_HAL_Driver/Src/*.c)
file(GLOB STM32H750_HAL_CMSIS_SOURCES  
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c)

add_library(${PROJECT_NAME} "")

if(CMAKE_C_COMPILER_ID MATCHES GNU)
  list(APPEND STM32H750_HAL_CMSIS_SOURCES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h750xx.s)
  set(LINKER_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/linker/stm32h755xx_flash_CM7.ld")

  set(CPU_FLAGS  -mcpu=cortex-m7 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16)
    set(COMPILER_FLAGS
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


set(STM32H750_HAL_SOURCES ${STM32H750_HAL_DRIVER_SOURCES} ${STM32H750_HAL_CMSIS_SOURCES})
list(FILTER STM32H750_HAL_SOURCES EXCLUDE REGEX template.c)

foreach (item IN LISTS STM32H750_HAL_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()


target_sources(${PROJECT_NAME} PUBLIC ${STM32H750_HAL_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_compile_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS} ${COMPILER_FLAGS})
target_compile_definitions(${PROJECT_NAME} PUBLIC USE_HAL_DRIVER STM32H750xx STM32H7)
