#pragma once 

#include <cstdint>
#include <string>
#include <functional>

namespace core {

struct Message;

using Callback = std::function<void(const Message&)>;
using TaskFunction = std::function<void()>;

enum Topics {
    MOTOR_CONTROL,
    READ_SPEED,
    READ_IMU,
    READ_GPS,
    REGISTER_TASK
};

struct TaskDescription {
    std::string task_name;
    uint16_t stack_size;
    uint8_t priority;
    TaskFunction task;
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
    RegisterTask(TaskDescription desc, const Topics tp): Message(tp) {}
};



}





