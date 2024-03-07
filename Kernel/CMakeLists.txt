cmake_minimum_required(VERSION 3.6)
project(LightRTOS-Kernel C)

########################################################################################################################
## OPTIONS
option(CPU_8BIT "CPU Is 8 BIT" OFF)
option(CPU_16BIT "CPU Is 16 BIT" OFF)
option(CPU_32BIT "CPU Is 32 BIT" ON)
option(CPU_64BIT "CPU Is 64 BIT" OFF)

########################################################################################################################
## DEFINITIONS
if (CPU_8BIT)
    add_definitions(-DCPU_8BIT)
endif ()

if (CPU_16BIT)
    add_definitions(-DCPU_16BIT)
endif ()

if (CPU_32BIT)
    add_definitions(-DCPU_32BIT)
endif ()

if (CPU_64BIT)
    add_definitions(-DCPU_64BIT)
endif ()

########################################################################################################################
## CONFIG FILE
configure_file(os_config.h.in os_config.h)

########################################################################################################################
## SOURCES
file(GLOB SOURCES *.c)

add_library(${PROJECT_NAME} "")

if (USE_TLSF)
    set(TLSF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/TLSF-2.4.6/src)
    message(STATUS "TLSF_INCLUDE_DIR:${TLSF_INCLUDE_DIR}")
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_TLSF)
    target_include_directories(${PROJECT_NAME} PUBLIC ${TLSF_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC TLSF)

    list(APPEND RTOS_KERNEL_INCLUDE_DIR ${TLSF_INCLUDE_DIR})
    list(APPEND RTOS_KERNEL_SOURCES ${TLSF_INCLUDE_DIR}/tlsf.c)

endif ()


target_sources(${PROJECT_NAME} PUBLIC ${SOURCES})
target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${PROJECT_NAME} PUBLIC LightRTOS-CPU)

foreach (item IN LISTS SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()


