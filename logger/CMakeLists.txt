add_library(logger
    logger.cpp
)

target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(logger
    pico_stdlib
)
