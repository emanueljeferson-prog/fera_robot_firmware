#include "rtos.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <iostream>

namespace service {
Rtos::Rtos(core::IMiddleware& middleware) 
: middleware(middleware) {
    std::cout << "[SERVICE] [RTOS] [START]" << std::endl;
}

void Rtos::init() {
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::REGISTER_TASK)) {
                const auto& register_msg = static_cast<const core::RegisterTask&>(msg);
                this->createTask(register_msg.desc);
            }
        }, 
        core::Topics::REGISTER_TASK,
        true
    );
    middleware.subscribe(
        [this](const core::Message& msg) {
            if(msg.compareTopic(core::Topics::DEALY_TASK)) {
                const auto& delay_msg = static_cast<const core::DelayTask&>(msg);
                this->delayTask(delay_msg.delay);
            }
        }, 
        core::Topics::DEALY_TASK,
        false
    );
}

void Rtos::createTask(const core::TaskDescription desc) {
    std::cout << "[SERVICE] [RTOS] [TASK CREATED]: " << desc.task_name << std::endl;
    xTaskCreate(
        desc.task,
        desc.task_name.c_str(),
        desc.stack_size,
        desc.parameters,
        desc.priority,
        NULL
    );
}

void Rtos::startScheduler() {
    vTaskStartScheduler();
}

void Rtos::delayTask(const uint32_t delay) {
    vTaskDelay(pdMS_TO_TICKS(delay));
}

}