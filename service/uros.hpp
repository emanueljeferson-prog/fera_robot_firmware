#pragma once 

#include <functional>
#include "uroslib/micro_ros.h"
#include "service_base.hpp"

namespace service {

typedef struct {
    bool motor_flag;
    bool imu_flag;
    bool gps_flag;
} SensorDataStatus_t;

class StateMachine {
public:
    StateMachine();
    void init();
    void update(core::Topics tp);
    bool checkFinish();
    void reset(); 
    enum States {
        NO_STATE,
        INIT,
        WAITING,
        FINISH
    }; 
private:
    bool speed_flag; 
    bool imu_flag;
    bool gps_flag;
    States state; 
};

class MicroRos {
public:
    MicroRos(core::IMiddleware& middleware);
    ~MicroRos();
    void init(); 
    void spin();
    static void spinWrapper(void* params);
private:
    core::IMiddleware& middleware;  
    robot_interfaces__msg__SpeedCmd* speed_cmd;
    robot_interfaces__msg__SensorData* sensor_data;
    SensorDataStatus_t sensor_data_status;
    IMicroRos microros; 
    StateMachine fms;
};

}