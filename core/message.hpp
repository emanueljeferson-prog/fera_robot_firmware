#pragma once 

#include <cstdint>
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
    UROS_SPEED,
    UROS_CMD,
    UROS_IMU,
    UROS_GPS,
    EXTERN_MESSAGE_SEND,
    EXTERN_MESSAGE_RECEIVE
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

struct Vector3D {
    double x;
    double y;
    double z;
    
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
    Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}
    
    Vector3D& operator=(const Vector3D& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
    
    // Conversão automática para geometry_msgs__msg__Point (definida em uros)
    operator struct geometry_msgs__msg__Point() const;
};   

struct TaskDescription {
    const char* task_name;
    uint16_t stack_size;
    uint32_t priority;
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

struct ReadImuMessage: public Message {
public:
    ReadImuMessage(Vector3D& accel, Vector3D& gyro, Vector3D& mag, double& temp)
    : accel(accel), gyro(gyro), mag(mag), temp(temp), Message(Topics::READ_IMU) {}
    Vector3D& accel;
    Vector3D& gyro;
    Vector3D& mag;
    double& temp; 
};

struct ReadSpeedMessage: public Message {
public:
    ReadSpeedMessage(const uint8_t id, double& speed)
    : id(id), speed(speed), Message(Topics::READ_SPEED) {}
    const uint8_t id;
    double& speed;
};

struct MotorCommandMessage: public Message {
public: 
    MotorCommandMessage(const uint8_t id, const int16_t signal)
    : id(id), signal(signal), Message(Topics::MOTOR_COMMAND) {}
    const uint8_t id;
    const int16_t signal;
};   

struct MicroRosMessageSpeed: public Message {
public:
    MicroRosMessageSpeed(const double speed_1, const double speed_2)
    : speed_1(speed_1), speed_2(speed_2), Message(Topics::UROS_SPEED) {}
    const double speed_1;
    const double speed_2;
};

struct MicroRosMessageImu: public Message {
public:
    MicroRosMessageImu(const Vector3D accel, const Vector3D gyro, const Vector3D mag, const double temp)
    : accel(std::move(accel)), gyro(std::move(gyro)), mag(std::move(mag)), temp(temp), Message(Topics::UROS_IMU) {}
    const Vector3D accel;
    const Vector3D gyro;
    const Vector3D mag;
    const double temp; 
};

struct MicroRosMessageGps: public Message {
public:
    MicroRosMessageGps(const double latitude, const double longitude, const double altitude, const uint32_t seconds, const uint32_t nanoseconds)
    : latitude(latitude), longitude(longitude), altitude(altitude), seconds(seconds), nanoseconds(nanoseconds), Message(Topics::UROS_GPS) {}
    const double latitude;
    const double longitude;
    const double altitude;
    const uint32_t seconds;
    const uint32_t nanoseconds;
};

struct ExternMessageSend: public Message {
public: 
    ExternMessageSend(const std::vector<uint8_t> payload, const size_t lenght, const ProtocolType protocol, const uint8_t address) 
    : payload(std::move(payload)), lenght(lenght), protocol(protocol), address(address), Message(Topics::EXTERN_MESSAGE_SEND){}
    const std::vector<uint8_t> payload;
    const size_t lenght;
    const ProtocolType protocol;
    const uint8_t address; 
};

struct ExternMessageReceive: public Message {
public: 
    ExternMessageReceive(std::vector<uint8_t>& payload, const size_t lenght, const ProtocolType protocol, const uint8_t address) 
    : payload(payload), lenght(lenght), protocol(protocol), address(address), Message(Topics::EXTERN_MESSAGE_RECEIVE){}
    std::vector<uint8_t>& payload;
    const size_t lenght;
    const ProtocolType protocol;
    const uint8_t address;
};

}
