#include "motor.hpp"
#include <cmath>

namespace app {

Motor::Motor(core::IMiddleware& middleware)
: middleware(middleware), speed_right(0), speed_left(0), integral_right(0.0), integral_left(0.0) {
    snprintf(commandTaskName, sizeof(commandTaskName), "motor_command");
    snprintf(speedTaskName, sizeof(speedTaskName), "motor_speed");
    //LOG_INFO("[APP] [START] task=%s/%s]", commandTaskName, speedTaskName);
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
    //LOG_INFO("[APP] [INIT]");
}

void Motor::control() {
    // LOG_INFO("MOTOR CONTROL");
    const float dt = static_cast<float>(config::TaskConfig::motorTaskPeriodMs) / 1000.0f;

    // --- estados anteriores (para derivada) ---
    const double prev_err_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.e_k1;
    const double prev_err_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.e_k1;

    const double ref_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.ref;
    const double ref_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.ref;
    const double err_r = ref_r - speed_right;
    const double err_l = ref_l - speed_left;

    const float Kp_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.kp;
    const float Ki_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.ki;
    const float Kd_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.kd;
    const float Kp_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.kp;
    const float Ki_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.ki;
    const float Kd_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.kd;

    const double de_dt_r = (err_r - prev_err_r) / dt;
    const double de_dt_l = (err_l - prev_err_l) / dt;

    double u_r_tent = Kp_r * err_r + Ki_r * integral_right + Kd_r * de_dt_r;
    double u_l_tent = Kp_l * err_l + Ki_l * integral_left + Kd_l * de_dt_l;

    const bool sat_max_r = (u_r_tent > config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_max);
    const bool sat_min_r = (u_r_tent < config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_min);
    const bool bloquear_integral_r = (sat_max_r && err_r > 0) || (sat_min_r && err_r < 0);

    const bool sat_max_l = (u_l_tent > config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_max);
    const bool sat_min_l = (u_l_tent < config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_min);
    const bool bloquear_integral_l = (sat_max_l && err_l > 0) || (sat_min_l && err_l < 0);

    if (!bloquear_integral_r) {
        integral_right += err_r * dt;
    }
    if (!bloquear_integral_l) {
        integral_left += err_l * dt;
    }

    double u_r = Kp_r * err_r + Ki_r * integral_right + Kd_r * de_dt_r;
    double u_l = Kp_l * err_l + Ki_l * integral_left + Kd_l * de_dt_l;

    if (u_r > config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_max) {
        u_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_max;
    } else if (u_r < config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_min) {
        u_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_min;
    }

    if (u_l > config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_max) {
        u_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_max;
    } else if (u_l < config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_min) {
        u_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_min;
    }

    auto applyDeadZone = [](double cmd, int16_t dead_zone) {
        const double threshold = std::fabs(static_cast<double>(dead_zone));
        const double magnitude = std::fabs(cmd);
        if (magnitude <= threshold) {
            return 0.0;
        }
        return std::copysign(magnitude - threshold, cmd);
    };

    u_r = applyDeadZone(u_r, config::motorConfigs[config::MotorId::MOTOR_RIGHT].drive.dead_zone_max);
    u_l = applyDeadZone(u_l, config::motorConfigs[config::MotorId::MOTOR_LEFT].drive.dead_zone_max);

    if (u_r > config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_max) {
        u_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_max;
    } else if (u_r < config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_min) {
        u_r = config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.out_min;
    }

    if (u_l > config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_max) {
        u_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_max;
    } else if (u_l < config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_min) {
        u_l = config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.out_min;
    }

    config::motorConfigs[config::MotorId::MOTOR_RIGHT].pid.e_k1 = err_r;
    config::motorConfigs[config::MotorId::MOTOR_LEFT].pid.e_k1 = err_l;

    auto command_msg_right = core::MotorCommandMessage(0, static_cast<int16_t>(u_r));
    auto command_msg_left = core::MotorCommandMessage(1, static_cast<int16_t>(u_l));
    middleware.publish(command_msg_right);
    middleware.publish(command_msg_left);

    LOG_INFO("%d,%d,%d,%d ---- %d,%d,%d,%d", (int)speed_right, (int)ref_r, (int)err_r, (int)u_r, (int)speed_left, (int)ref_l, (int)err_l, (int)u_l);
}

void Motor::readSpeed() {
    auto read_speed_msg_right = core::ReadSpeedMessage(0, speed_right);
    auto read_speed_msg_left = core::ReadSpeedMessage(1, speed_left);
    middleware.publish(read_speed_msg_right);
    middleware.publish(read_speed_msg_left);
    auto uros_speed_msg = core::MicroRosMessageSpeed(static_cast<int16_t>(speed_right), static_cast<int16_t>(speed_left)); 
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