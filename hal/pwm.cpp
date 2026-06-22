#include "pwm.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <string>
#include "logger/logger.hpp"
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

    static bool led_initialized = false;
    if (!led_initialized) {
        gpio_init(25);
        gpio_set_dir(25, GPIO_OUT);
        led_initialized = true;
    }
}

void Pwm::writePwm(uint16_t pwm) {
    LOG_INFO("[HAL] [PWM: %u] [WRITE PWM]: %u", pin, pwm);

    static bool led_state = false;
    led_state = !led_state;
    gpio_put(25, led_state ? 1 : 0);
}

}