#pragma once

#include "app_base.hpp"
#include <cstdint>
#include <vector>

namespace app { 

class Imu: public IApplication {
public:
    Imu(core::IMiddleware& middleware); 
    void init() override;
    void readImu();
    static void readImuWrapper(void* params);
private:    
    core::Vector3D accel; 
    core::Vector3D gyro; 
    core::Vector3D mag; 
    double temp;
    core::IMiddleware& middleware;
};

}

 