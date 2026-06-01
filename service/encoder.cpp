#include "encoder.hpp"
#include "hal/gpio.hpp"
#include <iostream>

namespace service {

Encoder* Encoder::instance = nullptr;
std::unordered_map<uint32_t, size_t> Encoder::channelByPin;

Encoder::Encoder(core::IMiddleware& middleware)
: middleware(middleware) {
    channels.clear();
    instance = this;
}

void Encoder::registerEncoder(uint8_t pinA, uint8_t pinB) {
    channels.push_back(Channel{pinA, pinB, 0});
}

Encoder::Channel* Encoder::findChannel(uint32_t gpio) {
    if(!instance) {
        return nullptr;
    }

    auto it = channelByPin.find(gpio);
    if(it == channelByPin.end()) {
        return nullptr;
    }

    size_t index = it->second;
    if(index >= instance->channels.size()) {
        return nullptr;
    }

    return &instance->channels[index];
}

void Encoder::init() {
    middleware.subscribe([this](const core::Message& msg) {
        if(msg.compareTopic(core::Topics::READ_SPEED)) {
            auto& readSpeedMsg = static_cast<const core::ReadSpeedMessage&>(msg);
            readSpeed(readSpeedMsg.id, readSpeedMsg.speed);
        }
    },
    core::Topics::READ_SPEED,
    false
    );

    channelByPin.clear();

    for(size_t i = 0; i < channels.size(); ++i) {
        Channel& channel = channels[i];

        hal::Gpio::init(channel.pinA);
        hal::Gpio::init(channel.pinB);

        hal::Gpio::setInput(channel.pinA);
        hal::Gpio::setInput(channel.pinB);

        hal::Gpio::pullUp(channel.pinA);
        hal::Gpio::pullUp(channel.pinB);

        hal::Gpio::setExternalInterrupt(channel.pinA, true, true, &Encoder::pulseCallback);
        channelByPin[channel.pinA] = i;
    }
}

void Encoder::readSpeed(uint8_t id, double& speed) {
    if(channels.empty()) {
        speed = 0.0;
        return;
    }
    speed = 42.5;
    //speed = static_cast<double>(channels[id].pulseCount);
}

void Encoder::pulseCallback(uint32_t gpio, uint32_t event) {
    if(!instance) {
        return;
    }

    if(!hal::Gpio::checkRisingEdge(event)) {
        return;
    }

    Channel* channel = findChannel(gpio);
    if(!channel) {
        return;
    }

    bool pinALevel = hal::Gpio::read(channel->pinA);
    bool pinBLevel = hal::Gpio::read(channel->pinB);

    if(pinALevel == pinBLevel) {
        channel->pulseCount++;
    } else {
        channel->pulseCount--;
    }
}

}
