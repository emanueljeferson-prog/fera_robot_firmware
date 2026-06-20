//#include <stdio.h>
//#include <stdlib.h>
#include <hardware/gpio.h>
//#include <hardware/irq.h>
#include <cstdint>

namespace hal {

class Gpio {
public:
    Gpio() = delete;
    ~Gpio() = delete;
    static void init(const uint8_t pin);
    static void setOutput(const uint8_t pin);
    static void setInput(const uint8_t pin);
    static void pullUp(const uint8_t pin);
    static bool read(const uint8_t pin);
    static void write(const uint8_t pin, const bool value);
    static void setExternalInterrupt(const uint8_t pin, bool rising_edge, bool enable, gpio_irq_callback_t callback);
    static bool checkRisingEdge(const uint32_t event);
    static void gpioSetFunction(uint8_t gpio, enum gpio_function_rp2040 fn); 
};

}