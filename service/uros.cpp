#include "uros.hpp"
#include "config.hpp"
#include "uroslib/pico_uart_transports.h"
#include <pico/stdlib.h>

namespace service {

MicroRos::MicroRos(core::IMiddleware& middleware):
    microros(config::microRosConfig.node_name, config::microRosConfig.publish_topic, config::microRosConfig.subscribe_topic),
    middleware(middleware)
{
    LOG_INFO("[SERVICE] [MICROS ROS] [START]");
    speed_cmd = new robot_interfaces__msg__SpeedCmd();
    sensor_data = new robot_interfaces__msg__SensorData();
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
            LOG_INFO("[SERVICE] [MICROS ROS] [PUBLISH]");
        } 
    };
    auto subscription_callback = [this](const void* message) {
        *speed_cmd = *(robot_interfaces__msg__SpeedCmd*)(message);
        //LOG_INFO("[SERVICE] [MICROROS] [SUBSCRIPTION] ref motor1: %f ----------- ref motor2: %f", speed_cmd->motor_01, speed_cmd->motor_02);
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
