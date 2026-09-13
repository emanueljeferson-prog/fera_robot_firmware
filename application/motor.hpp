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
    int16_t speed_right, speed_left;
    double integral_right;
    double integral_left;
    char commandTaskName[16];
    char speedTaskName[16];
    core::IMiddleware& middleware;
};

}

