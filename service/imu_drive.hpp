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
    void readImu(std::vector<double>& accel, std::vector<double>& gyro, std::vector<double>& mag, std::vector<double>& temp);
private:
    core::IMiddleware& middleware;
    hal::I2c i2c;
};

}
