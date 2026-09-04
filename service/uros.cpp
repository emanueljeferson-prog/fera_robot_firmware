#include "uros.hpp"
#include "config.hpp"
#include "uroslib/pico_uart_transports.h"
#include "rosidl_runtime_c/string_functions.h"
#include <pico/stdlib.h>

namespace service {

static uint64_t boot_time_us = 0;
static bool timestamp_initialized = false;

static void updateTimestamp(builtin_interfaces__msg__Time& timestamp) {
    if (!timestamp_initialized) {
        boot_time_us = time_us_64();
        timestamp_initialized = true;
    }
    
    uint64_t elapsed_us = time_us_64() - boot_time_us;
    
    // Convert microseconds to seconds and nanoseconds with full precision
    uint32_t sec = (uint32_t)(elapsed_us / 1000000ULL);
    uint32_t nsec = (uint32_t)((elapsed_us % 1000000ULL) * 1000ULL);
    
    timestamp.sec = sec;
    timestamp.nanosec = nsec;
}

MicroRos::MicroRos(core::IMiddleware& middleware):
    microros(config::microRosConfig.node_name, config::microRosConfig.publish_topic, config::microRosConfig.subscribe_topic),
    middleware(middleware)
{
    //LOG_INFO("[SERVICE] [MICROS ROS] [START]");
    speed_cmd = new robot_interfaces__msg__SpeedCmd();
    sensor_data = new robot_interfaces__msg__SensorData();
}

MicroRos::~MicroRos() {

}

void MicroRos::init() {
    fms.init();
    robot_interfaces__msg__SpeedCmd__init(speed_cmd);
    robot_interfaces__msg__SensorData__init(sensor_data);
    
    // Initialize sensor_data with device address
    sensor_data->address = config::device_address;
    
    auto timer_callback = [this](rcl_timer_t *timer, int64_t last_call_time) {
        if(fms.checkFinish()) {
            updateTimestamp(sensor_data->timestamp);
            microros.publish(*sensor_data);
            //LOG_INFO("[SERVICE] [MICROS ROS] [PUBLISH]");
        } 
    };
    auto subscription_callback = [this](const void* message) {
        *speed_cmd = *(robot_interfaces__msg__SpeedCmd*)(message);
        sensor_data->motor_speed_left = speed_cmd->motor_cmd_left;
        sensor_data->motor_speed_right = speed_cmd->motor_cmd_right;
        // Update motorConfigs with the received speed command and PID parameters - motor 1
        config::motorConfigs[0].pid.ref = speed_cmd->motor_cmd_left;
        config::motorConfigs[0].pid.kp = speed_cmd->motor_kp_left;
        config::motorConfigs[0].pid.ki = speed_cmd->motor_ki_left;
        config::motorConfigs[0].pid.kd = speed_cmd->motor_kd_left;
        // Update motorConfigs with the received speed command and PID parameters - motor 2
        config::motorConfigs[1].pid.ref = speed_cmd->motor_cmd_right;
        config::motorConfigs[1].pid.kp = speed_cmd->motor_kp_right;
        config::motorConfigs[1].pid.ki = speed_cmd->motor_ki_right;
        config::motorConfigs[1].pid.kd = speed_cmd->motor_kd_right;
        //LOG_INFO("[SERVICE] [MICROROS] [SUBSCRIPTION] ref motor1: %f ----------- ref motor2: %f", speed_cmd->motor_left, speed_cmd->motor_right);
    };
    auto desc_timer_task = 
        core::TaskDescription{
            .task_name = "microros_timer",
            .stack_size = 1024,
            .priority = config::TaskConfig::urosTaskPriority,
            .task = &MicroRos::spinWrapper,
            .parameters = this
        };
    microros.register_timer_callback(timer_callback);
    microros.register_subscription_callback(subscription_callback);
    //LOG_INFO("[SERVICE] [MICROROS] [REGISTER TASK]: microros_timer");
    middleware.enqueueTask(desc_timer_task);
    //LOG_INFO("[SERVICE] [MICROROS] [REGISTER TASK] DONE");
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::UROS_SPEED)) {
                const auto& speed_msg = static_cast<const core::MicroRosMessageSpeed&>(msg); 
                sensor_data->motor_speed_left = speed_msg.speed_1;
                sensor_data->motor_speed_right = speed_msg.speed_2;
                fms.update(core::Topics::UROS_SPEED);
            }
        },
        core::Topics::UROS_SPEED,
        false
    );
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::UROS_IMU)) {
                const auto& imu_msg = static_cast<const core::MicroRosMessageImu&>(msg); 
                sensor_data->accel.x = imu_msg.accel.x;
                sensor_data->accel.y = imu_msg.accel.y;
                sensor_data->accel.z = imu_msg.accel.z;
                sensor_data->gyro.x = imu_msg.gyro.x;
                sensor_data->gyro.y = imu_msg.gyro.y;
                sensor_data->gyro.z = imu_msg.gyro.z;
                sensor_data->mag.x = imu_msg.mag.x;
                sensor_data->mag.y = imu_msg.mag.y;
                sensor_data->mag.z = imu_msg.mag.z;
                sensor_data->temperature = imu_msg.temp; 
                fms.update(core::Topics::UROS_IMU);
            }
        },
        core::Topics::UROS_IMU,
        false
    );
}

void MicroRos::spin() {
    microros.spin(
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SensorData),
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SpeedCmd)
    );
}

void MicroRos::spinWrapper(void* params) {
    MicroRos* uros = static_cast<MicroRos*>(params);
    //LOG_INFO("[SERVICE] [MICROROS] [SPIN TASK] ENTER");
    uros->spin();
}

StateMachine::StateMachine() {
    state = States::NO_STATE; 
}

void StateMachine::init() {
    state = States::INIT; 
}

void StateMachine::update(core::Topics tp) {
    if(tp == core::Topics::UROS_SPEED) {
        speed_flag = true;
    }
    else if(tp == core::Topics::UROS_IMU) {
        imu_flag = true;
    }
    else {}

    const bool done = speed_flag && imu_flag; 

    if(done) {
        state = States::FINISH; 
    }
    else {
        state = States::WAITING; 
    }
}

bool StateMachine::checkFinish() {
    const bool finished = state == States::FINISH; 
    reset();
    //return finished;
    return true;    
}

void StateMachine::reset() {
    speed_flag = false; 
    imu_flag = false; 
    state = States::INIT; 
}

}
