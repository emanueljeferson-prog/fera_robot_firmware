#include "imu_drive.hpp"
#include "config.hpp"
#include "core/message.hpp"
#include <vector>
#include <cmath>
#include "pico/stdlib.h"
#include "logger/logger.hpp"

namespace service {
namespace {
    static inline int16_t toInt16(uint8_t high, uint8_t low) {
        return static_cast<int16_t>((static_cast<uint16_t>(high) << 8) | low);
    }
}

ImuDrive::ImuDrive(core::IMiddleware& middleware)
: middleware(middleware) {
    logger::info("[SERVICE] [IMU DRIVER] [START]");
}

void ImuDrive::init() {
    middleware.subscribe(
        [this](const core::Message& msg) {
            if (msg.compareTopic(core::Topics::READ_IMU)) {
                auto& readImuMsg = static_cast<const core::ReadImuMessage&>(msg);
                readImu(readImuMsg.accel, readImuMsg.gyro, readImuMsg.mag, readImuMsg.temp);
            }
        },
        core::Topics::READ_IMU,
        false
    );
    i2c.reset();
    std::vector<uint8_t> whoami;
    i2c.readRegister(config::mpu9250::ADDRESS, config::mpu9250::REG_WHO_AM_I, whoami, 1);
    if (whoami.empty() || (whoami[0] != 0x71 && whoami[0] != 0x73))
    {
        logger::info("[SERVICE] [IMU DRIVER] [DEVICE NOT DETECTED]");
    }
    sleep_ms(10);
    i2c.writeRegister(config::mpu9250::ADDRESS, config::mpu9250::REG_USER_CTRL, static_cast<uint8_t>(0x00));
    sleep_ms(10);
    i2c.writeRegister(config::mpu9250::ADDRESS, config::mpu9250::REG_PWR_MGMT_1, 0x00);
    sleep_ms(100);
}

void ImuDrive::readImu(std::vector<double>& accel, std::vector<double>& gyro, std::vector<double>& mag, std::vector<double>& temp) {
    logger::info("[SERVICE] [IMU DRIVER] [READ IMU]");
    std::vector<int16_t> accelRaw(3);
    std::vector<int16_t> gyroRaw(3);
    std::vector<int16_t> magRaw(3);
    std::vector<int16_t> tempRaw(1);
    accelRaw.assign(3, 0);
    gyroRaw.assign(3, 0);
    magRaw.assign(3, 0);
    tempRaw.assign(1, 0);

    std::vector<uint8_t> raw_data;
    i2c.readRegister(config::mpu9250::ADDRESS, config::mpu9250::REG_ACCEL_XOUT_H, raw_data, 14);

    accelRaw[0] = toInt16(raw_data[0], raw_data[1]);
    accelRaw[1] = toInt16(raw_data[2], raw_data[3]);
    accelRaw[2] = toInt16(raw_data[4], raw_data[5]);
    tempRaw[0] = toInt16(raw_data[6], raw_data[7]);
    gyroRaw[0] = toInt16(raw_data[8], raw_data[9]);
    gyroRaw[1] = toInt16(raw_data[10], raw_data[11]);
    gyroRaw[2] = toInt16(raw_data[12], raw_data[13]);

    accel[0] = accelRaw[0] * (config::mpu9250::GRAVITY / config::mpu9250::ACCEL_SENSITIVITY) + config::mpu9250::ACCEL_X_OFFSET;
    accel[1] = accelRaw[1] * (config::mpu9250::GRAVITY / config::mpu9250::ACCEL_SENSITIVITY) + config::mpu9250::ACCEL_Y_OFFSET;
    accel[2] = accelRaw[2] * (config::mpu9250::GRAVITY / config::mpu9250::ACCEL_SENSITIVITY) + config::mpu9250::ACCEL_Z_OFFSET;
    gyro[0] = gyroRaw[0] * (config::mpu9250::DEG2RAD / config::mpu9250::GYRO_SENSITIVITY) + config::mpu9250::GYRO_X_OFFSET;
    gyro[1] = gyroRaw[1] * (config::mpu9250::DEG2RAD / config::mpu9250::GYRO_SENSITIVITY) + config::mpu9250::GYRO_Y_OFFSET;
    gyro[2] = gyroRaw[2] * (config::mpu9250::DEG2RAD / config::mpu9250::GYRO_SENSITIVITY) + config::mpu9250::GYRO_Z_OFFSET;
    temp[0] = (tempRaw[0] / config::mpu9250::TEMP_SENSITIVITY) + config::mpu9250::TEMP_OFFSET;
}

}
