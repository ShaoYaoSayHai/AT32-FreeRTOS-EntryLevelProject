#include "led_colour.h"

/// LED ������
TaskHandle_t led_task_handle;

// LED ��ʾ״̬����
QueueHandle_t xQueueLedShowList = NULL;

// LED ��˸����
volatile uint16_t us_time_tick = 0;
volatile uint8_t us_blink_flag = FALSE; // Ĭ���ǲ�������˸��

// ��ȡLED ����ָ��
TaskHandle_t *pxTakeLedTaskHandler(void)
{
	return &led_task_handle;
}

int led_task_init(void)
{
	xQueueLedShowList = xQueueCreate(3, sizeof(led_action_t));
	return 0;
}

void led_set(uint16_t us_led_pin, uint16_t us_led_status)
{
	if (us_led_status)
	{
		gpio_bits_set(GPIOB, us_led_pin);
	}
	else
	{
		gpio_bits_reset(GPIOB, us_led_pin);
	}
}

void led_on(uint16_t us_led_pin)
{
	led_set(us_led_pin, TRUE);
}

void led_off(uint16_t us_led_pin)
{
	led_set(us_led_pin, FALSE);
}

/**
 * @brief LED ��˸
 *
 * @param us_led_pin
 * @param interval   30 -- 300ms
 */
void led_blink(uint16_t us_led_pin, uint16_t interval)
{
	if (!us_blink_flag)
	{
		return;
	}
	us_time_tick++;
	if ((us_time_tick / interval) == 1)
	{
		gpio_bits_set(GPIOB, us_led_pin);
	}
	else if ((us_time_tick / interval) == 2)
	{
		us_time_tick = 0;
		gpio_bits_reset(GPIOB, us_led_pin);
	}
}

void led_task(void *pvParameters)
{
	led_action_t ul_receive_flag;
	led_action_t ul_old_receive_info;
	uint16_t us_target_led = 0;
	us_time_tick = 0;
	us_blink_flag = FALSE;
	led_task_init();
	vTaskDelay(1000);

	while (1)
	{
		if (xQueueReceive(xQueueLedShowList, &ul_receive_flag, 10) != pdFALSE)
		{
			if ((ul_old_receive_info.ledx != ul_receive_flag.ledx) || (ul_old_receive_info.action != ul_receive_flag.action))
			{
				// ��ȡLED����ɹ�
				us_time_tick = 0;
				us_blink_flag = FALSE;
				led_set(LED_WHITE, FALSE);
				if (ul_receive_flag.action == LED_ON)
				{
					led_on(ul_receive_flag.ledx);
				}
				else if (ul_receive_flag.action == LED_OFF)
				{
					led_off(ul_receive_flag.ledx);
					us_time_tick = 0;
				}
				else if (ul_receive_flag.action == LED_BLINK)
				{
					us_target_led = ul_receive_flag.ledx;
					us_blink_flag = TRUE;
				}
				else
				{
					led_off(LED_WHITE);
					us_time_tick = 0;
				}
			}
			ul_old_receive_info.ledx = ul_receive_flag.ledx;
			ul_old_receive_info.action = ul_receive_flag.action;
		}
		else
		{
		}
		// ִ����˸����
		led_blink(us_target_led, 15);
		vTaskDelay(10);
	}
}

void led_show_yellow(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_YELLOW,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_yellow_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_YELLOW,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_disable_yellow(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_YELLOW,
		.action = LED_OFF};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_green(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_GREEN,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_green_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_GREEN,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_red(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_RED,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_blue(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_BLUE,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_null(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_WHITE,
		.action = LED_OFF};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_white(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_WHITE,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_red_show_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_RED,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_white_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_WHITE,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_purple(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_PURPLE,
		.action = LED_ON};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}

void led_show_purple_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_PURPLE,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}


void led_show_bluegreen_blink(void)
{
	led_action_t ul_receive_flag = {
		.ledx = LED_GRAY,
		.action = LED_BLINK};
	xQueueSend(xQueueLedShowList, &ul_receive_flag, 100);
}