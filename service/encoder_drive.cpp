#include "encoder_drive.hpp"
#include "hal/gpio.hpp"
#include "logger/logger.hpp"

namespace service {

EncoderDrive* EncoderDrive::instance = nullptr;
std::unordered_map<uint32_t, size_t> EncoderDrive::channelByPin;

EncoderDrive::EncoderDrive(core::IMiddleware& middleware)
: middleware(middleware) {
    channels.clear();
    instance = this;
    logger::info("[SERVICE] [ENCODER DRIVE] [START]");
}

void EncoderDrive::registerEncoderDrive(uint8_t pinA, uint8_t pinB) {
    channels.push_back(Channel{pinA, pinB, 0});
}

EncoderDrive::Channel* EncoderDrive::findChannel(uint32_t gpio) {
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

void EncoderDrive::init() {
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

        hal::Gpio::setExternalInterrupt(channel.pinA, true, true, &EncoderDrive::pulseCallback);
        channelByPin[channel.pinA] = i;
    }
}

void EncoderDrive::readSpeed(uint8_t id, double& speed) {
    if(channels.empty()) {
        speed = 0.0;
        return;
    }
    //speed = 42.5;
    logger::info("[SERVICE] [ENCODER DRIVE] [READ SPEED]: " + std::to_string(channels[id].pulseCount));
    speed = static_cast<double>(channels[id].pulseCount);
}

void EncoderDrive::pulseCallback(unsigned int gpio, long unsigned int event) {
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
