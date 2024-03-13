cmake_minimum_required(VERSION 3.6)
project(UB_N32G457_MiniOfflineV1 C)
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
        -DTLSF_MEMORY_POOL_SIZE=20480
        -DCPU_32BIT
        -DUSE_TLSF
        -DSYSCLK_FREQ=100000000
        -DHSE_VALUE=25000000
)

########################################################################################################################
add_subdirectory(../../ThirdParty/N32G45x ${CMAKE_CURRENT_BINARY_DIR}/N32G45x)
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

file(GLOB CONSOLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/Console/*.c)
foreach (item IN LISTS CONSOLE_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_sFLASH_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/sFLASH/*.c)
foreach (item IN LISTS DEVICE_sFLASH_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_DS1307_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/DS1307/*.c)
foreach (item IN LISTS DEVICE_DS1307_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB DEVICE_IM1253E_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device/IM1253E/*.c)
foreach (item IN LISTS DEVICE_IM1253E_SOURCES)
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
file(GLOB MODULE_CONSOLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Module/Console/*.c)
foreach (item IN LISTS CONSOLE_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB MODULE_NTP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Module/NTP/*.c)
foreach (item IN LISTS MODULE_NTP_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB MODULE_SYSTEM_DATETIME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Module/SystemDateTime/*.c)
foreach (item IN LISTS MODULE_SYSTEM_DATETIME_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

file(GLOB MODULE_ENERGY_METER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Module/EnergyMeter/*.c)
foreach (item IN LISTS MODULE_ENERGY_METER_SOURCES)
    message(STATUS "[${PROJECT_NAME}] SOURCE: ${item}")
endforeach ()

########################################################################################################################
add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME}
        PUBLIC ${SOURCES}
        PUBLIC ${BSP_SOURCES}
        PUBLIC ${HARDWARE_SOURCES}
        PUBLIC ${CONSOLE_SOURCES}
        PUBLIC ${DEVICE_sFLASH_SOURCES}
        PUBLIC ${DEVICE_DS1307_SOURCES}
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

        PUBLIC ${MODULE_CONSOLE_SOURCES}
        PUBLIC ${MODULE_NTP_SOURCES}
        PUBLIC ${MODULE_SYSTEM_DATETIME_SOURCES}
        PUBLIC ${MODULE_ENERGY_METER_SOURCES}

)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/BSP)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Hardware)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/Console)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/sFLASH)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/DS1307)
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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Device/IM1253E)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Module/Console)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Module/NTP)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Module/SystemDateTime)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Module/EnergyMeter)

target_link_libraries(${PROJECT_NAME}
        PUBLIC N32G45x
        PUBLIC LightRTOS-Kernel
        PUBLIC LightRTOS-SDK
)


firmware_size(${PROJECT_NAME} sysv)
generate_object(${PROJECT_NAME} .bin binary)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    generate_object(${PROJECT_NAME} .hex ihex)
endif()

