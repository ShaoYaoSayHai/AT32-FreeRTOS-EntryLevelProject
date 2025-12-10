#include "user_task.h"
#include "gt_uart.h"
#include "mobile_device.h"
#include "mobile_monitor.h"
#include "bt_device.h"
#include "mqtt_business_task.h"
#include "mbrtu.h"
#include "mqttsend.h"

#include "user_flash.h"
#include "led_colour.h"
#include "watchdog_task.h"
#include "button_task.h"

#include "ota_mcu.h"

MqttInfo mqttinfo;

TaskHandle_t uart2_task_handle;
TaskHandle_t ml307a_send_task_handle;
TaskHandle_t hpw6862i_task_handle;
TaskHandle_t hp203b_task_handle;
TaskHandle_t valva_control_task_handle;
TaskHandle_t main_process_task_handle;
TaskHandle_t xWatchdogTaskHandle;
TaskHandle_t mqtt_send_receive_task_handle;

// 产测模式看门狗任�?
TaskHandle_t xFactoryModeWatchDogTaskHandler = NULL;
// LED Task
TaskHandle_t xLedManagerTaskHandler = NULL;

// 读取产测模式代码
FACTORY_MODE_t pxGetFactoryModeCode(void);

SemaphoreHandle_t bt_change_valve_sem = NULL;	//????????????z???
SemaphoreHandle_t net_change_valve_sem = NULL;	//???????????z????
SemaphoreHandle_t bt_security_check_sem = NULL; //????????z???

SemaphoreHandle_t xOnem_sem = NULL;				   // h?????z???
SemaphoreHandle_t ValveControl_open_sem = NULL;	   //?????z???
SemaphoreHandle_t ValveControl_close_sem = NULL;   //????z???
SemaphoreHandle_t valve_action_success_sem = NULL; //??????????z???

SemaphoreHandle_t mqtt_send_sem = NULL;

SemaphoreHandle_t mqtt_type1_send_signal = NULL;
SemaphoreHandle_t mqtt_type2_send_signal = NULL;
SemaphoreHandle_t mqtt_type3_send_signal = NULL;

SemaphoreHandle_t valve_protect_Mute = NULL; //????????????

QueueHandle_t hpw6862i_queue = NULL; // LQCP005TA???????

EventGroupHandle_t xEventGroup;

// 创建一个FACTORY MODE �?
SemaphoreHandle_t xMutexFactoryMode = NULL;

float pressure_air, Temperature_air, Altitude_air;
volatile float air_p = 0.0f;
volatile bool netSetValveState = true;

_data_sensor_sendnet mqtt_net_send_data;
DeviceInfo Device_serial_number;

bool happen_warn = false;

extern i2c_handle_type hi2c1, hi2c2;

// char message_buffer[256]={0};
// char subtopic,pubtopic;
char pub_buffer[256];
WarningInfo warning;

int net_control_valve;
int net_control_delay;
int net_control_mode;

double net_set_overp;
double net_set_owep;

uint16_t ActionTimeCount;

float to_gt_pre1, to_gt_pre2, to_gt_pre3;

ota_param send_ota_param;

/**
 * @brief
 *
 * @param register_address
 * @param param
 * @param param_len
 * @param result
 * @param result_len
 * @return uint8_t
 */
uint8_t write_factory_th(uint16_t register_address, uint8_t *param,
						 uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	if (param_len != 17)
	{
		return OPERATION_FAILURE;
	}

	float over_p, owe_p, over_t, over_f;
	bool ai_control;

	memcpy(&over_p, &param[0], sizeof(float));
	memcpy(&owe_p, &param[4], sizeof(float));
	memcpy(&over_t, &param[8], sizeof(float));
	memcpy(&over_f, &param[12], sizeof(float));
	// 写入 0x01 就是常闭模式
	ai_control = (param[16] == 0x01);

	write_flash_userParameters(&over_p, &owe_p, &over_t, &over_f, &ai_control);

	*result_len = 0x00;
	result[0] = 0x00;
	ret = OPERATION_SUCCESS;
	return ret;
}

