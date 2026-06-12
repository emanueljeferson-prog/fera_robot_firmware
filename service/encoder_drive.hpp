#pragma once

#include "service_base.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace service {

class EncoderDrive: public IService {
public:
    explicit EncoderDrive(core::IMiddleware& middleware);
    void init() override;
    void readSpeed(uint8_t id, double& speed);
    void registerEncoderDrive(uint8_t pinA, uint8_t pinB);
    static void pulseCallback(uint32_t gpio, uint32_t event);

private:
    struct Channel {
        uint8_t pinA;
        uint8_t pinB;
        int32_t pulseCount;
    };

    core::IMiddleware& middleware;
    std::vector<Channel> channels;
    static EncoderDrive* instance;
    static std::unordered_map<uint32_t, size_t> channelByPin;
    static Channel* findChannel(uint32_t gpio);
};

}