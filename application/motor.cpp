#include "motor.hpp"
#include <iostream>

namespace app {

Motor::Motor(uint8_t id, core::IMiddleware& middleware)
: id(id), middleware(middleware) {
    std::cout << "[APP] [MOTOR: " << (int)id << "] [START]" << std::endl;
} 
void Motor::init() {
    auto register_task_control_msg = core::RegisterTask(
        core::TaskDescription{
            .task_name = "motor_control_" + std::to_string((int)id),
            .stack_size = 1024,
            .priority = 1,
            .task = &Motor::controlWrapper,
            .parameters = this
        },
        core::Topics::REGISTER_TASK
    );
    auto register_task_speed_msg = core::RegisterTask(
        core::TaskDescription{
            .task_name = "motor_speed_" + std::to_string((int)id),
            .stack_size = 1024,
            .priority = 1,
            .task = &Motor::readSpeedWrapper,
            .parameters = this
        },
        core::Topics::REGISTER_TASK
    );
    middleware.publish(register_task_control_msg);
    middleware.publish(register_task_speed_msg);
}
void Motor::control() {
    std::cout << "[APP] [MOTOR: " << (int)id << "] [CONTROL TASK]" << std::endl;
}

void Motor::readSpeed() {
    std::cout << "[APP] [MOTOR: " << (int)id << "] [READ SPEED TASK]" << std::endl;
}

void Motor::controlWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    for(;;) {
        motor->control();
        auto delay_msg = core::DelayTask(1000, core::Topics::DEALY_TASK);
        motor->middleware.publish(delay_msg);
    }
}

void Motor::readSpeedWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    for(;;) {
        motor->readSpeed();
        auto delay_msg = core::DelayTask(1000, core::Topics::DEALY_TASK);
        motor->middleware.publish(delay_msg);
    }
}


}