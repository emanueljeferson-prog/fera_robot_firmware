#pragma once 

#include "service_base.hpp"
#include "hal/pwm.hpp"

namespace service {

class MotorDrive: public IService {
public:
    MotorDrive(core::IMiddleware& middleware);
    void init() override;
    void move(uint8_t id, int16_t signal);
    void moveFoward(uint8_t id, uint16_t signal);
    void moveBackward(uint8_t id, uint16_t signal);
    void stop(uint8_t id);
    void registerMotor(uint8_t pinA, uint8_t pinB);
private:
    struct Channel {
        hal::Pwm pwmA;
        hal::Pwm pwmB;
    };
    core::IMiddleware& middleware;
    std::vector<Channel> channels;
};

}