uint8_t factory_clean_err(uint16_t register_address, uint8_t *param,
						  uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	if (param_len != 1)
	{
		return OPERATION_FAILURE;
	}
	if (param[0] == 0x01)
	{
		// clear异常
		happen_warn = false;
	}
	else if (param[0] == 0x00)
	{
		/* code */
	}
	*result_len = 0x00;
	result[0] = 0x00;
	ret = OPERATION_SUCCESS;
	return ret;
}

uint8_t read_factory_th(uint16_t register_address, uint8_t *param,
						uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	*result_len = 19;
	spiflash_read_with_lock(result, 0x5000, 19);
	ret = OPERATION_SUCCESS;
	return ret;
}

// usTakeFactoryTick
uint8_t read_factory_tick(uint16_t register_address, uint8_t *param,
						  uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	*result_len = 2;
	uint16_t tick = usTakeFactoryTick();
	result[0] = (tick >> 8) & 0xFF;
	result[1] = (tick & 0xFF);
	ret = OPERATION_SUCCESS;
	return ret;
}

#define SW_MODE_SIZE 1
#define SW_MODE_ADDR 0x5016

/**
 * @brief 产测模式下设置常开或者常闭
 *
 * @param register_address
 * @param param
 * @param param_len
 * @param result
 * @param result_len
 * @return uint8_t
 */
uint8_t factory_set_sw_mode(uint16_t register_address, uint8_t *param,
							uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	if (param_len != SW_MODE_SIZE)
	{
		return OPERATION_FAILURE;
	}
	uint8_t writeValue[SW_MODE_SIZE] ;
	uint8_t flag = SW_MODE_WR_FLAG;
	// 1 开 0 闭
	if (param[0] == 0x01)
	{
		writeValue[0] = 0x01;
		// 设置为常开模式
	}
	else if (param[0] == 0x00)
	{
		// 设置为常闭模式
		writeValue[0] = 0x00;
	}
	spiflash_write_with_lock(&writeValue[0], SW_MODE_ADDR, SW_MODE_SIZE);			   // write value
	spiflash_write_with_lock(&flag, 0x5018, 1); // write flag
	*result_len = 0x00;
	result[0] = 0x00;
	ret = OPERATION_SUCCESS;
	return ret;
}

uint8_t factory_read_sw_mode(uint16_t register_address, uint8_t *param, uint8_t param_len, uint8_t *result, uint8_t *result_len)
{
	uint8_t ret = OPERATION_FAILURE;
	*result_len = SW_MODE_SIZE;
	uint8_t value = 0xFF;
	spiflash_read_with_lock(&value, SW_MODE_ADDR, SW_MODE_SIZE);
	result[0] = value;
	ret = OPERATION_SUCCESS;
	return ret;
}

union TransData
{
	uint8_t bytes[8];
	double f64Data;
} TransData;

void user_queue_create(void)
{

	hpw6862i_queue = xQueueCreate(32, sizeof(hpw6862i_data_format));
	if (hpw6862i_queue == NULL)
	{
		return;
	}

	ota_cmd_queue = xQueueCreate(5, sizeof(ota_param));
	if (ota_cmd_queue == NULL)
	{
		return;
	}
}

void user_sem_create(void)
{
	bt_change_valve_sem = xSemaphoreCreateBinary();
	if (bt_change_valve_sem == NULL)
	{
		return;
	}
	bt_security_check_sem = xSemaphoreCreateBinary();
	if (bt_security_check_sem == NULL)
	{
		return;
	}
	net_change_valve_sem = xSemaphoreCreateBinary();
	if (net_change_valve_sem == NULL)
	{
		return;
	}
	xOnem_sem = xSemaphoreCreateBinary();
	if (xOnem_sem == NULL)
	{
		return;
	}
	ValveControl_open_sem = xSemaphoreCreateBinary();
	if (ValveControl_open_sem == NULL)
	{
		return;
	}
	ValveControl_close_sem = xSemaphoreCreateBinary();
	if (ValveControl_close_sem == NULL)
	{
		return;
	}
	valve_action_success_sem = xSemaphoreCreateBinary();
	if (valve_action_success_sem == NULL)
	{
		return;
	}
	mqtt_send_sem = xSemaphoreCreateBinary();
	if (mqtt_send_sem == NULL)
	{
		return;
	}
	mqtt_type1_send_signal = xSemaphoreCreateBinary();
	mqtt_type2_send_signal = xSemaphoreCreateBinary();
	mqtt_type3_send_signal = xSemaphoreCreateBinary();
	if (mqtt_type1_send_signal == NULL || mqtt_type2_send_signal == NULL || mqtt_type3_send_signal == NULL)
	{
		return;
	}
}

