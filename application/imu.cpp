#include "imu.hpp"

namespace app {

Imu::Imu(core::IMiddleware& middleware)
: middleware(middleware) {
    std::cout << "[APP] [IMU] [START]" << std::endl;
}

void Imu::init() {

}

void Imu::readImu() {
    std::vector<uint8_t> accelRaw(6, 0);
    std::vector<uint8_t> gyroRaw(6, 0);
    std::vector<uint8_t> magRaw(6, 0);
    std::vector<uint8_t> tempRaw(2, 0);

    auto get_accel_msg = core::ExternMessageReceive(accelRaw, core::ProtocolType::SPI, 0x3B, core::Topics::EXTERN_MESSAGE_SEND);
    auto get_gyro_msg = core::ExternMessageReceive(gyroRaw, core::ProtocolType::SPI, 0x46, core::Topics::EXTERN_MESSAGE_SEND);
    auto get_mag_msg = core::ExternMessageReceive(magRaw, core::ProtocolType::I2C, 0x0C, core::Topics::EXTERN_MESSAGE_SEND);
    auto get_temp_msg = core::ExternMessageReceive(tempRaw, core::ProtocolType::SPI, 0x41, core::Topics::EXTERN_MESSAGE_SEND);

    middleware.publish(get_accel_msg);
    middleware.publish(get_gyro_msg);
    middleware.publish(get_mag_msg);
    middleware.publish(get_temp_msg);
    //accel = 

    //std::cout << "[APP] [ACCEL]" << "[x, y, z]: " << (int) << std::endl;
}

void Imu::readImuWrapper(void* params) {

}

}
