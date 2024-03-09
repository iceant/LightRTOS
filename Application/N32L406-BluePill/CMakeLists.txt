cmake_minimum_required(VERSION 3.6)
project(N32L40X-BluePill C)
enable_language(ASM)

########################################################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
include(utils)

########################################################################################################################
option(CPU_32BIT "CPU is 32 BIT" ON)
option(CPU_ARM_CORTEX_M4 "Use ARM Cortex-M4 CPU" ON)
option(USE_TLSF "Use TLSF Memory Allocator" ON)

add_definitions(
        -DCPU_TICKS_PER_SECOND=100
        -DTLSF_MEMORY_POOL_SIZE=10240
        -DCPU_32BIT
        -DUSE_TLSF
)

########################################################################################################################
add_subdirectory(../../ThirdParty/N32L40x ${CMAKE_CURRENT_BINARY_DIR}/N32L40x)
add_subdirectory(../../Kernel ${CMAKE_CURRENT_BINARY_DIR}/Kernel)
add_subdirectory(../../CPU/ARM_CortexM4 ${CMAKE_CURRENT_BINARY_DIR}/CPU)
add_subdirectory(../../SDK ${CMAKE_CURRENT_BINARY_DIR}/SDK)
add_subdirectory(../../ThirdParty/TLSF-2.4.6 ${CMAKE_CURRENT_BINARY_DIR}/TLSF-2.4.6)

########################################################################################################################
file(GLOB SOURCES *.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB HARDARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Hardware/*.c)
foreach (item IN LISTS HARDARE_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB BSP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/BSP/*.c)
foreach (item IN LISTS BSP_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DS1307_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Devices/DS1307/*.c)
foreach (item IN LISTS DS1307_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()


########################################################################################################################

add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME}
        PUBLIC ${SOURCES}
        PUBLIC ${HARDARE_SOURCES}
        PUBLIC ${BSP_SOURCES}
        PUBLIC ${DS1307_SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Hardware)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BSP)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Devices/DS1307)
target_link_libraries(${PROJECT_NAME}
        PUBLIC n32l40x
        PUBLIC LightRTOS-Kernel
        PUBLIC LightRTOS-SDK
)


firmware_size(${PROJECT_NAME} sysv)
generate_object(${PROJECT_NAME} .bin binary)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    generate_object(${PROJECT_NAME} .hex ihex)
endif()

