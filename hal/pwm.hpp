#pragma once 

#include <cstdint>

namespace hal {

class Pwm {
public:
    explicit Pwm(const uint8_t pin);
    void init();
    void writePwm(uint16_t pwm);
private:
    uint8_t pin;
    uint32_t slice_num;
    uint32_t chann;
};

}