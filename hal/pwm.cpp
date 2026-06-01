#include "pwm.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <hardware/pwm.h>
//#include <hardware/gpio.h>

namespace hal {

Pwm::Pwm(const uint8_t pin)
: pin(pin), slice_num(0), chann(0) {}

void Pwm::init() {
    /*gpio_set_function(pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(pin);
    chann = pwm_gpio_to_channel(pin);
    pwm_set_wrap(slice_num, PWM_WRAP); 
    pwm_set_chan_level(slice_num, chann, 0);
    pwm_set_enabled(slice_num, true);*/
}

void Pwm::writePwm(uint16_t pwm) {
    //pwm_set_chan_level(slice_num, chann, pwm);
    std::cout << "[HAL] [PWM: " << (int)pin << "] [WRITE PWM]: " << pwm << std::endl;
}

}