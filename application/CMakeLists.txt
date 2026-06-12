add_library(app
    motor.cpp
    imu.cpp
)

target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(app
    pico_stdlib
)
