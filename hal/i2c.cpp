#include "i2c.hpp"
#include "gpio.hpp"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "logger/logger.hpp"
#include <iomanip>
#include <vector>

namespace hal {

I2c::I2c() {
    logger::info("[HAL] [i2c] [START]");
    config.i2c_port = i2c_default;
    config.address = 0x68;
    config.sda = PICO_DEFAULT_I2C_SDA_PIN;
    config.scl = PICO_DEFAULT_I2C_SCL_PIN;

    i2c_init(config.i2c_port, 400 * 1000);

    Gpio::gpioSetFunction(config.sda, GPIO_FUNC_I2C);
    Gpio::gpioSetFunction(config.scl, GPIO_FUNC_I2C);
    Gpio::pullUp(config.sda);
    Gpio::pullUp(config.scl);
    bi_decl(bi_2pins_with_func(config.sda, config.scl, GPIO_FUNC_I2C));
    reset();
}

void I2c::readRegister(uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght) {
    logger::info("[HAL] [i2c] [READ REGISTER: " + std::to_string((int)reg) + "]");
    buffer.assign(lenght, 0);
    int w = i2c_write_blocking(config.i2c_port, config.address, &reg, 1, true);
    int r = i2c_read_blocking(config.i2c_port, config.address, buffer.data(), lenght, false);
}

void I2c::readRegister(const uint8_t address, const uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght) {
    logger::info("[HAL] [i2c] [READ REGISTER: " + std::to_string((int)reg) + " @ 0x" + std::to_string((int)address) + "]");
    buffer.assign(lenght, 0);
    int w = i2c_write_blocking(config.i2c_port, address, &reg, 1, true);
    int r = -1;
    if (w >= 0) {
        r = i2c_read_blocking(config.i2c_port, address, buffer.data(), lenght, false);
    }
}

void I2c::writeRegister(const uint8_t address, const uint8_t reg, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> write_buffer;
    write_buffer.reserve(1 + data.size());
    write_buffer.push_back(reg);
    write_buffer.insert(write_buffer.end(), data.begin(), data.end());
    int w = i2c_write_blocking(config.i2c_port, address, write_buffer.data(), write_buffer.size(), false);
    logger::info("[HAL] [i2c] [WRITE REGISTER: " + std::to_string((int)reg) + " @ 0x" + std::to_string((int)address) + " bytes=" + std::to_string(w) + "]");
}

void I2c::writeRegister(const uint8_t address, const uint8_t reg, const uint8_t value) {
    uint8_t write_buffer[2] = { reg, value };
    int w = i2c_write_blocking(config.i2c_port, address, write_buffer, 2, false);
}

void I2c::scanBus(const char* label) {
    logger::info("[HAL] [i2c] [SCAN] " + std::string(label ? label : ""));
    int nDevices = 0;
    for (uint8_t address = 1; address < 0x7F; ++address) {
        uint8_t dummy = 0;
        int result = i2c_read_blocking(config.i2c_port, address, &dummy, 1, false);
        if (result >= 0) {
            logger::info("[HAL] [i2c] [I2C device found at address 0x" + std::to_string((int)address) + "]");
            ++nDevices;
        }
    }
    if (nDevices == 0) {
        logger::info("[HAL] [i2c] [No I2C devices found]");
    } else {
        logger::info("[HAL] [i2c] [done]");
    }
}

void I2c::reset() {
    logger::info("[HAL] [i2c] [RESET i2c]");
    uint8_t buf[] = { 0x6B, 0x80 };
    i2c_write_blocking(config.i2c_port, config.address, buf, 2, false);
    sleep_ms(100);
    buf[1] = 0x00;
    i2c_write_blocking(config.i2c_port, config.address, buf, 2, false);
    sleep_ms(10);
}

}