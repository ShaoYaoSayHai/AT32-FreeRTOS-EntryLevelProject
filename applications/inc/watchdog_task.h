
#ifndef _WATCHDOG_TASK_H_
#define _WATCHDOG_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

#define WATCHDOG_TIMEOUT_MS 500

void factory_mode_watchdog_task(void *pxParameter);

void instart_watchdog_task(void *pxParameter);

void create_wdt_task(void) ; 

TaskHandle_t *pxTakeStartWdtTaskHandler(void) ; 

uint16_t usTakeFactoryTick(void) ; 

#endif
