#include <iostream>
#include "core/middleware.hpp"
#include "application/motor.hpp"
#include "service/rtos.hpp"
#include "service/encoder.hpp"

int main() {
    auto broker = core::Middleware(); 
    auto rtos = service::RtosService(broker);
    auto encoder = service::Encoder(broker);
    auto motor = app::Motor(1, broker);
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