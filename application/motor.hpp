#pragma once

#include "core/middleware.hpp"
#include <cstdint>

namespace app {

class Motor {
public:
    Motor(uint8_t id, core::IMiddleware& middleware); 
    void init();
    void control();
    void readSpeed();
    static void controlWrapper(void* params);
    static void readSpeedWrapper(void* params);
private:
    const uint8_t id; 
    double speed; 
    core::IMiddleware& middleware;
};

}

