#include "motor_drive.hpp"
#include <iostream>

namespace service {

MotorDrive::MotorDrive(core::IMiddleware& middleware)
: middleware(middleware) {
    channels.clear();
    std::cout << "[SERVICE] [MOTOR DRIVE] [START]" << std::endl;
}

void MotorDrive::registerMotor(uint8_t pinA, uint8_t pinB) {
    channels.push_back(Channel{hal::Pwm(pinA), hal::Pwm(pinB)});
}

void MotorDrive::init() {

    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::MOTOR_COMMAND)) {
                const auto& command_msg = static_cast<const core::MotorCommandMessage&>(msg); 
                move(command_msg.id, command_msg.signal);
            }
        },
        core::Topics::MOTOR_COMMAND,
        false
    );
    for(auto& channel: channels) {
        channel.pwmA.init();
        channel.pwmB.init();
    }
}

void MotorDrive::move(uint8_t id, int16_t signal) {
    if(signal > 0) {
        moveFoward(id, signal);
    } else if(signal < 0) {
        moveBackward(id, -signal);
    } else {
        stop(id);
    }
}

void MotorDrive::moveFoward(uint8_t id, uint16_t signal) {
    if(id >= channels.size()) {
        return;
    }

    Channel& channel = channels[id];
    channel.pwmA.writePwm(signal);
    channel.pwmB.writePwm(0);
}

void MotorDrive::moveBackward(uint8_t id, uint16_t signal) {
    if(id >= channels.size()) {
        return;
    }

    Channel& channel = channels[id];
    channel.pwmA.writePwm(0);
    channel.pwmB.writePwm(signal);
}

void MotorDrive::stop(uint8_t id) {
    if(id >= channels.size()) {
        return;
    }

    Channel& channel = channels[id];
    channel.pwmA.writePwm(0);
    channel.pwmB.writePwm(0);
}

}