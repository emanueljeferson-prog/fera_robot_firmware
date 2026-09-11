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
: middleware(middleware), acell_giro_device(), mag_device(), mag_enabled(false) {
    LOG_INFO("[SERVICE] [IMU DRIVE] [START]");
}

void ImuDrive::init() {
    LOG_INFO("[SERVICE] [IMU DRIVE] [INIT]");
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

    hal::I2cConfig i2cCfg;
    i2cCfg.i2c_port = i2c_default;
    i2cCfg.address = config::mpu6500Config.address;
    i2cCfg.sda = config::mpu6500Config.sda_pin;
    i2cCfg.scl = config::mpu6500Config.scl_pin;
    i2cCfg.baudrate = config::mpu6500Config.i2c_baud_rate;
    i2cCfg.who_ami_reg = config::mpu6500Config.reg_who_am_i;
    acell_giro_device.init(i2cCfg);
    acell_giro_device.scanBus("I2C bus scan after MPU6500 init");
    std::vector<uint8_t> whoami;
    acell_giro_device.readRegister(i2cCfg.address, config::mpu6500Config.reg_who_am_i, whoami, 1);
    if (whoami.empty())
    {
        LOG_ERROR("[SERVICE] [IMU DRIVE] [DEVICE NOT DETECTED]");
    }
    // 0x71 = MPU9250 (tem AK8963 embutido). 0x70/0x73/0x78/0x98 (varia por
    // revisao) = MPU6500 puro, sem magnetometro nenhum no chip.
    LOG_INFO("[SERVICE] [IMU DRIVE] [MPU WHOAMI] [0x%02X]", whoami.empty() ? 0xFF : whoami[0]);

    sleep_ms(10);
    acell_giro_device.writeRegister(config::mpu6500Config.address, config::mpu6500Config.reg_user_ctrl, static_cast<uint8_t>(0x00));
    sleep_ms(10);
    acell_giro_device.writeRegister(config::mpu6500Config.address, config::mpu6500Config.reg_pwr_mgmt_1, 0x00);
    sleep_ms(100);
    LOG_INFO("[SERVICE] [IMU DRIVE] [INIT] [DONE]");
}

void ImuDrive::readImu(core::Vector3D& accel, core::Vector3D& gyro, core::Vector3D& mag, int16_t& temp) {
    LOG_INFO("[SERVICE] [IMU DRIVE] [READ IMU]");

    std::vector<uint8_t> accel_raw_data(6, 0);
    std::vector<uint8_t> gyro_raw_data(6, 0);
    std::vector<uint8_t> mag_raw_data(6, 0);
    std::vector<uint8_t> temp_raw_data(2, 0);

    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_accel_xout_h, accel_raw_data, 6);
    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_gyro_xout_h, gyro_raw_data, 6);
    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_temp_out_h, temp_raw_data, 2);

    accel.x = -toInt16(accel_raw_data[0], accel_raw_data[1]);
    accel.y = toInt16(accel_raw_data[2], accel_raw_data[3]);
    accel.z = -toInt16(accel_raw_data[4], accel_raw_data[5]);

    gyro.x = toInt16(gyro_raw_data[0], gyro_raw_data[1]);
    gyro.y = toInt16(gyro_raw_data[2], gyro_raw_data[3]);
    gyro.z = toInt16(gyro_raw_data[4], gyro_raw_data[5]);

    // Bloco vem como [XH, XL, ZH, ZL, YH, YL] -> atenção a ordem Z antes de Y.
    /*mag.x = toInt16(mag_raw_data[0], mag_raw_data[1]);  // X = XH,XL
    mag.y = toInt16(mag_raw_data[4], mag_raw_data[5]);  // Y = YH,YL (bytes 4,5)
    mag.z = toInt16(mag_raw_data[2], mag_raw_data[3]);  // Z = ZH,ZL (bytes 2,3)*/

    temp = toInt16(temp_raw_data[0], temp_raw_data[1]);
    LOG_INFO("[SERVICE] [IMU DRIVE] [READ IMU] [DONE]");
}

}