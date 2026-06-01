#include <iostream>
#include "core/middleware.hpp"
#include "application/motor.hpp"
#include "service/rtos.hpp"

int main() {
    auto broker = core::Middleware(); 
    auto rtos = service::RtosService(broker);
    auto motor = app::Motor(1, broker);

    rtos.init();
    motor.init();
    
    broker.run();

    rtos.startScheduler();
    return 0;
}