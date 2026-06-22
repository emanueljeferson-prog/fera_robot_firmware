#include "motor.hpp"

namespace app {

Motor::Motor(uint8_t id, core::IMiddleware& middleware)
: id(id), middleware(middleware), speed(0.0) {
    snprintf(commandTaskName, sizeof(commandTaskName), "motor_command_%u", id);
    snprintf(speedTaskName, sizeof(speedTaskName), "motor_speed_%u", id);
    LOG_INFO("[APP] [MOTOR: %u] [START] task=%s/%s]", id, commandTaskName, speedTaskName);
} 

void Motor::init() {
    auto desc_command_task = 
        core::TaskDescription{
            .task_name = commandTaskName,
            .stack_size = config::TaskConfig::motorTaskStackSize,
            .priority = config::TaskConfig::motorTaskPriority,
            .task = &Motor::controlWrapper,
            .parameters = this
        };
    auto desc_speed_task = 
        core::TaskDescription{
            .task_name = speedTaskName,
            .stack_size = config::TaskConfig::motorTaskStackSize,
            .priority = config::TaskConfig::motorTaskPriority,
            .task = &Motor::readSpeedWrapper,
            .parameters = this
        };
    middleware.enqueueTask(desc_command_task);
    middleware.enqueueTask(desc_speed_task);
    LOG_INFO("[APP] [MOTOR: %u] [INIT]", id);
}

void Motor::control() {
    auto command_msg = core::MotorCommandMessage(id, -1000);
    middleware.publish(command_msg);
    LOG_INFO("[APP] [MOTOR: %u] [CONTROL TASK]", id);
}

void Motor::readSpeed() {
    auto read_speed_msg = core::ReadSpeedMessage(id, speed);
    middleware.publish(read_speed_msg);
    auto uros_speed_msg = core::MicroRosMessageSpeed(speed, 0.55); 
    middleware.publish(uros_speed_msg);
    LOG_INFO("[APP] [MOTOR: %u] [READ SPEED TASK]: %f", id, speed);
}

void Motor::controlWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    core::RtosTimer timer;
    for(;;) {
        motor->control();
        timer.delay_ms(config::TaskConfig::motorTaskPeriodMs);
    }
}

void Motor::readSpeedWrapper(void* params) {
    Motor* motor = static_cast<Motor*>(params);
    core::RtosTimer timer;
    for(;;) {   
        motor->readSpeed();
        timer.delay_ms(config::TaskConfig::motorTaskPeriodMs);
    }
}

}