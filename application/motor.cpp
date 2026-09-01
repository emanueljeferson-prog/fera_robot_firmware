#include "motor.hpp"

namespace app {

Motor::Motor(core::IMiddleware& middleware)
: middleware(middleware), speed_01(0.0), speed_02(0.0) {
    snprintf(commandTaskName, sizeof(commandTaskName), "motor_command");
    snprintf(speedTaskName, sizeof(speedTaskName), "motor_speed");
    //LOG_INFO("[APP] [MOTOR: %u] [START] task=%s/%s]", id, commandTaskName, speedTaskName);
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
    //LOG_INFO("[APP] [MOTOR: %u] [INIT]", id);
}

void Motor::control() {
    auto command_msg = core::MotorCommandMessage(0,-1000);
    middleware.publish(command_msg);
    //LOG_INFO("[APP] [MOTOR: %u] [CONTROL TASK]", id);
}

void Motor::readSpeed() {
    auto read_speed_msg_01 = core::ReadSpeedMessage(0, speed_01);
    auto read_speed_msg_02 = core::ReadSpeedMessage(1, speed_02);
    middleware.publish(read_speed_msg_01);
    middleware.publish(read_speed_msg_02);
    auto uros_speed_msg = core::MicroRosMessageSpeed(speed_01, speed_02); 
    middleware.publish(uros_speed_msg);
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