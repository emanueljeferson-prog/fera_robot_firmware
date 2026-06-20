add_subdirectory(uroslib)

add_library(service
    rtos.cpp
    encoder_drive.cpp
    motor_drive.cpp
    imu_drive.cpp
    uros.cpp
    comm.cpp
)

target_link_libraries(service PUBLIC
    pico_stdlib
    hardware_spi
    hardware_gpio
    micro_ros
    FreeRTOS-Kernel-Heap4
    hal
    logger
)

target_include_directories(service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/rtos 
    ${CMAKE_CURRENT_SOURCE_DIR}/uroslib/
    ${CMAKE_CURRENT_SOURCE_DIR}/uroslib/libmicroros/include
)

SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rtos/FreeRTOS-Kernel)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)


