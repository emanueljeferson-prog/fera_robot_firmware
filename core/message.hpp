#pragma once 

#include <cstdint>
#include <string>
#include <functional>

namespace core {

struct Message;

using Callback = std::function<void(const Message&)>;
using TaskFunction = void (*)(void*);

enum Topics {
    MOTOR_CONTROL,
    READ_SPEED,
    READ_IMU,
    READ_GPS,
    REGISTER_TASK,
    DEALY_TASK
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

struct ReadSpeedMessage: public Message {
public:
    ReadSpeedMessage(const uint8_t id, double& speed, const Topics tp)
    : id(id), speed(speed), Message(tp) {}
    const uint8_t id;
    double& speed;
};

struct ControlMotorMessage: public Message {
public: 
    ControlMotorMessage(const uint8_t id, const int16_t signal, const Topics tp)
    : id(id), signal(signal), Message(tp) {}
    const uint8_t id;
    const int16_t signal;
};   

}



