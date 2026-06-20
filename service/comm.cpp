#include "comm.hpp"
#include "logger/logger.hpp"

namespace service {

Router::Router()
: spi(hal::SPI_TYPE::SPI0), i2c() {
    logger::info("[SERVICE] [ROUTER] [START]");
}

void Router::read(const uint8_t address, std::vector<uint8_t>& buffer, const size_t lenght, const core::ProtocolType protocol) {
    logger::info("[SERVICE] [ROUTER] [READ]");
    switch (protocol) {
        case core::ProtocolType::SPI: {
            spi.readRegister(address, buffer, lenght);
            break;
        }
        case core::ProtocolType::I2C: {
            i2c.readRegister(address, buffer, lenght);
            break;
        }
        default: {
            break;
        }
    }
}

void Router::write(const uint8_t address, const std::vector<uint8_t> buffer, const size_t lenght, const core::ProtocolType protocol) {}

Communication::Communication(core::IMiddleware& middleware)
: middleware(middleware), router() {
    logger::info("[SERVICE] [COMMUNICATION] [START]");
}

void Communication::init() {
    logger::info("[SERVICE] [COMMUNICATION] [INIT]");
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::EXTERN_MESSAGE_SEND)) {
                const auto& extern_msg = static_cast<const core::ExternMessageReceive&>(msg); 
                router.read(extern_msg.address, extern_msg.payload, extern_msg.lenght, extern_msg.protocol);
            }
        },
        core::Topics::EXTERN_MESSAGE_SEND,
        false
    ); 
}

}