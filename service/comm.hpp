#pragma once 

#include "service_base.hpp"
#include <vector>
#include "spi.hpp"
#include "i2c.hpp"

namespace service {

class Router {
public:
    Router();
    void read(const uint8_t address, std::vector<uint8_t>& buffer, const size_t lenght, const core::ProtocolType protocol);
    void write(const uint8_t address, const std::vector<uint8_t> buffer, const size_t lenght, const core::ProtocolType protocol);
private:
    hal::Spi spi;
    hal::I2c i2c;
};

class Communication: public IService {
public: 
    explicit Communication(core::IMiddleware& middleware);
    void init() override;
private:
    core::IMiddleware& middleware;
    Router router; 
    //std::vector<core::Message> buffer;
};

}
