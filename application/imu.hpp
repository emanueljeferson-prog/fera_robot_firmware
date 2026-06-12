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
    std::vector<uint16_t> accel; 
    std::vector<uint16_t> gyro; 
    std::vector<uint16_t> mag; 
    core::IMiddleware& middleware;
};

}

 