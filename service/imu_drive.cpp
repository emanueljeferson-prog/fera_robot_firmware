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

void ImuDrive::readImu(core::Vector3D& accel, core::Vector3D& gyro, core::Vector3D& mag, double& temp) {
    LOG_INFO("[SERVICE] [IMU DRIVE] [READ IMU]");
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

    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_accel_xout_h, accel_raw_data, 6);
    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_gyro_xout_h, gyro_raw_data, 6);
    acell_giro_device.readRegister(config::mpu6500Config.address, config::mpu6500Config.reg_temp_out_h, temp_raw_data, 2);

    accelRaw[0] = toInt16(accel_raw_data[0], accel_raw_data[1]);
    accelRaw[1] = toInt16(accel_raw_data[2], accel_raw_data[3]);
    accelRaw[2] = toInt16(accel_raw_data[4], accel_raw_data[5]);

    gyroRaw[0] = toInt16(gyro_raw_data[0], gyro_raw_data[1]);
    gyroRaw[1] = toInt16(gyro_raw_data[2], gyro_raw_data[3]);
    gyroRaw[2] = toInt16(gyro_raw_data[4], gyro_raw_data[5]);

    // Bloco vem como [XH, XL, ZH, ZL, YH, YL] -> atenção a ordem Z antes de Y.
    /*magRaw[0] = toInt16(mag_raw_data[0], mag_raw_data[1]);  // X = XH,XL
    magRaw[1] = toInt16(mag_raw_data[4], mag_raw_data[5]);  // Y = YH,YL (bytes 4,5)
    magRaw[2] = toInt16(mag_raw_data[2], mag_raw_data[3]);  // Z = ZH,ZL (bytes 2,3)*/

    tempRaw[0] = toInt16(temp_raw_data[0], temp_raw_data[1]);

    accel.x = accelRaw[0] * (config::ImuConfig::gravity / config::mpu6500Config.accel_sensitivity) + config::ImuConfig::accel_x_offset;
    accel.y = -accelRaw[1] * (config::ImuConfig::gravity / config::mpu6500Config.accel_sensitivity) + config::ImuConfig::accel_y_offset;
    accel.z = -accelRaw[2] * (config::ImuConfig::gravity / config::mpu6500Config.accel_sensitivity) + config::ImuConfig::accel_z_offset;
    gyro.x = gyroRaw[0] * (config::ImuConfig::deg2rad / config::mpu6500Config.gyro_sensitivity) + config::ImuConfig::gyro_x_offset;
    gyro.y = gyroRaw[1] * (config::ImuConfig::deg2rad / config::mpu6500Config.gyro_sensitivity) + config::ImuConfig::gyro_y_offset;
    gyro.z = gyroRaw[2] * (config::ImuConfig::deg2rad / config::mpu6500Config.gyro_sensitivity) + config::ImuConfig::gyro_z_offset;
    /*mag.x = magRaw[0] * config::Lsm303Dlhc::MAG_SCALE_XY + config::ImuConfig::mag_x_offset;
    mag.y = magRaw[1] * config::Lsm303Dlhc::MAG_SCALE_XY + config::ImuConfig::mag_y_offset;
    mag.z = magRaw[2] * config::Lsm303Dlhc::MAG_SCALE_Z + config::ImuConfig::mag_z_offset;*/
    temp = (tempRaw[0] / config::mpu6500Config.temp_sensitivity) + config::ImuConfig::temp_offset;
    LOG_INFO("[SERVICE] [IMU DRIVE] [READ IMU] [DONE]");
}

}