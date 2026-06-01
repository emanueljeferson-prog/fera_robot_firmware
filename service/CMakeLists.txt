set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rtos/FreeRTOS-Kernel)

file(GLOB FREERTOS_POSIX_UTILS
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix/utils/*.c
)

add_library(service
    rtos.cpp
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix/port.c
    ${FREERTOS_POSIX_UTILS}
)

target_link_libraries(service
    #pico_stdlib
    #hardware_spi
    #hardware_gpio
)

target_include_directories(service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rtos 
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

find_package(Threads REQUIRED)
target_link_libraries(service PUBLIC Threads::Threads)