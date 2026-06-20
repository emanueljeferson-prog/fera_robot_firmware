#pragma once

#include <cstdint>
#include <vector>
#include "hardware/spi.h"

namespace hal {

enum SPI_TYPE {SPI0,SPI1};

struct SpiConfig
{
    spi_inst_t* spi_port;
    uint8_t read_bit;
    uint8_t cs; 
    uint8_t miso;
    uint8_t mosi;
    uint8_t sclk;
};

class Spi {
public:
    Spi(const SPI_TYPE spi_type);
    void chipSelect();
    void chipDeselect();
    void readRegister(const uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght);
    void reset();
private:
    SpiConfig config;
};

}