void user_Mute_create(void)
{
	valve_protect_Mute = xSemaphoreCreateMutex();
	if (valve_protect_Mute == NULL)
	{
		return;
	}
	xMutexFactoryMode = xSemaphoreCreateMutex();
	xMutexMqttWriteBufferProtectHandler = xSemaphoreCreateMutex();
}

void user_eventgroup_create(void)
{
	xEventGroup = xEventGroupCreate();
	if (xEventGroup == NULL)
	{
		return;
	}
}

void user_task_create(void)
{
	bt_dev_create_task();
	mobile_dev_create_task(115200);
	mobile_monitor_create_task();
	xTaskCreate(mqtt_business_task,
				"MT_BS_TASK",
				1024,
				NULL,
				4,
				&mqtt_business_task_handle);
	modbus_rtu_create_task();

	xTaskCreate(
		led_task,
		"LED-TASK",
		128,
		NULL, 5,
		&xLedManagerTaskHandler);

	xTaskCreate(valva_control_task,
				"valva_control_TASK",
				128,
				NULL,
				7,
				&valva_control_task_handle); //???????????

	xTaskCreate(BMP6862I_Task,
				"bmp6862i_TASK",
				512,
				NULL,
				4,
				&hpw6862i_task_handle); // LQCP005TA????????

	xTaskCreate(hp203b_task,
				"hp203b_TASK",
				256,
				NULL,
				4,
				&hp203b_task_handle); // hp203b

#if 1
	xTaskCreate(main_process_task,
				"main_process_TASK",
				1024,
				NULL,
				4,
				&main_process_task_handle); //?????????????????
#endif
	xTaskCreate(watchdog_task,
				"watchdog_TASK",
				128, NULL,
				6,
				&xWatchdogTaskHandle); //???Z????????

	xTaskCreate(ota_task,
				"ota_TASK",
				512,
				NULL,
				3,
				&ota_task_handle);

	create_wdt_task();
	ButtonDevTaskCreate();

	vTaskSuspend(hpw6862i_task_handle);
	vTaskSuspend(main_process_task_handle);
	vTaskSuspend(xWatchdogTaskHandle);

	register_manager_init(6);
	register_manager_add(0x06, 0x5080, factory_clean_err);
	register_manager_add(0x06, 0x5082, write_factory_th);
	register_manager_add(0x03, 0x5083, read_factory_th);
	register_manager_add(0x03, 0x5081, read_factory_tick);
	//	// 设置常开常闭模式
	register_manager_add(0x06, 0x5084, factory_set_sw_mode);
	register_manager_add(0x03, 0x5085, factory_read_sw_mode);
}

