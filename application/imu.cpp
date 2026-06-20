#include "imu.hpp"

namespace app {

Imu::Imu(core::IMiddleware& middleware)
: middleware(middleware) {
    logger::info("[APP] [IMU] [START]");
}

void Imu::init() {
    auto register_task_imu_msg = core::RegisterTask(
        core::TaskDescription{
            .task_name = "imu",
            .stack_size = 1024,
            .priority = 2,
            .task = &Imu::readImuWrapper,
            .parameters = this
        },
        core::Topics::REGISTER_TASK
    );
    
    middleware.publish(register_task_imu_msg);
    logger::info("[APP] [IMU] [INIT]");
}

void Imu::readImu() {
    std::vector<double> _accel(3);
    std::vector<double> _gyro(3);
    std::vector<double> _mag(3);
    std::vector<double> _temp(1);

    auto get_imu_data_msg = core::ReadImuMessage(_accel, _gyro, _mag, _temp, core::Topics::READ_IMU);
    middleware.publish(get_imu_data_msg);

    accel.x = _accel[0];
    accel.y = _accel[1];
    accel.z = _accel[2];
    gyro.x = _gyro[0];
    gyro.y = _gyro[1];
    gyro.z = _gyro[2];
    mag.x = _mag[0];
    mag.y = _mag[1];
    mag.z = _mag[2];
    temp = _temp[0];

    logger::info("[APP] [ACCEL] [x, y, z]: " + std::to_string(accel.x) + ", " + std::to_string(accel.y) + ", " + std::to_string(accel.z));
    logger::info("[APP] [GYRO] [x, y, z]: " + std::to_string(gyro.x) + ", " + std::to_string(gyro.y) + ", " + std::to_string(gyro.z));
    logger::info("[APP] [MAG] [x, y, z]: " + std::to_string(mag.x) + ", " + std::to_string(mag.y) + ", " + std::to_string(mag.z));
    logger::info("[APP] [TEMP] [º]: " + std::to_string(temp));
}

void Imu::readImuWrapper(void* params) {
    Imu* imu = static_cast<Imu*>(params);
    for(;;) {
        imu->readImu();
        /*auto delay_msg = core::DelayTask(1000, core::Topics::DELAY_TASK);
        imu->middleware.publish(delay_msg);*/
        sleep_ms(1000);
    }
}

}
