#pragma once 

#include <functional>
#include "uroslib/micro_ros.h"
#include "service_base.hpp"

namespace service {

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
    IMicroRos microros; 
};

}