#include <iostream>
#include "config.hpp"
#include "core/middleware.hpp"
#include "application/motor.hpp"
#include "service/rtos.hpp"
#include "service/encoder.hpp"

int main() {
    auto broker = core::Middleware(); 
    auto rtos = service::Rtos(broker);
    auto encoder = service::Encoder(broker);
    auto motor = app::Motor(0, broker);

    for(size_t i = 0; i < config::motorCount; ++i) {
        const auto& motorCfg = config::motorConfigs[i];
        encoder.registerEncoder(motorCfg.encoder.pin_a, motorCfg.encoder.pin_b);
    }

    // services 
    rtos.init();
    encoder.init();
    // applications
    motor.init();
    // middleware
    broker.run();

    // scheduler
    rtos.startScheduler();
    return 0;
}