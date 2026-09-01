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
    //void initMpu6500();
    //void initLsm303();
    void readImu(core::Vector3D& accel, core::Vector3D& gyro, core::Vector3D& mag, double& temp);
private:
    core::IMiddleware& middleware;
    hal::I2c acell_giro_device; // MPU9250/6500: acelerometro + giroscopio
    hal::I2c mag_device;        // GY-511 (LSM303DLHC): magnetometro externo, endereco proprio 0x1E
    bool mag_enabled;
};

}