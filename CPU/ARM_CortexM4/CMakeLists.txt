cmake_minimum_required(VERSION 3.6)
project(LightRTOS-CPU C)
enable_language(ASM)

set(CPU_FLAGS -mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16)

file(GLOB SOURCES *.c)
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpu_gcc.s)
endif ()

foreach (item IN LISTS SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS})
target_link_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS})

