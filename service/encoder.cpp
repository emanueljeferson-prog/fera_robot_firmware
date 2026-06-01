#include "encoder.hpp"

namespace service {

Encoder::Encoder(core::IMiddleware& middleware) 
: middleware(middleware) {}

void Encoder::init() {
    middleware.subscribe([this](const core::Message& msg) {
        if(msg.compareTopic(core::Topics::READ_SPEED)) {
            auto& readSpeedMsg = static_cast<const core::ReadSpeedMessage&>(msg);
            readSpeed(readSpeedMsg.speed);
        }
    },
    core::Topics::READ_SPEED,
    false
    );
}

void Encoder::readSpeed(double& speed) {
    speed = 42.5;

}

}