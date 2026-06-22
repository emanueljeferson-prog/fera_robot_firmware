#pragma once 

#include "message.hpp"
#include "logger/logger.hpp"

namespace core {

class RtosTimer {
public:
    RtosTimer();
    void delay_ms(const uint32_t delay_ms);
private:
    uint32_t lastWakeTime;
};

class Rtos {
public:
    Rtos();
    void createTask(const TaskDescription desc);
    void startScheduler();
    uint16_t getElapsedTime();
private:
    uint16_t tickStartTime;
};

}