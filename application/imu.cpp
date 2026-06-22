#include "imu.hpp"

namespace app {

Imu::Imu(core::IMiddleware& middleware)
: middleware(middleware) {
    LOG_INFO("[APP] [IMU] [START]");
}

void Imu::init() {
    LOG_INFO("[APP] [IMU] [INIT]");
    auto desc_imu_task = 
        core::TaskDescription{
            .task_name = "imu_task",
            .stack_size = config::TaskConfig::imuTaskStackSize,
            .priority = config::TaskConfig::imuTaskPriority,
            .task = &Imu::readImuWrapper,
            .parameters = this
        };
    middleware.enqueueTask(desc_imu_task);
    LOG_INFO("[APP] [IMU] [INIT] [DONE]");
}

void Imu::readImu() {
    core::Vector3D _accel;
    core::Vector3D _gyro;
    core::Vector3D _mag;
    double _temp;

    auto get_imu_data_msg = core::ReadImuMessage(_accel, _gyro, _mag, _temp);
    middleware.publish(get_imu_data_msg);

    accel = _accel;
    gyro = _gyro;
    mag = _mag;
    temp = _temp;

    LOG_INFO("[APP] [ACCEL] [x, y, z]: %f, %f, %f", accel.x, accel.y, accel.z);
    LOG_INFO("[APP] [GYRO] [x, y, z]: %f, %f, %f", gyro.x, gyro.y, gyro.z);
    LOG_INFO("[APP] [MAG] [x, y, z]: %f, %f, %f", mag.x, mag.y, mag.z);
    LOG_INFO("[APP] [TEMP] [º]: %f", temp);

    auto uros_imu_msg = core::MicroRosMessageImu(accel, gyro, mag, temp);
    middleware.publish(uros_imu_msg);
}

void Imu::readImuWrapper(void* params)
{
    Imu* imu = static_cast<Imu*>(params);
    core::RtosTimer timer;
    for(;;) {   
        imu->readImu();
        timer.delay_ms(config::TaskConfig::imuTaskPeriodMs);
    }
}

}
