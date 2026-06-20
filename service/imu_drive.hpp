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
    constexpr static double GRAVITY = 9.80665; // m/s²
    constexpr static double ACCEL_SENSITIVITY = 16384.0f; // LSB/g for ±2g
    constexpr static double GYRO_SENSITIVITY = 131.0f; // LSB/(°/s) for ±250°/s
    constexpr static double MAG_SENSITIVITY = 0.15f; // μT/LSB for 16-bit output
    constexpr static double TEMP_SENSITIVITY = 333.87f; // LSB/°C
    constexpr static double DEG2RAD = 3.14159265358979323846f / 180.0f; // Degrees to radians conversion factor
};

}
