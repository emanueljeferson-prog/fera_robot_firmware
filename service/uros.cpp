#include "uros.hpp"
#include "config.hpp"
#include "uroslib/pico_uart_transports.h"
#include "rosidl_runtime_c/string_functions.h"
#include <pico/stdlib.h>

namespace service {

MicroRos::MicroRos(core::IMiddleware& middleware):
    microros(config::microRosConfig.node_name, config::microRosConfig.publish_topic, config::microRosConfig.subscribe_topic, config::microRosConfig.system_config),
    middleware(middleware)
{
    LOG_INFO("[SERVICE] [MICROS ROS] [START]");
    speed_cmd = new robot_interfaces__msg__SpeedCmd();
    sensor_data = new robot_interfaces__msg__SensorData();
    system_config = new robot_interfaces__msg__SystemConfig();
}

MicroRos::~MicroRos() {

}

void MicroRos::init() {
    fms.init();
    robot_interfaces__msg__SpeedCmd__init(speed_cmd);
    robot_interfaces__msg__SensorData__init(sensor_data);
    robot_interfaces__msg__SystemConfig__init(system_config);
    auto timer_callback = [this](rcl_timer_t *timer, int64_t last_call_time) {
        if(fms.checkFinish()) {
            microros.publish(*sensor_data);
            LOG_INFO("[SERVICE] [MICROS ROS] [PUBLISH]");
        } 
    };
    auto subscription_callback = [this](const void* message) {
        *speed_cmd = *(robot_interfaces__msg__SpeedCmd*)(message);
        sensor_data->motor_01 = speed_cmd->motor_01;
        sensor_data->motor_02 = speed_cmd->motor_02;
        LOG_INFO("[SERVICE] [MICROROS] [SUBSCRIPTION] ref motor1: %f ----------- ref motor2: %f", speed_cmd->motor_01, speed_cmd->motor_02);
    };
    auto system_config_callback = [this](const void* message) {
        *system_config = *(robot_interfaces__msg__SystemConfig*)(message);
        // Motor drive parameters
        config::motorConfigs[0].drive.dead_zone_min = system_config->dead_zone_min.data;
        config::motorConfigs[0].drive.dead_zone_max = system_config->dead_zone_max.data;
        // Encoder drive parameters
        config::motorConfigs[0].encoder.conversion_factor = system_config->conversion_factor;
        // PID controller parameters 
        config::motorConfigs[0].pid.kp = system_config->kp;
        config::motorConfigs[0].pid.ki = system_config->ki;
        config::motorConfigs[0].pid.kd = system_config->kd;
        config::motorConfigs[0].pid.out_min = system_config->out_min.data;
        config::motorConfigs[0].pid.out_max = system_config->out_max.data;
        // Sample time tasks
        config::TaskConfig::imuTaskPeriodMs = system_config->sample_time_motor.data;
        config::TaskConfig::motorTaskPeriodMs = system_config->sample_time_imu.data;
        config::TaskConfig::gpsTaskPeriodMs = system_config->sample_time_gps.data;
        // IMU calibration
        config::ImuConfig::accel_x_offset = system_config->accel_offset.x;
        config::ImuConfig::accel_y_offset = system_config->accel_offset.y;
        config::ImuConfig::accel_z_offset = system_config->accel_offset.z;
        config::ImuConfig::gyro_x_offset = system_config->gyro_offset.x;
        config::ImuConfig::gyro_y_offset = system_config->gyro_offset.y;
        config::ImuConfig::gyro_z_offset = system_config->gyro_offset.z;
        config::ImuConfig::mag_x_offset = system_config->mag_offset.x;
        config::ImuConfig::mag_y_offset = system_config->mag_offset.y;
        config::ImuConfig::mag_z_offset = system_config->mag_offset.z;
        config::ImuConfig::temp_offset = system_config->temp_offset;
        // Configured system
        config::configured_system_status = true; 
        LOG_INFO("[SERVICE] [MICROS ROS] [SYSTEM CONFIG RECEIVED]");
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
    microros.register_system_config_callback(system_config_callback);
    LOG_INFO("[SERVICE] [MICROROS] [REGISTER TASK]: microros_timer");
    middleware.enqueueTask(desc_timer_task);
    LOG_INFO("[SERVICE] [MICROROS] [REGISTER TASK] DONE");
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::UROS_SPEED)) {
                const auto& speed_msg = static_cast<const core::MicroRosMessageSpeed&>(msg); 
                sensor_data->motor_01 = speed_msg.speed_1;
                sensor_data->motor_02 = speed_msg.speed_2;
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
                sensor_data->accel = imu_msg.accel; 
                sensor_data->gyro = imu_msg.gyro; 
                sensor_data->mag = imu_msg.mag; 
                sensor_data->temperature = imu_msg.temp; 
                fms.update(core::Topics::UROS_IMU);
            }
        },
        core::Topics::UROS_IMU,
        false
    );
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::UROS_GPS)) {
                const auto& gps_msg = static_cast<const core::MicroRosMessageGps&>(msg); 
                rosidl_runtime_c__String__assign(&sensor_data->gps_data.data, gps_msg.gps_data.c_str());
                fms.update(core::Topics::UROS_GPS);
            }
        },
        core::Topics::UROS_GPS,
        false
    );
}

void MicroRos::spin() {
    microros.spin(
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SensorData),
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SpeedCmd),
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SystemConfig)
    );
}

void MicroRos::spinWrapper(void* params) {
    MicroRos* uros = static_cast<MicroRos*>(params);
    LOG_INFO("[SERVICE] [MICROROS] [SPIN TASK] ENTER");
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
    else if(tp == core::Topics::UROS_GPS) {
        gps_flag = true; 
    }
    else {}

    const bool done = speed_flag && imu_flag && gps_flag; 

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
    gps_flag = false; 
    state = States::INIT; 
}

}

// Implementação do operador de conversão Vector3D -> geometry_msgs__msg__Point
namespace core {

Vector3D::operator geometry_msgs__msg__Point() const {
    geometry_msgs__msg__Point point;
    point.x = x;
    point.y = y;
    point.z = z;
    return point;
}

}
