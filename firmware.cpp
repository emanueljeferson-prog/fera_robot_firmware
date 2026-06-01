#include <iostream>
#include "config.hpp"
#include "core/middleware.hpp"
#include "application/motor.hpp"
#include "service/rtos.hpp"
#include "service/encoder.hpp"
#include "service/motor_drive.hpp"

int main() {
    auto broker = core::Middleware(); 
    auto rtos = service::Rtos(broker);
    auto encoder = service::Encoder(broker);
    auto motor_drive = service::MotorDrive(broker);
    auto motor = app::Motor(1, broker);

    for(size_t i = 0; i < config::motorCount; ++i) {
        const auto& motorCfg = config::motorConfigs[i];
        encoder.registerEncoder(motorCfg.encoder.pin_a, motorCfg.encoder.pin_b);
        motor_drive.registerMotor(motorCfg.driver.pin_a, motorCfg.driver.pin_b);
    }

    // services 
    rtos.init();
    encoder.init();
    motor_drive.init();
    // applications
    motor.init();
    // middleware
    broker.run();

    // scheduler
    rtos.startScheduler();
    return 0;
}