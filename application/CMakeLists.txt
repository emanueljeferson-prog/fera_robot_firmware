add_library(app
    motor.cpp
)

target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(app
    #pico_stdlib
    #hardware_spi
    #hardware_gpio
)
