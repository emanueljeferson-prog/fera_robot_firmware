#include "imu_drive.hpp"
#include "config.hpp"
#include "core/message.hpp"
#include <vector>
#include <cmath>
#include "pico/stdlib.h"

namespace service {
namespace {
    static inline int16_t toInt16(uint8_t high, uint8_t low) {
        return static_cast<int16_t>((static_cast<uint16_t>(high) << 8) | low);
    }
}

ImuDrive::ImuDrive(core::IMiddleware& middleware)
: middleware(middleware), acell_giro_device() {
    //LOG_INFO("[SERVICE] [IMU DRIVER] [START]");
}

void ImuDrive::init() {
    //LOG_INFO("[SERVICE] [IMU DRIVER] [INIT]");
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

    hal::I2cConfig mpu6500Config;
    mpu6500Config.i2c_port = i2c_default;
    mpu6500Config.address = config::Mpu6500::ADDRESS;
    mpu6500Config.sda = config::Mpu6500::SDA_PIN;
    mpu6500Config.scl = config::Mpu6500::SCL_PIN;
    mpu6500Config.baudrate = config::Mpu6500::BAUD_RATE;
    mpu6500Config.who_ami_reg = config::Mpu6500::REG_WHO_AM_I;
    acell_giro_device.init(mpu6500Config);
    std::vector<uint8_t> whoami;
    acell_giro_device.readRegister(mpu6500Config.address, config::Mpu6500::REG_WHO_AM_I, whoami, 1);
    if (whoami.empty())
    {
        LOG_ERROR("[SERVICE] [IMU DRIVER] [DEVICE NOT DETECTED]");
    }

    sleep_ms(10);
    acell_giro_device.writeRegister(mpu6500Config.address, config::Mpu6500::REG_USER_CTRL, static_cast<uint8_t>(0x00));
    sleep_ms(10);
    acell_giro_device.writeRegister(mpu6500Config.address, config::Mpu6500::REG_PWR_MGMT_1, 0x00);
    sleep_ms(100);
    //LOG_INFO("[SERVICE] [IMU DRIVER] [INIT] [DONE]");
}

void ImuDrive::readImu(core::Vector3D& accel, core::Vector3D& gyro, core::Vector3D& mag, double& temp) {
    //LOG_INFO("[SERVICE] [IMU DRIVER] [READ IMU]");
    std::vector<int16_t> accelRaw(3);
    std::vector<int16_t> gyroRaw(3);
    std::vector<int16_t> magRaw(3);
    std::vector<int16_t> tempRaw(1);
    accelRaw.assign(3, 0);
    gyroRaw.assign(3, 0);
    magRaw.assign(3, 0);
    tempRaw.assign(1, 0);

    std::vector<uint8_t> accel_raw_data(6, 0);
    std::vector<uint8_t> gyro_raw_data(6, 0);
    std::vector<uint8_t> mag_raw_data(6, 0);
    std::vector<uint8_t> temp_raw_data(2, 0);

    acell_giro_device.readRegister(config::Mpu6500::ADDRESS, config::Mpu6500::REG_ACCEL_XOUT_H, accel_raw_data, 6);
    acell_giro_device.readRegister(config::Mpu6500::ADDRESS, config::Mpu6500::REG_GYRO_XOUT_H, gyro_raw_data, 6);
    //acell_giro_device.readRegister(config::Lsm303::ADDRESS, config::Lsm303::REG_MAG_XOUT_H, mag_raw_data, 6);
    acell_giro_device.readRegister(config::Mpu6500::ADDRESS, config::Mpu6500::REG_TEMP_OUT_H, temp_raw_data, 2);

    accelRaw[0] = toInt16(accel_raw_data[0], accel_raw_data[1]);
    accelRaw[1] = toInt16(accel_raw_data[2], accel_raw_data[3]);
    accelRaw[2] = toInt16(accel_raw_data[4], accel_raw_data[5]);

    gyroRaw[0] = toInt16(gyro_raw_data[0], gyro_raw_data[1]);
    gyroRaw[1] = toInt16(gyro_raw_data[2], gyro_raw_data[3]);
    gyroRaw[2] = toInt16(gyro_raw_data[4], gyro_raw_data[5]);

    magRaw[0] = toInt16(mag_raw_data[0], mag_raw_data[1]);
    magRaw[1] = toInt16(mag_raw_data[2], mag_raw_data[3]);
    magRaw[2] = toInt16(mag_raw_data[4], mag_raw_data[5]);

    tempRaw[0] = toInt16(temp_raw_data[0], temp_raw_data[1]);

    accel.x = accelRaw[0] * (config::ImuConfig::GRAVITY / config::Mpu6500::ACCEL_SENSITIVITY) + config::ImuConfig::ACCEL_X_OFFSET;
    accel.y = accelRaw[1] * (config::ImuConfig::GRAVITY / config::Mpu6500::ACCEL_SENSITIVITY) + config::ImuConfig::ACCEL_Y_OFFSET;
    accel.z = accelRaw[2] * (config::ImuConfig::GRAVITY / config::Mpu6500::ACCEL_SENSITIVITY) + config::ImuConfig::ACCEL_Z_OFFSET;
    gyro.x = gyroRaw[0] * (config::ImuConfig::DEG2RAD / config::Mpu6500::GYRO_SENSITIVITY) + config::ImuConfig::GYRO_X_OFFSET;
    gyro.y = gyroRaw[1] * (config::ImuConfig::DEG2RAD / config::Mpu6500::GYRO_SENSITIVITY) + config::ImuConfig::GYRO_Y_OFFSET;
    gyro.z = gyroRaw[2] * (config::ImuConfig::DEG2RAD / config::Mpu6500::GYRO_SENSITIVITY) + config::ImuConfig::GYRO_Z_OFFSET;
    mag.x = magRaw[0] * config::Lsm303::MAG_SENSITIVITY + config::ImuConfig::MAG_X_OFFSET;
    mag.y = magRaw[1] * config::Lsm303::MAG_SENSITIVITY + config::ImuConfig::MAG_Y_OFFSET;
    mag.z = magRaw[2] * config::Lsm303::MAG_SENSITIVITY + config::ImuConfig::MAG_Z_OFFSET;
    temp = (tempRaw[0] / config::Mpu6500::TEMP_SENSITIVITY) + config::ImuConfig::TEMP_OFFSET;
    LOG_INFO("[SERVICE] [IMU DRIVER] [READ IMU] [DONE]");
}

}
