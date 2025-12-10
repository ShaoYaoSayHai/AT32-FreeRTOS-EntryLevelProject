#ifndef _LED_COLOUR_H
#define _LED_COLOUR_H

#include "user_source.h"

void led_show_green(void);
void led_show_red(void);
void led_show_blue(void);
void led_show_yellow(void);
void led_disable_yellow(void);
void led_show_null(void);
void led_show_white(void);
void led_red_show_blink(void);
void led_show_white_blink(void);
// void led_show_purple(void);
void led_show_purple_blink(void);
void led_show_yellow_blink(void);
void led_show_green_blink(void);
void led_show_bluegreen_blink(void) ; 
//

#define LED_RED (GPIO_PINS_14)
#define LED_GREEN (GPIO_PINS_13)
#define LED_BLUE (GPIO_PINS_3)

#define LED_YELLOW (LED_GREEN | LED_RED)
#define LED_PURPLE (LED_RED | LED_BLUE)
#define LED_GRAY (LED_GREEN | LED_BLUE)
#define LED_WHITE (LED_GREEN | LED_RED | LED_BLUE)

typedef enum
{
    LED_OFF,
    LED_ON,
    LED_BLINK,
} ActionList;

typedef struct led_action_t
{
    uint16_t ledx;
    ActionList action;
} led_action_t;

typedef struct led_int_show
{
    uint16_t flag;
    void (*on)(uint16_t us_ledx);
    void (*off)(uint16_t us_ledx);
    void (*blink)(uint16_t us_ledx, uint16_t interval);
} led_int_show;

extern TaskHandle_t led_task_handle;

void led_task(void *pvParameters);

void led_on(uint16_t us_led_pin);

void led_off(uint16_t us_led_pin);

void led_show_purple(void);

/**
 * @brief LED ��˸
 *
 * @param us_led_pin
 * @param interval   30 -- 300ms
 */
void led_blink(uint16_t us_led_pin, uint16_t interval);

#endif
