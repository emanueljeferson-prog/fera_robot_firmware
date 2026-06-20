#pragma once

#include "app_base.hpp"
#include <cstdint>
#include <vector>

namespace app {

struct Vector3D {
    double x;
    double y;
    double z;
};    

class Imu: public IApplication {
public:
    Imu(core::IMiddleware& middleware); 
    void init() override;
    void readImu();
    static void readImuWrapper(void* params);
private:    
    Vector3D accel; 
    Vector3D gyro; 
    Vector3D mag; 
    double temp;
    core::IMiddleware& middleware;
};

}

 