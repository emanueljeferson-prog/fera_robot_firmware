#pragma once

#include <cstdint>
#include <vector>
#include "service_base.hpp"
#include "hal/i2c.hpp"

namespace service {  

class ImuDrive : public IService {
public:
    ImuDrive(core::IMiddleware& middleware);
    void init() override;
    //void initMpu6050();
    //void initLsm303();
    void readImu(core::Vector3D& accel, core::Vector3D& gyro, core::Vector3D& mag, double& temp);
private:
    core::IMiddleware& middleware;
    hal::I2c acell_giro_device;
};

}
