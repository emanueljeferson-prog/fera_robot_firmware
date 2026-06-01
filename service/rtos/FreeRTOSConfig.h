#pragma once

/* =========================
   Kernel basics
========================= */
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configCPU_CLOCK_HZ                      ((unsigned long)100000000)

/* Tick type (OBRIGATÓRIO no seu erro) */
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS

/* =========================
   Task system
========================= */
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                1024

#define configUSE_IDLE_HOOK                    0
#define configUSE_TICK_HOOK                    0

/* =========================
   Memory
========================= */
#define configSUPPORT_DYNAMIC_ALLOCATION       1
#define configSUPPORT_STATIC_ALLOCATION        0
#define configTOTAL_HEAP_SIZE                  (128 * 1024)

/* =========================
   Timers (você ativou isso!)
========================= */
#define configUSE_TIMERS                       1
#define configTIMER_TASK_PRIORITY              2
#define configTIMER_QUEUE_LENGTH              10
#define configTIMER_TASK_STACK_DEPTH          2048

/* =========================
   Runtime options
========================= */
#define INCLUDE_vTaskDelay                     1
#define INCLUDE_vTaskDelayUntil               1