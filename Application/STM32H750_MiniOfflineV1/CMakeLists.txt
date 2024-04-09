cmake_minimum_required(VERSION 3.6)
project(UB_STM32H750_MiniOfflineV1 C)
enable_language(ASM)

########################################################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
include(utils)

########################################################################################################################
option(CPU_32BIT "CPU is 32 BIT" ON)
option(CPU_ARM_CORTEX_M4 "Use ARM Cortex-M4 CPU" ON)
option(USE_TLSF "Use TLSF Memory Allocator" ON)

add_definitions(
        -DCPU_TICKS_PER_SECOND=1000
        -DTLSF_MEMORY_POOL_SIZE=10240
        -DCPU_32BIT
        -DUSE_TLSF
        -DSYSCLK_FREQ=100000000
        -DHSE_VALUE=25000000
)

########################################################################################################################
add_subdirectory(../../ThirdParty/STM32H750x ${CMAKE_CURRENT_BINARY_DIR}/STM32H750x)
add_subdirectory(../../Kernel ${CMAKE_CURRENT_BINARY_DIR}/Kernel)
add_subdirectory(../../CPU/ARM_CortexM4 ${CMAKE_CURRENT_BINARY_DIR}/CPU)
add_subdirectory(../../ThirdParty/TLSF-2.4.6 ${CMAKE_CURRENT_BINARY_DIR}/TLSF-2.4.6)
add_subdirectory(../../SDK ${CMAKE_CURRENT_BINARY_DIR}/SDK)

########################################################################################################################

file(GLOB SOURCES *.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB BSP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/BSP/*.c)
foreach (item IN LISTS BSP_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB HARDWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Hardware/*.c)
foreach (item IN LISTS HARDWARE_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

########################################################################################################################
file(GLOB DEVICE_A7670C_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/Common/*.c)
foreach (item IN LISTS DEVICE_A7670C_COMMON_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_HTTP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/HTTP/*.c)
foreach (item IN LISTS DEVICE_A7670C_HTTP_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_INTERNET_SERVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/InternetService/*.c)
foreach (item IN LISTS DEVICE_A7670C_INTERNET_SERVICE_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_MQTT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/MQTT/*.c)
foreach (item IN LISTS DEVICE_A7670C_MQTT_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_NETWORK_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/Network/*.c)
foreach (item IN LISTS DEVICE_A7670C_NETWORK_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_PACKET_DOMAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/PacketDomain/*.c)
foreach (item IN LISTS DEVICE_A7670C_PACKET_DOMAIN_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_SIM_CARD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/SIMCard/*.c)
foreach (item IN LISTS DEVICE_A7670C_SIM_CARD_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_STATUS_CONTROL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/StatusControl/*.c)
foreach (item IN LISTS DEVICE_A7670C_STATUS_CONTROL_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_V25TER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/V25TER/*.c)
foreach (item IN LISTS DEVICE_A7670C_STATUS_CONTROL_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_A7670C_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/*.c)
foreach (item IN LISTS DEVICE_A7670C_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()


########################################################################################################################


########################################################################################################################
add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME}
        PUBLIC ${SOURCES}
        PUBLIC ${BSP_SOURCES}
        PUBLIC ${HARDWARE_SOURCES}

        PUBLIC ${DEVICE_A7670C_SOURCES}
        PUBLIC ${DEVICE_A7670C_COMMON_SOURCES}
        PUBLIC ${DEVICE_A7670C_HTTP_SOURCES}
        PUBLIC ${DEVICE_A7670C_INTERNET_SERVICE_SOURCES}
        PUBLIC ${DEVICE_A7670C_MQTT_SOURCES}
        PUBLIC ${DEVICE_A7670C_NETWORK_SOURCES}
        PUBLIC ${DEVICE_A7670C_PACKET_DOMAIN_SOURCES}
        PUBLIC ${DEVICE_A7670C_SIM_CARD_SOURCES}
        PUBLIC ${DEVICE_A7670C_STATUS_CONTROL_SOURCES}
        PUBLIC ${DEVICE_A7670C_V25TER_SOURCES}
        PUBLIC ${DEVICE_IM1253E_SOURCES}
        PUBLIC ${DEVICE_KEY_SOURCES}
        PUBLIC ${DEVICE_CHCS_CAB_500A_SOURCES}


)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BSP)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Hardware)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/Common)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/HTTP)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/InternetService)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/MQTT)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/Network)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/PacketDomain)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/SIMCard)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/StatusControl)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/A7670C/V25TER)

target_link_libraries(${PROJECT_NAME}
        PUBLIC STM32H750x_HAL
        PUBLIC LightRTOS-Kernel
        PUBLIC LightRTOS-SDK
)


firmware_size(${PROJECT_NAME} sysv)
generate_object(${PROJECT_NAME} .bin binary)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    generate_object(${PROJECT_NAME} .hex ihex)
endif()

