#pragma once

#include <iostream>

namespace logger {
    inline void info(const std::string message) {
        #ifdef DEBUG
            std::cout << "[INFO] " << message << std::endl;
        #endif
    }
    inline void warn(const std::string message) {
        #ifdef DEBUG
            std::cout << "[WARN] " << message << std::endl;
        #endif
    }
    inline void error(const std::string message) {
        #ifdef DEBUG
            std::cout << "[ERROR] " << message << std::endl;
        #endif
    }
}