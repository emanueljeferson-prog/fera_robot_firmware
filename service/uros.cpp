#include "uros.hpp"
#include "config.hpp"
#include "uroslib/pico_uart_transports.h"
#include <pico/stdlib.h>
#include "logger/logger.hpp"

namespace service {

MicroRos::MicroRos(core::IMiddleware& middleware):
    microros(config::microRosConfig.node_name, config::microRosConfig.publish_topic, config::microRosConfig.subscribe_topic),
    middleware(middleware)
{
    speed_cmd = new robot_interfaces__msg__SpeedCmd();
    sensor_data = new robot_interfaces__msg__SensorData();
    logger::info("[SERVICE] [MICROS ROS] [START]");
}

MicroRos::~MicroRos() {

}

void MicroRos::init() {
    fms.init();
    robot_interfaces__msg__SpeedCmd__init(speed_cmd);
    robot_interfaces__msg__SensorData__init(sensor_data);
    auto timer_callback = [this](rcl_timer_t *timer, int64_t last_call_time) {
        if(fms.checkFinish()) {
            microros.publish(*sensor_data);
            logger::info("[SERVICE] [MICROS ROS] [PUBLISH]");
        } 
    };
    auto subscription_callback = [this](const void* message) {
        *speed_cmd = *(robot_interfaces__msg__SpeedCmd*)(message);
        //printf("ref motor1: %f ----------- ref motor2: %f", speed_cmd->motor_01, speed_cmd->motor_02);
    };
    auto register_spin_msg = core::RegisterTask(
        core::TaskDescription{
            .task_name = "micro_ros",
            .stack_size = 1024,
            .priority = 2,
            .task = &MicroRos::spinWrapper,
            .parameters = this
        },
        core::Topics::REGISTER_TASK
    );
    microros.register_timer_callback(timer_callback);
    microros.register_subscription_callback(subscription_callback);
    middleware.publish(register_spin_msg);
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
                sensor_data->accel.x = imu_msg.accel[0]; 
                sensor_data->accel.y = imu_msg.accel[1]; 
                sensor_data->accel.z = imu_msg.accel[2]; 
                sensor_data->gyro.x = imu_msg.gyro[0]; 
                sensor_data->gyro.y = imu_msg.gyro[1]; 
                sensor_data->gyro.z = imu_msg.gyro[2]; 
                sensor_data->mag.x = imu_msg.mag[0]; 
                sensor_data->mag.y = imu_msg.mag[1]; 
                sensor_data->mag.z = imu_msg.mag[2]; 
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
                sensor_data->gps_latitude = gps_msg.latitude;
                sensor_data->gps_longitude = gps_msg.longitude;
                sensor_data->gps_altitude = gps_msg.altitude;
                sensor_data->timestamp.sec = gps_msg.seconds;
                sensor_data->timestamp.nanosec = gps_msg.nanoseconds; 
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
        ROSIDL_GET_MSG_TYPE_SUPPORT(robot_interfaces, msg, SpeedCmd)
    );
}

void MicroRos::spinWrapper(void* params) {
    MicroRos* uros = static_cast<MicroRos*>(params);
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
    return finished;    
}

void StateMachine::reset() {
    speed_flag = false; 
    imu_flag = false; 
    gps_flag = false; 
    state = States::INIT; 
}

}