/*
 * FreeRTOS configuration for Raspberry Pi Pico 2 (RP2350)
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* RP2350 runs at 150 MHz by default */
#define configCPU_CLOCK_HZ                          150000000

/* Scheduling */
#define configTICK_RATE_HZ                          1000
#define configUSE_PREEMPTION                        1
#define configUSE_TIME_SLICING                      1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION      0
#define configUSE_TICKLESS_IDLE                      0
#define configMAX_PRIORITIES                         5
#define configMINIMAL_STACK_SIZE                     256
#define configMAX_TASK_NAME_LEN                      16
#define configTICK_TYPE_WIDTH_IN_BITS                TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                      1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES         1
#define configQUEUE_REGISTRY_SIZE                    0
#define configENABLE_BACKWARD_COMPATIBILITY          0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS       0
#define configUSE_MINI_LIST_ITEM                      1
#define configSTACK_DEPTH_TYPE                        size_t
#define configMESSAGE_BUFFER_LENGTH_TYPE              size_t
#define configUSE_NEWLIB_REENTRANT                    0

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION              1
#define configSUPPORT_DYNAMIC_ALLOCATION             1
#define configTOTAL_HEAP_SIZE                         (16 * 1024)
#define configAPPLICATION_ALLOCATED_HEAP              0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP     0
#define configKERNEL_PROVIDED_STATIC_MEMORY           1

/* Software timers */
#define configUSE_TIMERS                             1
#define configTIMER_TASK_PRIORITY                     (configMAX_PRIORITIES - 1)
#define configTIMER_TASK_STACK_DEPTH                  configMINIMAL_STACK_SIZE
#define configTIMER_QUEUE_LENGTH                      10

/* Event groups and stream buffers */
#define configUSE_EVENT_GROUPS                        1
#define configUSE_STREAM_BUFFERS                      1

/* Hook functions */
#define configUSE_IDLE_HOOK                          0
#define configUSE_TICK_HOOK                          0
#define configUSE_MALLOC_FAILED_HOOK                 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK           0
#define configCHECK_FOR_STACK_OVERFLOW               2

/* Debug */
#define configASSERT(x)                              \
    if ((x) == 0) {                                  \
        portDISABLE_INTERRUPTS();                    \
        for (;;);                                    \
    }

/* Stats */
#define configGENERATE_RUN_TIME_STATS                0
#define configUSE_TRACE_FACILITY                     0
#define configUSE_STATS_FORMATTING_FUNCTIONS         0

/* Co-routines (deprecated, leave off) */
#define configUSE_CO_ROUTINES                        0
#define configMAX_CO_ROUTINE_PRIORITIES              1

/* SMP - single core for now */
#define configNUMBER_OF_CORES                        1
#define configRUN_MULTIPLE_PRIORITIES                0
#define configUSE_CORE_AFFINITY                      0
#define configUSE_PASSIVE_IDLE_HOOK                  0

/* INCLUDE functions */
#define INCLUDE_vTaskPrioritySet                     1
#define INCLUDE_uxTaskPriorityGet                    1
#define INCLUDE_vTaskDelete                          1
#define INCLUDE_vTaskSuspend                         1
#define INCLUDE_vTaskDelayUntil                      1
#define INCLUDE_vTaskDelay                           1
#define INCLUDE_xTaskGetSchedulerState               1
#define INCLUDE_xTaskGetCurrentTaskHandle            1
#define INCLUDE_uxTaskGetStackHighWaterMark          1
#define INCLUDE_xTimerPendFunctionCall               1

/* Cortex-M33 interrupt priority configuration.
 * RP2350 implements 4 priority bits (0x00-0xF0 in steps of 0x10).
 * Lowest numeric value = highest urgency. */
#define configKERNEL_INTERRUPT_PRIORITY              0
#define configMAX_SYSCALL_INTERRUPT_PRIORITY          16  /* 0x10 - priority 1 */
#define configMAX_API_CALL_INTERRUPT_PRIORITY         configMAX_SYSCALL_INTERRUPT_PRIORITY

/* RP2350 Cortex-M33 specific - no TrustZone, no MPU */
#define configENABLE_TRUSTZONE                       0
#define configRUN_FREERTOS_SECURE_ONLY               1 /* Run FreeRTOS in secure mode else will cause a silent hard fault*/
#define configENABLE_MPU                             0
#define configENABLE_FPU                             1
#define configENABLE_MVE                             0

#endif /* FREERTOS_CONFIG_H */
