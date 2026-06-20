#include "imu_drive.hpp"
#include "core/message.hpp"
#include <vector>
#include <cmath>
#include "pico/stdlib.h"
#include "logger/logger.hpp"

namespace service {
namespace {
    static constexpr uint8_t MPU9250_ADDRESS = 0x68;
    static constexpr uint8_t AK8963_ADDRESS = 0x0C;

    static constexpr uint8_t MPU9250_REG_PWR_MGMT_1 = 0x6B;
    static constexpr uint8_t MPU9250_REG_PWR_MGMT_2 = 0x6C;
    static constexpr uint8_t MPU9250_REG_USER_CTRL = 0x6A;
    static constexpr uint8_t MPU9250_REG_I2C_MST_CTRL = 0x24;
    static constexpr uint8_t MPU9250_REG_INT_PIN_CFG = 0x37;
    static constexpr uint8_t MPU9250_REG_WHO_AM_I = 0x75;
    static constexpr uint8_t MPU9250_REG_ACCEL_XOUT_H = 0x3B;
    static constexpr uint8_t MPU9250_REG_GYRO_XOUT_H = 0x43;
    static constexpr uint8_t MPU9250_REG_ACCEL_CONFIG = 0x1C;
    static constexpr uint8_t MPU9250_REG_GYRO_CONFIG = 0x1B;
    static constexpr uint8_t MPU9250_REG_I2C_SLV0_ADDR = 0x25;
    static constexpr uint8_t MPU9250_REG_I2C_SLV0_REG = 0x26;
    static constexpr uint8_t MPU9250_REG_I2C_SLV0_CTRL = 0x27;
    static constexpr uint8_t MPU9250_REG_EXT_SENS_DATA_00 = 0x49;
    static constexpr uint8_t MPU9250_REG_I2C_SLV0_DO = 0x63;
    static constexpr uint8_t MPU9250_I2C_SLV0_EN = 0x80;

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
    i2c.readRegister(MPU9250_ADDRESS, MPU9250_REG_WHO_AM_I, whoami, 1);
    if (whoami.empty() || (whoami[0] != 0x71 && whoami[0] != 0x73))
    {
        logger::info("[SERVICE] [IMU DRIVER] [DEVICE NOT DETECTED]");
    }
    sleep_ms(10);
    i2c.writeRegister(MPU9250_ADDRESS, MPU9250_REG_USER_CTRL, static_cast<uint8_t>(0x00));
    sleep_ms(10);
    i2c.writeRegister(MPU9250_ADDRESS, MPU9250_REG_PWR_MGMT_1, 0x00);
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
    i2c.readRegister(MPU9250_ADDRESS, MPU9250_REG_ACCEL_XOUT_H, raw_data, 14);

    accelRaw[0] = toInt16(raw_data[0], raw_data[1]);
    accelRaw[1] = toInt16(raw_data[2], raw_data[3]);
    accelRaw[2] = toInt16(raw_data[4], raw_data[5]);
    tempRaw[0] = toInt16(raw_data[6], raw_data[7]);
    gyroRaw[0] = toInt16(raw_data[8], raw_data[9]);
    gyroRaw[1] = toInt16(raw_data[10], raw_data[11]);
    gyroRaw[2] = toInt16(raw_data[12], raw_data[13]);

    accel[0] = accelRaw[0] * (GRAVITY / ACCEL_SENSITIVITY);
    accel[1] = accelRaw[1] * (GRAVITY / ACCEL_SENSITIVITY);
    accel[2] = accelRaw[2] * (GRAVITY / ACCEL_SENSITIVITY);
    gyro[0] = gyroRaw[0] * (DEG2RAD / GYRO_SENSITIVITY);
    gyro[1] = gyroRaw[1] * (DEG2RAD / GYRO_SENSITIVITY);
    gyro[2] = gyroRaw[2] * (DEG2RAD / GYRO_SENSITIVITY);
    temp[0] = (tempRaw[0] / TEMP_SENSITIVITY); 
}

}
