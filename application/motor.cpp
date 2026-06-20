#include "motor.hpp"

namespace app {

Motor::Motor(uint8_t id, core::IMiddleware& middleware)
: id(id), middleware(middleware), speed(0.0) {
    logger::info("[APP] [MOTOR: " + std::to_string((int)id) + "] [START]");
} 

void Motor::init() {
    auto register_task_command_msg = core::RegisterTask(
        core::TaskDescription{
            .task_name = "motor_command_" + std::to_string((int)id),
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
    middleware.publish(register_task_command_msg);
    middleware.publish(register_task_speed_msg);
    logger::info("[APP] [MOTOR: " + std::to_string((int)id) + "] [INIT]");
}

void Motor::control() {
    auto command_msg = core::MotorCommandMessage(id, -1000, core::Topics::MOTOR_COMMAND);
    middleware.publish(command_msg);
    logger::info("[APP] [MOTOR: " + std::to_string((int)id) + "] [CONTROL TASK]");
}

void Motor::readSpeed() {
    auto read_speed_msg = core::ReadSpeedMessage(id, speed, core::Topics::READ_SPEED);
    middleware.publish(read_speed_msg);
    auto uros_speed_msg = core::MicroRosMessageSpeed(speed, 0.55, core::Topics::UROS_SPEED); 
    middleware.publish(uros_speed_msg);
    logger::info("[APP] [MOTOR: " + std::to_string((int)id) + "] [READ SPEED TASK]: " + std::to_string(speed));
}

void Motor::controlWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    for(;;) {
        motor->control();
        /*auto delay_msg = core::DelayTask(1000, core::Topics::DELAY_TASK);
        motor->middleware.publish(delay_msg);*/
        sleep_ms(1000);
    }
}

void Motor::readSpeedWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    for(;;) {
        motor->readSpeed();
        /*auto delay_msg = core::DelayTask(1000, core::Topics::DELAY_TASK);
        motor->middleware.publish(delay_msg);*/
        sleep_ms(1000);
    }
}

}