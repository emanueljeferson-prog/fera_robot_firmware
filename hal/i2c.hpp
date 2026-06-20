#pragma once

#include <cstdint>
#include <vector>
#include "hardware/i2c.h"

namespace hal {

struct I2cConfig
{
    i2c_inst_t* i2c_port;
    uint8_t address;
    uint8_t sda;
    uint8_t scl;
};

class I2c {
public:
    I2c();
    void readRegister(const uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght);
    void readRegister(const uint8_t address, const uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght);
    void writeRegister(const uint8_t address, const uint8_t reg, const std::vector<uint8_t>& data);
    void writeRegister(const uint8_t address, const uint8_t reg, const uint8_t value);
    void scanBus(const char* label = nullptr);
    void reset();
private:
    I2cConfig config;
};

}