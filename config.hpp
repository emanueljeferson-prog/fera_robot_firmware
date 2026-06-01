#pragma once

#include <cstdint>
#include <array>

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

struct EncoderConfig {
    uint8_t pin_a;
    uint8_t pin_b;
    uint32_t reduction_factor;
    uint32_t pulses_per_rev;
};

struct DriverConfig {
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
    EncoderConfig encoder;
    DriverConfig driver;
    PIDConfig pid;
};

static constexpr std::array<MotorConfig, 2> motorConfigs = {
    MotorConfig{
        EncoderConfig{18, 19, 496, 11},
        DriverConfig{20, 21, 1000},
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
        EncoderConfig{16, 17, 496, 11},
        DriverConfig{22, 23, 1000},
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

}