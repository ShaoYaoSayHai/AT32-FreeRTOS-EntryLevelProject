
#include "drv_led.h"

void drv_led_blue(int status)
{
    if (status)
    {
        gpio_bits_set(LED_PORT, LED_BLUE_PIN);
    }
    else
    {
        gpio_bits_reset(LED_PORT, LED_BLUE_PIN);
    }
}

void drv_led_green(int status)
{
    if (status)
    {
        gpio_bits_set(LED_PORT, LED_GREEN_PIN);
    }
    else
    {
        gpio_bits_reset(LED_PORT, LED_GREEN_PIN);
    }
}

void drv_led_red(int status)
{
    if (status)
    {
        gpio_bits_set(LED_PORT, LED_RED_PIN);
    }
    else
    {
        gpio_bits_reset(LED_PORT, LED_RED_PIN);
    }
}