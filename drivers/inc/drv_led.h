/**
 * @file drv_led.h
 * @author Enzo
 * @brief  LEDÇý¶¯
 * @version 0.1
 * @date 2025-11-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef _DRV_LED_H_
#define _DRV_LED_H_

#include "wk_gpio.h"

#define LED_PORT (GPIOB)
#define LED_RED_PIN (GPIO_PINS_14)
#define LED_GREEN_PIN (GPIO_PINS_13)
#define LED_BLUE_PIN (GPIO_PINS_3)

void drv_led_blue(int status);

void drv_led_green(int status);

void drv_led_red(int status);

#endif
