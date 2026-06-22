add_subdirectory(uroslib)

add_library(service
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
    hal
    logger
)

target_include_directories(service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/uroslib/
    ${CMAKE_CURRENT_SOURCE_DIR}/uroslib/libmicroros/include
)



