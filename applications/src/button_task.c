/**
 * @file button_task.c
 * @author Enzo
 * @brief  button 任务 基于MultiButton
 * @version 0.1
 * @date 2025-12-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "button_task.h"
#include "multi_button.h"
#include "led_colour.h"

#include "user_task.h"
// 按钮句柄
static Button xMultiButton1;

TaskHandle_t xTaskMultiButtonHandler;

/***************双击修改*****************/

static bool waiting_for_double_click = false;
static TickType_t click_timeout = 0;

void btn1_single_click_handler(Button *btn)
{
    //    // 如果正在等待双击，则取消单击处理
    //    if (waiting_for_double_click) {
    //        return;
    //    }
    //
    //    if (xSemaphoreGive(bt_change_valve_sem) == pdTRUE) {
    //        // 成功发送信号量
    //    }
    waiting_for_double_click = true;
    click_timeout = xTaskGetTickCount();
}

void btn1_double_click_handler(Button *btn)
{
    // 双击发生时，清除等待标志
    waiting_for_double_click = false;

    if (xSemaphoreGive(bt_security_check_sem) == pdTRUE)
    {
        // 成功发送信号量
    }
}

// 在 button_ticks 的调用循环中检测超时
void check_click_timeout(void)
{
    if (waiting_for_double_click)
    {
        if ((xTaskGetTickCount() - click_timeout) > pdMS_TO_TICKS(SHORT_TICKS * TICKS_INTERVAL))
        {
            // 双击超时，真正触发单击
            waiting_for_double_click = false;
            if (xSemaphoreGive(bt_change_valve_sem) == pdTRUE)
            {
                // 成功发送信号量
            }
        }
    }
}

// 修改第一次单击的处理
void btn1_first_click_handler(Button *btn)
{
    // 第一次单击时设置等待标志
    waiting_for_double_click = true;
    click_timeout = xTaskGetTickCount();
}

// void buttons_init(void)
//{
//     button_init(&xMultiButton1, KeyReadPin, 0, MultiButtonId_1);
//     button_attach(&xMultiButton1, BTN_SINGLE_CLICK, btn1_first_click_handler); // 第一次单击
//     button_attach(&xMultiButton1, BTN_DOUBLE_CLICK, btn1_double_click_handler);
//     button_attach(&xMultiButton1, BTN_LONG_PRESS_START, btn1_long_click_handler);
//     button_start(&xMultiButton1);
// }

/***************双击修改*****************/

/**
 * @brief 读取按键值
 *
 * @param ucBtnId 按键ID
 * @return uint8_t  根据按键ID排列
 */
static uint8_t KeyReadPin(uint8_t ucBtnId)
{
    return (gpio_input_data_bit_read(KEY1_PORT, KEY1_PIN));
}

/**
 * @brief 初始化 GPIO INPUT 引脚
 *
 */
void btn_gpio_init(void)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = KEY1_PIN;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(KEY1_PORT, &gpio_init_struct);
}

/***************双击修改*****************/
// void btn1_single_click_handler(Button *btn)
//{
//  //   led_show_green();
//
//			if (xSemaphoreGive(bt_change_valve_sem) == pdTRUE)
//		{
//
//		}
// }

// void btn1_double_click_handler(Button *btn)
//{
//  //   led_show_blue();
//			if (xSemaphoreGive(bt_security_check_sem) == pdTRUE)
//		{
//
//		}
// }
/***************双击修改*****************/

void btn1_long_click_handler(Button *btn)
{
    //    led_show_white();
    waiting_for_double_click = false;

    xSemaphoreTake(bt_change_valve_sem, 0);
    xSemaphoreTake(bt_security_check_sem, 0);
    happen_warn = false;
}

/**
 * @brief 按键初始化
 *
 */

/***************双击修改*****************/
void buttons_init(void)
{
    button_init(&xMultiButton1, KeyReadPin, 0, MultiButtonId_1);
    button_attach(&xMultiButton1, BTN_SINGLE_CLICK, btn1_first_click_handler); // 第一次单击
    button_attach(&xMultiButton1, BTN_DOUBLE_CLICK, btn1_double_click_handler);
    button_attach(&xMultiButton1, BTN_LONG_PRESS_START, btn1_long_click_handler);
    button_start(&xMultiButton1);
}
/***************双击修改*****************/
/**
 * @brief 按键任务
 *
 * @param pvParameter
 */
/***************双击修改*****************/
// void multibutton_task(void *pvParameter)
//{
//     // 初始化
//     btn_gpio_init();
//     buttons_init();

//    TickType_t xLastWakeTime;
//    const TickType_t xFrequency = pdMS_TO_TICKS(5);
//    xLastWakeTime = xTaskGetTickCount();

//    while (1)
//    {
//        button_ticks();
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
//    }
//}

void multibutton_task(void *pvParameter)
{
    btn_gpio_init();
    buttons_init();

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(5);
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        button_ticks();
        check_click_timeout(); // 添加超时检测
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/***************双击修改*****************/
void ButtonDevTaskCreate(void)
{
    xTaskCreate(
        multibutton_task,
        "Button-task",
        256, NULL, 5,
        &xTaskMultiButtonHandler);
}
