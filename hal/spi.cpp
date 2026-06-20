
#include "spi.hpp"
#include "gpio.hpp"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "logger/logger.hpp"

namespace hal {

Spi::Spi(const SPI_TYPE spi_type) {
    logger::info("[HAL] [SPI] [START]");
    if (spi_type == SPI_TYPE::SPI0) {
        config.spi_port = spi0;
    } 
    else {
        config.spi_port = spi1;
    } 
    config.read_bit = 0x80;
    config.miso = 4;
    config.cs = 5; 
    config.mosi = 7;
    config.sclk = 6; 
    //stdio_init_all();
    spi_init(config.spi_port, 500 * 1000);
    Gpio::gpioSetFunction(config.miso, GPIO_FUNC_SPI);
    Gpio::gpioSetFunction(config.sclk, GPIO_FUNC_SPI);
    Gpio::gpioSetFunction(config.mosi, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(config.miso, config.mosi, config.sclk, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    Gpio::init(config.cs);
    Gpio::setOutput(config.cs);
    Gpio::write(config.cs, true);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(config.cs, "SPI CS"));

    reset();
}
void Spi::chipSelect() {
    asm volatile("nop \n nop \n nop");
    Gpio::write(config.cs, false);
    asm volatile("nop \n nop \n nop");
}
void Spi::chipDeselect() {
    asm volatile("nop \n nop \n nop");
    Gpio::write(config.cs, true);
    asm volatile("nop \n nop \n nop");
}
void Spi::readRegister(uint8_t reg, std::vector<uint8_t>& buffer, const size_t lenght) {
    logger::info("[HAL] [SPI] [READ REGISTER: " + std::to_string((int)reg) + "]");
    buffer.assign(lenght, 0);
    reg |= config.read_bit;
    chipSelect();
    spi_write_blocking(config.spi_port, &reg, 1);
    sleep_ms(10);
    spi_read_blocking(config.spi_port, 0, buffer.data(), lenght);
    chipDeselect();
    sleep_ms(10);
}
void Spi::reset() {
    logger::info("[HAL] [SPI] [RESET SPI]");
    uint8_t buf[] = {0x6B, 0x00};
    chipSelect();
    spi_write_blocking(config.spi_port, buf, 2);
    chipDeselect();
}

}