void BMP6862I_Task(void *pvParameters)
{
	static volatile uint8_t p_cal_times = 0, one_houre = 0;
	hpw6862i_data_format get_value;

	static uint8_t err1, err2;
	BMP6862I_Data_t sensor_data1, sensor_data2;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(62.5);

	BMP6862I_Init(&hi2c1);
	BMP6862I_Init(&hi2c2);
	xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency);

		err1 = BMP6862I_ReadData(&sensor_data1, &hi2c1, sensor1);
		err2 = BMP6862I_ReadData(&sensor_data2, &hi2c2, sensor2);

		get_value.sensorf_p = sensor_data1.pressure;
		get_value.sensorf_t = sensor_data1.temperature;
		get_value.sensorb_p = sensor_data2.pressure;
		get_value.sensorb_t = sensor_data2.temperature;

		to_gt_pre1 = sensor_data1.pressure;
		to_gt_pre2 = sensor_data2.pressure;

		if (hpw6862i_queue == NULL)
		{
			led_show_red();
		}

		xQueueSend(hpw6862i_queue, &get_value, 0);

		if (err1 && err2)
		{
			p_cal_times++;
			if (p_cal_times >= 16)
			{
				one_houre++;
				p_cal_times = 0;
			}
			if (one_houre >= 60)
			{
				if (xSemaphoreGive(xOnem_sem) == pdTRUE)
				{
				}
				one_houre = 0;
			}
		}
		else
		{
		}
		xEventGroupSetBits(xEventGroup, TASK2_BIT);
		while (xGetFactoryModeCode() != IN_APP_MODE)
		{
			vTaskDelay(1000);
		}
	}
}

void hp203b_task(void *pvParameters)
{

	const TickType_t xDelayTime = pdMS_TO_TICKS(HPW203B_RCV_SIG_TIMEOUT_MS);

	HP203_relocation2();
	vTaskDelay(50);
	Hp203bReadPressureTemperature2(&pressure_air, &Temperature_air, &Altitude_air);

	air_p = pressure_air;
	to_gt_pre3 = pressure_air;
	while (1)
	{
		if (xSemaphoreTake(xOnem_sem, xDelayTime) == pdTRUE)
		{
			Hp203bReadPressureTemperature2(&pressure_air, &Temperature_air, &Altitude_air);
			if (((pressure_air - air_p) <= 300) && ((pressure_air - air_p) >= (-300)))
			{
				air_p = pressure_air;
				to_gt_pre3 = pressure_air;
			}
		}
		while (xGetFactoryModeCode() != IN_APP_MODE)
		{
			vTaskDelay(1000);
		}
	}
}

void valva_control_task(void *pvParameters)
{
	gpio_bits_reset(GPIOB, GPIO_PINS_12);
	gpio_bits_reset(GPIOB, GPIO_PINS_5);
	while (1)
	{
		vTaskDelay(80);
		if (xSemaphoreTake(ValveControl_open_sem, 0) == pdTRUE)
		{
			if (xSemaphoreTake(valve_protect_Mute, 100) == pdTRUE)
			{
				open_valve();
				vTaskDelay(50);
				xSemaphoreGive(valve_protect_Mute);
				if (!gpio_input_data_bit_read(GPIOB, GPIO_PINS_15))
				{
					xSemaphoreGive(valve_action_success_sem);
				}
				else
				{
					tmr_counter_enable(TMR4, TRUE);
					while (ActionTimeCount < 1500)
					{
						vTaskDelay(5);
						if (!gpio_input_data_bit_read(GPIOB, GPIO_PINS_15))
						{
							tmr_counter_enable(TMR4, FALSE);
							xSemaphoreGive(valve_action_success_sem);
							return;
						}
					}
					tmr_counter_enable(TMR4, FALSE);
					mqtt_send_warn(2, 0, 2, 1, 0, 0, 0, 0, 0, 0, "local", (150 + ActionTimeCount));
					ActionTimeCount = 0;
					/************* 阀门进入故障状 ***************/
					led_show_yellow();
					valve_state = false;

					// vTaskSuspend(main_process_task_handle);
					// vTaskSuspend(hpw6862i_task_handle);
					// vTaskSuspend(xTaskMultiButtonHandler);
					// vTaskSuspend(hp203b_task_handle);
					// vTaskSuspend(valva_control_task_handle);
					// while (1)
					// {
					// 	wdt_counter_reload();
					// 	vTaskDelay(500);
					// }
					/*********进入故障状�?***************/
				}
			}
		}
		else if (xSemaphoreTake(ValveControl_close_sem, 0) == pdTRUE)
		{
			if (xSemaphoreTake(valve_protect_Mute, 100) == pdTRUE)
			{
				close_valve();
				vTaskDelay(50);
				xSemaphoreGive(valve_protect_Mute);
				if (gpio_input_data_bit_read(GPIOB, GPIO_PINS_15))
				{

					xSemaphoreGive(valve_action_success_sem);
				}
				else
				{
					tmr_counter_enable(TMR4, TRUE);
					while (ActionTimeCount < 1500)
					{
						vTaskDelay(5);
						if (gpio_input_data_bit_read(GPIOB, GPIO_PINS_15))
						{
							tmr_counter_enable(TMR4, FALSE);
							xSemaphoreGive(valve_action_success_sem);
							return;
						}
					}
					tmr_counter_enable(TMR4, FALSE);
					mqtt_send_warn(2, 0, 2, 0, 0, 0, 0, 0, 0, 0, "local", (150 + ActionTimeCount));

					ActionTimeCount = 0;

					/*********  进入故障状 ***************/
					led_show_yellow();
					valve_state = false;

					// vTaskSuspend(main_process_task_handle);
					// vTaskSuspend(hpw6862i_task_handle);
					// vTaskSuspend(xTaskMultiButtonHandler);
					// vTaskSuspend(hp203b_task_handle);
					// vTaskSuspend(valva_control_task_handle);
					// while (1)
					// {
					// 	// 此处循环上报错误 并且记录已经上报的次数 // 多次之后重启设备，重启多次仍然失败报个大ERROR
					// 	wdt_counter_reload();
					// 	vTaskDelay(500);
					// }
					/*********进入故障状�?***************/
				}
			}
		}
	}
}

