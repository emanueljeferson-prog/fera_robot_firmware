#include "core/middleware.hpp"
#include "application/motor.hpp"
#include "application/imu.hpp"
#include "service/encoder_drive.hpp"
#include "service/motor_drive.hpp"
#include "service/uros.hpp"
#include "service/comm.hpp"
#include "service/imu_drive.hpp"
#include "logger/logger.hpp"
#include "config.hpp"

int main() {
    stdio_init_all();
    sleep_ms(5000);
    auto broker = core::Middleware(); 
    auto encoder_drive = service::EncoderDrive(broker);
    auto imu_drive = service::ImuDrive(broker);
    auto motor_drive = service::MotorDrive(broker);
    auto uros = service::MicroRos(broker);
    auto motor = app::Motor(broker);
    auto imu = app::Imu(broker);
    // services 
    encoder_drive.init();
    motor_drive.init();
    imu_drive.init();
    uros.init();
    // applications
    motor.init();
    imu.init();
    // scheduler
    broker.run();
    return 0;
}