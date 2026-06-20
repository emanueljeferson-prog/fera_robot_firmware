#pragma once 

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace core {

struct Message;

using Callback = std::function<void(const Message&)>;
using TaskFunction = void (*)(void*);

enum Topics {
    MOTOR_COMMAND,
    READ_SPEED,
    READ_IMU,
    READ_GPS,
    REGISTER_TASK,
    DELAY_TASK,
    UROS_SPEED,
    UROS_CMD,
    UROS_IMU,
    UROS_GPS,
    EXTERN_MESSAGE_SEND
};

enum ProtocolType {
    UART,
    SPI,
    I2C,
    USB,
    CAN,
    FlexRay,
    LIN,
    WIFI,
    ETHERNET
};

struct TaskDescription {
    std::string task_name;
    uint16_t stack_size;
    uint8_t priority;
    TaskFunction task;
    void* parameters;
};

struct Message {
public:
    Message(const Topics tp): topic(tp) {}
    virtual ~Message() = default;
    bool compareTopic(const Topics other) const {
        return topic == other;
    }
private:
    const Topics topic;
};

struct RegisterTask: public Message {
public:
    RegisterTask(const TaskDescription& desc, const Topics tp)
    : desc(desc), Message(tp) {}
    const TaskDescription desc;
};

struct DelayTask: public Message {
public:
    DelayTask(const uint32_t delay, const Topics tp)
    : delay(delay), Message(tp) {}
    const uint32_t delay;
};

struct ReadImuMessage: public Message {
public:
    ReadImuMessage(std::vector<double>& accel, std::vector<double>& gyro, std::vector<double>& mag, std::vector<double>& temp, const Topics tp)
    : accel(accel), gyro(gyro), mag(mag), temp(temp), Message(tp) {}
    std::vector<double>& accel;
    std::vector<double>& gyro;
    std::vector<double>& mag;
    std::vector<double>& temp; 
};

struct ReadSpeedMessage: public Message {
public:
    ReadSpeedMessage(const uint8_t id, double& speed, const Topics tp)
    : id(id), speed(speed), Message(tp) {}
    const uint8_t id;
    double& speed;
};

struct MotorCommandMessage: public Message {
public: 
    MotorCommandMessage(const uint8_t id, const int16_t signal, const Topics tp)
    : id(id), signal(signal), Message(tp) {}
    const uint8_t id;
    const int16_t signal;
};   

struct MicroRosMessageSpeed: public Message {
public:
    MicroRosMessageSpeed(const double speed_1, const double speed_2, const Topics tp)
    : speed_1(speed_1), speed_2(speed_2), Message(tp) {}
    const double speed_1;
    const double speed_2;
};

struct MicroRosMessageImu: public Message {
public:
    MicroRosMessageImu(std::vector<double> accel, std::vector<double> gyro, std::vector<double> mag, const double temp, const Topics tp)
    : accel(std::move(accel)), gyro(std::move(gyro)), mag(std::move(mag)), temp(temp), Message(tp) {}
    std::vector<double> accel;
    std::vector<double> gyro;
    std::vector<double> mag;
    const double temp; 
};

struct MicroRosMessageGps: public Message {
public:
    MicroRosMessageGps(const double latitude, const double longitude, const double altitude, const uint32_t seconds, const uint32_t nanoseconds, const Topics tp)
    : latitude(latitude), longitude(longitude), altitude(altitude), seconds(seconds), nanoseconds(nanoseconds), Message(tp) {}
    const double latitude;
    const double longitude;
    const double altitude;
    const uint32_t seconds;
    const uint32_t nanoseconds;
};

struct ExternMessageSend: public Message {
public: 
    ExternMessageSend(const std::vector<uint8_t> payload, const size_t lenght, const ProtocolType protocol, const uint8_t address, const Topics tp) 
    : payload(std::move(payload)), lenght(lenght), protocol(protocol), address(address), Message(tp){}
    const std::vector<uint8_t> payload;
    const size_t lenght;
    const ProtocolType protocol;
    const uint8_t address; 
};

struct ExternMessageReceive: public Message {
public: 
    ExternMessageReceive(std::vector<uint8_t>& payload, const size_t lenght, const ProtocolType protocol, const uint8_t address, const Topics tp) 
    : payload(payload), lenght(lenght), protocol(protocol), address(address), Message(tp){}
    std::vector<uint8_t>& payload;
    const size_t lenght;
    const ProtocolType protocol;
    const uint8_t address;
};

}
