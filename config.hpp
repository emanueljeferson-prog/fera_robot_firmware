#pragma once

#include <cstdint>
#include <array>
#include <string>

namespace config {

inline uint8_t device_address = 0x02;

inline bool configured_system_status = false;

struct EncoderDriveConfig {
    uint8_t pin_a;
    uint8_t pin_b;
};

struct MotorDriveConfig {
    uint8_t pin_a;
    uint8_t pin_b;
    uint32_t pwm_wrap;
    int16_t dead_zone_min;
    int16_t dead_zone_max; 
};

struct PIDConfig {
    double ref;
    double kp;
    double ki;
    double kd;
    int32_t out_min;
    int32_t out_max;
};

struct MotorConfig {
    EncoderDriveConfig encoder;
    MotorDriveConfig drive;
    PIDConfig pid;
};

struct MicroRosConfig {
    std::string node_name;
    std::string publish_topic;
    std::string subscribe_topic;
    std::string system_config;
};

struct Mpu6500Config {
    // I2C Address and pins
    uint8_t address;
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t i2c_baud_rate;
    
    // Register addresses
    uint8_t reg_accel_xout_h;
    uint8_t reg_gyro_xout_h;
    uint8_t reg_temp_out_h;
    uint8_t reg_accel_config;
    uint8_t reg_gyro_config;
    uint8_t reg_pwr_mgmt_1;
    uint8_t reg_pwr_mgmt_2;
    uint8_t reg_user_ctrl;
    uint8_t reg_i2c_mst_ctrl;
    uint8_t reg_int_pin_cfg;
    uint8_t reg_who_am_i;
    uint8_t reg_i2c_slv0_addr;
    uint8_t reg_i2c_slv0_reg;
    uint8_t reg_i2c_slv0_ctrl;
    uint8_t reg_ext_sens_data_00;
    uint8_t reg_i2c_slv0_do;
    uint8_t i2c_slv0_en;
};

inline Mpu6500Config mpu6500Config = {
    0x68,      // address
    0,         // sda_pin
    1,         // scl_pin
    400000,    // i2c_baud_rate
    0x3B,      // reg_accel_xout_h
    0x43,      // reg_gyro_xout_h
    0x41,      // reg_temp_out_h
    0x1C,      // reg_accel_config
    0x1B,      // reg_gyro_config
    0x6B,      // reg_pwr_mgmt_1
    0x6C,      // reg_pwr_mgmt_2
    0x6A,      // reg_user_ctrl
    0x24,      // reg_i2c_mst_ctrl
    0x37,      // reg_int_pin_cfg
    0x75,      // reg_who_am_i
    0x25,      // reg_i2c_slv0_addr
    0x26,      // reg_i2c_slv0_reg
    0x27,      // reg_i2c_slv0_ctrl
    0x49,      // reg_ext_sens_data_00
    0x63,      // reg_i2c_slv0_do
    0x80       // i2c_slv0_en
};

namespace TaskConfig {
    inline uint16_t imuTaskStackSize = 1024;
    inline uint16_t motorTaskStackSize = 1024;
    inline uint16_t urosTaskStackSize = 1024;
    inline uint16_t stateMachineTaskStackSize = 1024;
    inline uint16_t imuTaskPriority = 3;
    inline uint16_t motorTaskPriority = 3;
    inline uint16_t urosTaskPriority = 2;
    inline uint16_t stateMachineTaskPriority = 1;
    inline uint32_t imuTaskPeriodMs = 30;
    inline uint32_t motorTaskPeriodMs = 30;
}

inline std::array<MotorConfig, 2> motorConfigs = {
    MotorConfig{
        EncoderDriveConfig{19, 18},
        MotorDriveConfig{20, 21, 1000, 0, 0},
        PIDConfig{
            0,
            0,
            0,
            0,
            -100,
            100
        }
    },
    MotorConfig{
        EncoderDriveConfig{16, 17},
        MotorDriveConfig{22, 26, 1000, 0, 0},
        PIDConfig{
            0,
            0,
            0,
            0,
            -100,
            100
        }
    }
};

static constexpr size_t motorCount = motorConfigs.size();

inline const MicroRosConfig microRosConfig {
    "pico_node",
    "sensor_data",
    "speed_cmd"
};

}