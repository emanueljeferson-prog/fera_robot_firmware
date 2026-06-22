#pragma once

#include <cstdint>
#include <array>
#include <string>

namespace config {

struct PolynomialGain {
    double a0;
    double a1;
    double a2;
    double a3;

    constexpr double evaluate(double x) const {
        return a0 + a1 * x + a2 * x * x + a3 * x * x * x;
    }
};

struct EncoderDriveConfig {
    uint8_t pin_a;
    uint8_t pin_b;
    uint32_t reduction_factor;
    uint32_t pulses_per_rev;
};

struct MotorDriverConfig {
    uint8_t pin_a;
    uint8_t pin_b;
    uint32_t pwm_wrap;
};

struct PIDConfig {
    PolynomialGain kp;
    PolynomialGain ki;
    PolynomialGain kd;
    uint32_t period_ms;
    int32_t out_min;
    int32_t out_max;
};

struct MotorConfig {
    EncoderDriveConfig encoder;
    MotorDriverConfig driver;
    PIDConfig pid;
};

struct MicroRosConfig {
    std::string node_name;
    std::string publish_topic;
    std::string subscribe_topic;
};

namespace Mpu6500 {
    static constexpr uint8_t ADDRESS = 0x68;
    static constexpr double ACCEL_SENSITIVITY = 16384.0; // LSB/g for ±2g
    static constexpr double GYRO_SENSITIVITY = 131.0; // LSB/(°/s) for ±250°/s
    static constexpr double TEMP_SENSITIVITY = 340.0; // LSB/°C
    static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
    static constexpr uint8_t REG_GYRO_XOUT_H = 0x43;
    static constexpr uint8_t REG_TEMP_OUT_H = 0x41;
    static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
    static constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
    static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
    static constexpr uint8_t REG_PWR_MGMT_2 = 0x6C;
    static constexpr uint8_t REG_USER_CTRL = 0x6A;
    static constexpr uint8_t REG_I2C_MST_CTRL = 0x24;
    static constexpr uint8_t REG_INT_PIN_CFG = 0x37;
    static constexpr uint8_t REG_WHO_AM_I = 0x75;
    static constexpr uint8_t REG_I2C_SLV0_ADDR = 0x25;
    static constexpr uint8_t REG_I2C_SLV0_REG = 0x26;
    static constexpr uint8_t REG_I2C_SLV0_CTRL = 0x27;
    static constexpr uint8_t REG_EXT_SENS_DATA_00 = 0x49;
    static constexpr uint8_t REG_I2C_SLV0_DO = 0x63;
    static constexpr uint8_t I2C_SLV0_EN = 0x80;
    static constexpr uint8_t SDA_PIN = 4;
    static constexpr uint8_t SCL_PIN = 5;
    static constexpr uint32_t BAUD_RATE = 400000; // 400 kHz
}

namespace Lsm303 {
    static constexpr uint8_t ADDRESS = 0x0C;
    static constexpr double MAG_SENSITIVITY = 0.15; // μT/LSB for 16-bit output
    static constexpr uint8_t REG_MAG_XOUT_H = 0x03;
    static constexpr uint8_t SDA_PIN = 4;
    static constexpr uint8_t SCL_PIN = 5;
    static constexpr uint32_t BAUD_RATE = 400000; // 400 kHz
}

namespace ImuConfig {
    static constexpr double GRAVITY = 9.80665; // m/s²
    static constexpr double DEG2RAD = 3.14159265358979323846 / 180.0; // Degrees to radians conversion factor
    static constexpr double ACCEL_X_OFFSET = 0.0; // g
    static constexpr double ACCEL_Y_OFFSET = 0.0; // g
    static constexpr double ACCEL_Z_OFFSET = 0.0; // g
    static constexpr double GYRO_X_OFFSET = 0.0; // °/s
    static constexpr double GYRO_Y_OFFSET = 0.0; // °/s
    static constexpr double GYRO_Z_OFFSET = 0.0; // °/s
    static constexpr double MAG_X_OFFSET = 0.0; // μT
    static constexpr double MAG_Y_OFFSET = 0.0; // μT   
    static constexpr double MAG_Z_OFFSET = 0.0; // μT
    static constexpr double TEMP_OFFSET = 36.53; // °C
}

namespace TaskConfig {
    static constexpr uint16_t imuTaskStackSize = 1024;
    static constexpr uint16_t motorTaskStackSize = 1024;
    static constexpr uint16_t urosTaskStackSize = 1024;
    static constexpr uint16_t stateMachineTaskStackSize = 1024;
    static constexpr uint16_t imuTaskPriority = 3;
    static constexpr uint16_t motorTaskPriority = 3;
    static constexpr uint16_t urosTaskPriority = 2;
    static constexpr uint16_t stateMachineTaskPriority = 1;
    static constexpr uint32_t imuTaskPeriodMs = 30;
    static constexpr uint32_t motorTaskPeriodMs = 30;
    static constexpr uint32_t urosTaskPeriodMs = 300;
    static constexpr uint32_t stateMachineTaskPeriodMs = 1000;
}

static constexpr std::array<MotorConfig, 2> motorConfigs = {
    MotorConfig{
        EncoderDriveConfig{18, 19, 496, 11},
        MotorDriverConfig{20, 21, 1000},
        PIDConfig{
            PolynomialGain{131.6, 10.01, 0.01, 50.0},
            PolynomialGain{0.0, 0.0, 0.0, 0.0},
            PolynomialGain{0.0, 0.0, 0.0, 0.0},
            10,
            -100,
            100
        }
    },
    MotorConfig{
        EncoderDriveConfig{16, 17, 496, 11},
        MotorDriverConfig{22, 23, 1000},
        PIDConfig{
            PolynomialGain{131.6, 10.01, 0.01, 50.0},
            PolynomialGain{0.0, 0.0, 0.0, 0.0},
            PolynomialGain{0.0, 0.0, 0.0, 0.0},
            10,
            -100,
            100
        }
    }
};

static constexpr size_t motorCount = motorConfigs.size();

static const MicroRosConfig microRosConfig {
    "pico_node",
    "sensor_data",
    "speed_cmd"
};

}