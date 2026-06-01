add_library(hal
    gpio.cpp
)

target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(hal
    #pico_stdlib
    #hardware_spi
    #hardware_gpio
)
