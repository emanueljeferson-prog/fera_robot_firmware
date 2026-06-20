add_library(hal
    gpio.cpp
    pwm.cpp
    uart.cpp
    spi.cpp
    i2c.cpp
)

target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(hal
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_gpio
)
