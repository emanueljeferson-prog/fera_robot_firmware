#pragma once

#include "app_base.hpp"
#include <cstdint>

namespace app {

class Motor: public IApplication {
public:
    Motor(core::IMiddleware& middleware); 
    void init() override;
    void control();
    void readSpeed();
    static void controlWrapper(void* params);
    static void readSpeedWrapper(void* params);
private: 
    int16_t speed_01, speed_02; 
    core::IMiddleware& middleware;
    char commandTaskName[16];
    char speedTaskName[16];
};

}

