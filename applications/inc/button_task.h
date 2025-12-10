

#ifndef _BUTTON_TASK_H_
#define _BUTTON_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "wk_gpio.h"

extern TaskHandle_t xTaskMultiButtonHandler;

#define KEY1_PORT GPIOB
#define KEY1_PIN GPIO_PINS_2

#define BUTTON_ENABLE 0 // 按下有效

typedef enum
{
    MultiButtonId_1 = 1,
    MultiButtonId_2 = 2,
    MultiButtonId_3,
    MultiButtonId_4,
} MultiButtonId_t;

/**
 * @brief 按键任务
 *
 * @param pvParameter
 */
void multibutton_task(void *pvParameter) ; 

void ButtonDevTaskCreate(void) ; 

#endif
