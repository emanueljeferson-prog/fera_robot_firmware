#include "pwm.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <string>
#include "logger/logger.hpp"
#include <hardware/pwm.h>
//#include <hardware/gpio.h>

namespace hal {

Pwm::Pwm(const uint8_t pin)
: pin(pin), slice_num(0), chann(0) {}

void Pwm::init() {
    //LOG_INFO("[HALL] [PWM: %u] [INIT]", pin);
    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(pin);
    chann = pwm_gpio_to_channel(pin);
    pwm_set_clkdiv(slice_num, 125.0f);
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_chan_level(slice_num, chann, 0);
    pwm_set_enabled(slice_num, true);
}

void Pwm::writePwm(uint16_t pwm) {
    //LOG_INFO("[HAL] [PWM: %u] [WRITE PWM]: %u", pin, pwm);
    if(pwm > PWM_WRAP) {
        pwm = PWM_WRAP;
    }
    pwm_set_chan_level(slice_num, chann, pwm);
}

}