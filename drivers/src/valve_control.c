#include "valve_control.h"

void open_valve(void)
{
	gpio_bits_reset(GPIOB, GPIO_PINS_5);
	gpio_bits_set(GPIOB, GPIO_PINS_12);
	vTaskDelay(100);
	gpio_bits_reset(GPIOB, GPIO_PINS_12);
	led_show_green();
}

void close_valve(void)
{
	gpio_bits_reset(GPIOB, GPIO_PINS_12);
	gpio_bits_set(GPIOB, GPIO_PINS_5);
	vTaskDelay(100);
	gpio_bits_reset(GPIOB, GPIO_PINS_5);
	led_show_red();
}