#include "rtos.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <iostream>

namespace service {
RtosService::RtosService(core::IMiddleware& middleware) 
: middleware(middleware) {
    std::cout << "[SERVICE] [RTOS] [START]" << std::endl;
}

void RtosService::init() {
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
        true
    );
}

void RtosService::createTask(const core::TaskDescription desc) {
    // Task creation logic here
    std::cout << "[SERVICE] [RTOS] [TASK CREATED]: " << desc.task_name << std::endl;
    //desc.task(desc.parameters); // For demonstration, we directly call the task function
    xTaskCreate(
        desc.task,
        desc.task_name.c_str(),
        desc.stack_size,
        desc.parameters,
        desc.priority,
        NULL
    );
}

void RtosService::startScheduler() {
    // Scheduler start logic here
    vTaskStartScheduler();
}

void RtosService::delayTask(const uint32_t delay) {
    // Task delay logic here
    vTaskDelay(pdMS_TO_TICKS(delay));
}
}