#pragma once 

#include "service_base.hpp"
#include "hal/pwm.hpp"
#include "hal/gpio.hpp"

namespace service {

class MotorDrive: public IService {
public:
    MotorDrive(core::IMiddleware& middleware);
    void init() override;
    void move(uint8_t id, int16_t signal);
    void moveFoward(uint8_t id, uint16_t signal);
    void moveBackward(uint8_t id, uint16_t signal);
    void stop(uint8_t id);
    void registerMotor(uint8_t pinA, uint8_t pinB, uint8_t pinPwm);
private:
    struct Channel {
        uint8_t pinA;
        uint8_t pinB;
        hal::Pwm pwm;
    };
    core::IMiddleware& middleware;
    std::vector<Channel> channels;
};

}