void main_process_task(void *pvParameters)
{
	uint8_t read_write_flage = 0x00;
#if 0
	spiflash_read_with_lock(&read_write_flage, 0x5017, 1);
#else
	uint8_t *pucReadBuffer = en_read_flash_wr_flag();
	read_write_flage = pucReadBuffer[0];
#endif

	if (read_write_flage == 0x01)
	{
		read_flash_userParameters(&ov_p_th, &ow_p_th, &ov_t_th, &ov_f_th);
	}
	if (pucReadBuffer[1] == 0x01)
	{
		read_flash_sw_mode(&AI_Flage);
	}

	gt_flash_read_calibration_param(&cal_param);

	maintask_open_valve(1);
	while (1)
	{
		processGoOn();
		//	xEventGroupSetBits(xEventGroup, TASK1_BIT);
	}
}
volatile uint8_t sensor_err_times = 0;

void watchdog_task(void *pvParameters)
{
	static volatile uint8_t error_flag = 0;
	static volatile uint8_t sensor_errtype = 0;
	EventBits_t uxBits;
	const TickType_t xDelayTime = pdMS_TO_TICKS(WATCHDOG_TIMEOUT_MS);
	while (1)
	{
		// wdt_counter_reload();
		vTaskDelay(xDelayTime);
		uxBits = xEventGroupWaitBits(
			xEventGroup,	//
			ALL_TASKS_BITS, //
			pdTRUE,			//
			pdTRUE,			//
			xDelayTime		//
		);
		if ((uxBits & ALL_TASKS_BITS) == ALL_TASKS_BITS) // 事件组上报成功
		{
			sensor_err_times = 0; // 传感器数据异常连续四次计数清零
			error_flag = 0;		  // 传感器数据异常计数清零
			wdt_counter_reload(); // 喂狗
		}
		else
		{
			if ((uxBits & TASK2_BIT) == 0)
			{
				sensor_errtype = 1;
			}
			else if ((uxBits & TASK1_BIT) == 0)
			{
				xQueueReset(hpw6862i_queue);
				sensor_errtype = 2;
			}
			error_flag++; // 传感器数据异常计数++
			if (error_flag >= 3)
			{
				mqtt_send_warn(3, 0, 0, 0, 0, sensor_errtype, 0, 0, 0, 0, 0, 0); // 上传传感器故障
				error_flag = 0;
				vTaskDelete(hpw6862i_task_handle); // 删除压力传感器任务
				hpw6862i_task_handle = NULL;	   // 重置句柄
				xQueueReset(hpw6862i_queue);
				xTaskCreate(BMP6862I_Task,
							"bmp6862i_TASK",
							512,
							NULL,
							5,
							&hpw6862i_task_handle); // 创建压力传感器任务

				sensor_err_times++; // 传感器数据异常连续四次计数++

				if (sensor_err_times >= 3) // 传感器数据异常连续四次连续三次
				{
					led_show_yellow_blink(); // 黄灯挂起任务喂狗等待维修或者ota
					vTaskSuspend(main_process_task_handle);
					valve_state = false;
					maintask_close_valve(1);
					vTaskSuspend(hpw6862i_task_handle);
					vTaskSuspend(xTaskMultiButtonHandler);
					vTaskSuspend(hp203b_task_handle);
					vTaskSuspend(valva_control_task_handle);
					while (1)
					{
						// wdt_counter_reload();
						vTaskDelay(500);
					}
				}
			}
			else
			{
			}
		}
	}
}

