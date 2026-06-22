#include "rtos.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "pico/stdlib.h"

namespace core {

RtosTimer::RtosTimer() {
    lastWakeTime = xTaskGetTickCount();
}

void RtosTimer::delay_ms(const uint32_t delay_ms) {
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(delay_ms));
}

Rtos::Rtos() {
    tickStartTime = xTaskGetTickCount();
    LOG_INFO("[SERVICE] [RTOS] [START]");
}

void Rtos::createTask(const core::TaskDescription desc) {
    LOG_INFO("[SERVICE] [RTOS] [TASK CREATED]: %s", desc.task_name);
    if( xTaskCreate(
        desc.task,
        desc.task_name,
        desc.stack_size,
        desc.parameters,
        desc.priority,
        NULL
    ) != pdPASS) {
        LOG_ERROR("[ERROR] [SERVICE] [RTOS] [TASK CREATION FAILED]: %s", desc.task_name);
    }
    LOG_INFO("[SERVICE] [RTOS] [TASK CREATED]: %s [DONE]", desc.task_name);
}

void Rtos::startScheduler() {
    LOG_INFO("[SERVICE] [RTOS] [START SCHEDULER]");
    vTaskStartScheduler();
}

uint16_t Rtos::getElapsedTime() {
    return xTaskGetTickCount() - tickStartTime;
}

}