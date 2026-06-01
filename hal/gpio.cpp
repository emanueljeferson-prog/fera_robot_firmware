//#include <stdio.h>
//#include <stdlib.h>
//#include <hardware/gpio.h>
//#include <hardware/irq.h>
#include <cstdint>
#include "gpio.hpp"

namespace hal {

void Gpio::init(const uint8_t pin) {
    //gpio_init(pin); 
}    

void Gpio::setOutput(const uint8_t pin) {
    //gpio_set_dir(pin, GPIO_OUT);
}

void Gpio::setInput(const uint8_t pin) {
    //gpio_set_dir(pin, GPIO_IN);
}

void Gpio::pullUp(const uint8_t pin) {
    //gpio_pull_up(pin);
}

bool Gpio::read(const uint8_t pin) {
    //return gpio_get(pin);
    return false;
}   

void Gpio::write(const uint8_t pin, const bool value) {
    //gpio_put(pin, value);
}   

void Gpio::setExternalInterrupt(const uint8_t pin, bool rising_edge, bool enable, void(*callback)(uint32_t, uint32_t)) {
    //gpio_set_irq_enabled_with_callback(pin, rising_edge ? GPIO_IRQ_EDGE_RISE : GPIO_IRQ_EDGE_FALL, enable, callback);
}

bool Gpio::checkRisingEdge(const uint32_t event) {
    //return event & GPIO_IRQ_EDGE_RISE;
    return false;
}

}
