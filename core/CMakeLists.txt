add_library(core
    middleware.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(core
    pico_stdlib
    logger
)
