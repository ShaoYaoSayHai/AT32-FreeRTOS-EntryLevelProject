
#include "watchdog_task.h"
#include "wk_wdt.h"

volatile uint16_t factory_tick = 0;
TaskHandle_t xTaskWDT_StartHandler = NULL;

extern TaskHandle_t xFactoryModeWatchDogTaskHandler;

void factory_mode_watchdog_task(void *pxParameter)
{
    const TickType_t xDelayTime = pdMS_TO_TICKS(WATCHDOG_TIMEOUT_MS);
    vTaskSuspend( xFactoryModeWatchDogTaskHandler );
    while (1)
    {
        wdt_counter_reload();
        vTaskDelay(xDelayTime);
        factory_tick++;
    }
}

void instart_watchdog_task(void *pxParameter)
{
    static volatile uint8_t error_flag = 0;
    const TickType_t xDelayTime = pdMS_TO_TICKS(WATCHDOG_TIMEOUT_MS);
    while (1)
    {
        wdt_counter_reload();
        vTaskDelay(xDelayTime);
    }
}

void create_wdt_task(void)
{
    xTaskCreate(
        factory_mode_watchdog_task,
        "FACTORY MODE WATCHDOG TASK",
        128,
        NULL, 6,
        &xFactoryModeWatchDogTaskHandler);

    xTaskCreate(
        instart_watchdog_task,
        "IN START TASK",
        128,
        NULL,
        6,
        &xTaskWDT_StartHandler);
}

TaskHandle_t *pxTakeStartWdtTaskHandler(void)
{
    return &xTaskWDT_StartHandler;
}

uint16_t usTakeFactoryTick(void)
{
    uint16_t currentTick = factory_tick;
    return currentTick;
}