void process_mqtt_message(char *message_buffer, uint8_t len)
{

	if (json_extract_int(message_buffer, "operate", &net_control_valve))
	{
		if (json_extract_int(message_buffer, "delayMinute", &net_control_delay))
		{
			if (net_control_delay == 0)
			{
				if (net_control_valve != valve_state)
				{
					xSemaphoreGive(bt_change_valve_sem);
				}
			}
			else
			{
			}
		}
	}
	else if (json_extract_double(message_buffer, "overPress", &net_set_overp))
	{
		if (json_extract_double(message_buffer, "lowPress", &net_set_owep))
		{
			ov_p_th = net_set_overp;
			ow_p_th = net_set_owep;
			write_flash_userParameters(&ov_p_th, &ow_p_th, &ov_t_th, &ov_f_th, &AI_Flage);

			uint8_t write_th_flag = 0x01;
			spiflash_write_with_lock(&write_th_flag, 0x5017, 1);
		}
	}
	else if (json_extract_int(message_buffer, "SWMode", &net_control_mode))
	{
		if (net_control_mode == 0 || net_control_mode == 1)
		{
			AI_Flage = net_control_mode;
			write_flash_userParameters(&ov_p_th, &ow_p_th, &ov_t_th, &ov_f_th, &AI_Flage);

			uint8_t write_th_flag = 0x01;
			spiflash_write_with_lock(&write_th_flag, 0x5017, 1);
		}
	}
	else if (json_extract_int(message_buffer, "ota_type", &send_ota_param.ota_type))
	{
		bool get_value_ok = false;
		get_value_ok = json_extract_string(message_buffer, "host", send_ota_param.host, sizeof(send_ota_param.host));
		if (!get_value_ok)
		{
			return;
		}
		get_value_ok = json_extract_int(message_buffer, "port", &send_ota_param.port);
		//		if(!get_value_ok)
		//		{
		//			return;
		//		}
		get_value_ok = json_extract_string(message_buffer, "path", send_ota_param.path, sizeof(send_ota_param.path));
		if (!get_value_ok)
		{
			return;
		}
		get_value_ok = json_extract_string(message_buffer, "version", send_ota_param.version, sizeof(send_ota_param.version));
		if (!get_value_ok)
		{
			return;
		}
		get_value_ok = json_extract_int(message_buffer, "file_size", &send_ota_param.file_size);
		if (!get_value_ok)
		{
			return;
		}
		get_value_ok = json_extract_string(message_buffer, "md5", send_ota_param.md5, sizeof(send_ota_param.md5));
		if (!get_value_ok)
		{
			return;
		}
		xQueueSend(ota_cmd_queue, &send_ota_param, 0);
	}
}

float getLQCP005TAMeasurement(uint8_t sensor_id)
{

	if (sensor_id == 1)
	{
		return to_gt_pre1;
	}
	else if (sensor_id == 2)
	{
		return to_gt_pre2;
	}
	else if (sensor_id == 3)
	{
		return to_gt_pre2;
	}

	return -1;
}
