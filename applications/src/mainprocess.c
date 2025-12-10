#include "mainprocess.h"

#include "gt_flash.h"
#include "mobile_monitor.h"
#include "mqttsend.h"
#include "led_colour.h"

// 阈值
float ov_p_th = 8000.0f;
float ow_p_th = 800.0f;
float ov_t_th = 65.0f;
float ov_f_th = 3.0f;

// 阀前阀后管压
volatile float gauge_f;
volatile float gauge_b;
// 接收管道压力结构体
hpw6862i_data_format receive_value_6862i;
// 用于熄火关阀，恒流关阀
float last512[512] = {0};
volatile uint16_t last512_index = 0;
volatile uint8_t NoUseGas_flage = 0;
volatile uint16_t three_hours_flag = 0;

// 运行状态机
system_state_t current_state = STATE_WAIT_STABLE;

// 用于检查传感器数据是否一直未改变
float CheckValue_f[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
float CheckValue_b[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
volatile uint8_t CheckValue_index = 0;

// 用于检漏

volatile float checkleak_five_f[5];
volatile float checkleak_five_b[5];

// 用于检漏计数
volatile uint16_t checkleak_index = 0;

// 常开、常闭
bool AI_Flage = false;

// 阀门状态
volatile bool valve_state = true;

// 过流检测
float flow_samples_f[16];
float flow_samples_b[16];

volatile uint8_t samples_count = 0;

// 当前流量
volatile float real_flow = 0.0f;

// 最近四次流量的储存数组
float flow_array[4];
volatile uint8_t flow_index;

// 压力温度异常标志位
volatile bool data_warn_flag = false;

// 发生异常计数
volatile uint8_t data_warn_index;

// 压力温度异常计数

volatile uint8_t over_p_count;
volatile uint8_t owe_p_count;
volatile uint8_t over_t_count;
// 点火开阀
volatile DropDetector_t fire_open_alve = {.write_index = 0, .buffer_full = false, .buffer = {0}};

// 关阀检漏
volatile bool found_ledk = false;

// 关阀一直检漏缓存
volatile uint8_t over_leack_samples[150] = {0};
// 关阀一直检漏缓存标志
volatile uint8_t over_leack_index = 0;

// 实时标准差
volatile float standard_deviation;
// 检漏值
float avg_leak_e, avg_leak_l;
float avg_check_e, avg_check_l;

// 过流计数
uint8_t ov_flow_cal;

// 解除异常通气、补气计数
uint8_t cal_time_forwait;
uint8_t cal_time_foraddgas;

// 解除异常补气标志位

volatile bool add_gas_flag = false;

// 比较大小
static int compare_floats(const void *a, const void *b)
{
	float fa = *(const float *)a;
	float fb = *(const float *)b;
	return (fa > fb) - (fa < fb);
}

//

void processGoOn(void)
{

	// 在这里进行压力温度数据接收并且对数据进行处理，控制发送开关阀，推送报警信息
	if (xQueueReceive(hpw6862i_queue, &receive_value_6862i, 500) == pdPASS)
	{

		gauge_f = receive_value_6862i.sensorf_p - air_p + cal_param.precal1;
		gauge_b = receive_value_6862i.sensorb_p - air_p + cal_param.precal2;

		CheckPressureValue(gauge_f, gauge_b);
		switch (current_state)
		{
		case STATE_WAIT_STABLE:
			ModuleInit(160);
			break;
		case STATE_LEAK_CHECK:
			CheckLeak();
			break;
		case STATE_SECURITY_CHECK:
			check_security();
			break;
		case STATE_NORMAL_OPERATION:
			Normal_operation();
			break;
		case STATE_BLOCKED:
			back_warn();
			break;
		case STATE_ADD_GAS:
			Add_gas();
			break;
		}
	}
	else
	{
		// 发送超时警报
	}
}

/**
 * @brief  用于计算标准差
 * @param  data:输入数组指针 n:数组长度
 * @retval 标准差
 */

float calculateStandardDeviation(float *data, int n)
{
	float sum = 0.0f;
	for (int i = 0; i < n; i++)
	{
		sum += data[i];
	}
	float mean = sum / n;
	float sum2 = 0.0f;
	float sum4 = 0.0f;
	for (int i = 0; i < n; i++)
	{
		float diff = data[i] - mean;
		float diff2 = diff * diff;
		sum2 += diff2;
	}
	float variance = sum2 / n;
	if (variance < 1e-9f)
	{
		return 0.0f;
	}
	float StandardDeviation = sqrt(variance);
	return StandardDeviation;
}

/**
 * @brief  由于将数组线性拟合
 * @param  data:输入数组指针 n:数组长度
 * @retval none
 */

void calculate_Fitting(float *data, int n)
{
	float sumxi = 0.0;
	float sumyi = 0.0;
	float sumxiyi = 0.0;
	float sumxi2 = 0;
	for (int i = 0; i < n; i++)
	{
		sumxi += i;
		sumyi += data[i];
		sumxiyi += i * data[i];
		sumxi2 += i * i;
	}

	float a = ((sumxi2 * sumyi) - (sumxi * sumxiyi)) / (n * sumxi2 - sumxi * sumxi);
	float b = (n * sumxiyi - sumxi * sumyi) / (n * sumxi2 - sumxi * sumxi);

	for (int i = 0; i < n; i++)
	{
		data[i] = data[i] - (b * i + a);
	}
}

/**
 * @brief  用于自动关闭阀门
 * @param  input当前管道内阀后压力数值
 * @retval none
 */

void AutomaticValveShutdown(float input)
{
	last512[last512_index++] = input;
	if (last512_index >= 512)
	{
		// ¼ÆËãÏßÐÔÄâºÏ
		calculate_Fitting(last512, 512);
		// ¼ÆËã±ê×¼²î
		standard_deviation = calculateStandardDeviation(last512, 512);
		if (standard_deviation < 1.5) // ±ê×¼²îÐ¡ÓÚ1.5
		{
			NoUseGas_flage++;
			// 32 * 56 = 1800 s ×óÓÒ
			if (NoUseGas_flage >= 56)
			//			if (NoUseGas_flage >= 10)
			{
				maintask_close_valve(1);
				// ÖØÖÃ¹¦ÄÜ¿ØÖÆÐÅºÅÁ¿
				xSemaphoreTake(bt_change_valve_sem, 0);
				xSemaphoreTake(net_change_valve_sem, 0);
				xSemaphoreTake(bt_security_check_sem, 0);
				NoUseGas_flage = 0;
			}
			three_hours_flag = 0;
		}
		/**
		 * Ö»ÒªÓÃ»ð±ê×¼²î¾Í´óÓÚ1.5
		 * ²»ÓÃ»ðµÄÊ±ºòÊÇ0.4-0.6
		 * ÓÃ»ð×´Ì¬Ê±6.x-7.x
		 */
		else
		{
			three_hours_flag++;
			if (three_hours_flag >= 337)
			//			if (three_hours_flag >= 5)
			{
				happen_warn = true;
				maintask_close_valve(1);
				mqtt_send_warn(1, 6, 0, 0, 0, 0, gauge_b, 0, receive_value_6862i.sensorb_t, real_flow, "null", 0);
				current_state = STATE_BLOCKED;
				three_hours_flag = 0;
			}
			NoUseGas_flage = 0;
		}
		last512_index = 0;
	}
}

/**
 * @brief  用于验证传感器数据是否一直未改变
 * @param  input:最新的压力值
 * @retval none
 */
void CheckPressureValue(float input_f, float input_b)
{
	CheckValue_f[CheckValue_index] = input_f;
	CheckValue_b[CheckValue_index] = input_b;
	CheckValue_index = (CheckValue_index + 1) % 12;

	if ((calculateStandardDeviation(CheckValue_f, 12) == 0) | (calculateStandardDeviation(CheckValue_b, 12) == 0))
	{
	}
	else
	{
		xEventGroupSetBits(xEventGroup, TASK1_BIT);
	}
}

/**
 * @brief  用于等待模块初始化完成
 * @param  number:等待的次数
 * @retval none
 */

void ModuleInit(uint16_t number)
{
	static uint16_t cal_number;
	if (cal_number < number)
	{
		cal_number++;
	}
	else
	{
		maintask_close_valve(1);
		vTaskDelay(200);
		led_show_blue();
		// ½øÈë¼ìÂ©
		current_state = STATE_LEAK_CHECK;
	}
}

/**
 * @brief  用于检漏
 * @param  none
 * @retval none
 */

void CheckLeak(void)
{

	if (checkleak_index < 5)
	{
		checkleak_five_f[checkleak_index] = gauge_b;
		checkleak_index++;
	}
	else if ((5 <= checkleak_index) && (checkleak_index <= 154))
	{
		if (checkleak_index == 5)
		{
			avg_leak_e = (checkleak_five_f[0] + checkleak_five_f[1] + checkleak_five_f[2] + checkleak_five_f[3] + checkleak_five_f[4]) / 5.0f;
		}
		checkleak_index++;
	}
	else if ((154 < checkleak_index) && (checkleak_index < 160))
	{
		checkleak_five_b[checkleak_index - 155] = gauge_b;
		if (checkleak_index == 159)
		{
			avg_leak_l = (checkleak_five_b[0] + checkleak_five_b[1] + checkleak_five_b[2] + checkleak_five_b[3] + checkleak_five_b[4]) / 5.0f;
			if (((avg_leak_e - avg_leak_l) / avg_leak_e) > 0.12f)
			{
				happen_warn = true;
				mqtt_send_warn(1, LEAK, 0, 0, 0, 0, gauge_b, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", 0);
				// 报警泄漏
				current_state = STATE_BLOCKED;
			}
			else
			{
				led_show_red();
				if (AI_Flage)
				{
					maintask_open_valve(1);
				}

				xSemaphoreTake(bt_change_valve_sem, 0);
				xSemaphoreTake(bt_security_check_sem, 0);

				current_state = STATE_NORMAL_OPERATION;
			}
		}
		checkleak_index++;
	}
}

/**
 * @brief  用于正常工作状态
 * @param  none
 * @retval none
 */
void Normal_operation(void)
{
	cal_time_foraddgas = 0;
	cal_time_forwait = 0;
	checkleak_index = 0;

	check_control_singal();

	check_required_items();
	if (!AI_Flage)
	{
		Ai_control();
	}
}
/**
 * @brief  超温、超压、欠压、过流检测
 * @param  none
 * @retval none
 */
void check_required_items(void)
{
	if (valve_state)
	{
		flow_samples_f[samples_count] = gauge_f;
		flow_samples_b[samples_count] = gauge_b;
		samples_count++;
		if (samples_count >= 16)
		{
			float avg_flow_f, avg_flow_b;
			process_flow_samples(flow_samples_f, 16, &avg_flow_f);
			process_flow_samples(flow_samples_b, 16, &avg_flow_b);
			double dif_p = avg_flow_f - avg_flow_b;

			if (dif_p <= 0)
			{
				real_flow = 0.0;
			}
			else if (dif_p < 23.0)
			{
				real_flow = sqrt(dif_p) * 0.108;
			}
			else if (dif_p >= 23.0)
			{
				real_flow = sqrt(dif_p) * 0.115;
			}
			flow_array[flow_index] = real_flow;

			flow_index = (flow_index + 1) % 4;

			ov_flow_cal = 0;
			for (int k = 0; k < 4; k++)
			{
				if (flow_array[k] > ov_f_th)
				{
					ov_flow_cal++;
				}
			}
			if (ov_flow_cal >= 3)
			{
				happen_warn = true;
				maintask_close_valve(1);
				vTaskDelay(1000);
				xQueueReset(hpw6862i_queue);
				mqtt_send_warn(1, 2, 0, 0, 0, 0, gauge_b, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", real_flow);
				current_state = STATE_BLOCKED;
			}
			samples_count = 0;
		}
	}
	else
	{

		if (xSemaphoreTake(bt_security_check_sem, 0) == pdTRUE)
		{
			maintask_open_valve(1);
			vTaskDelay(200);
			led_show_blue();
			xQueueReset(hpw6862i_queue);

			// 清空报警计数
			over_p_count = 0;
			owe_p_count = 0;
			over_t_count = 0;
			data_warn_flag = 0;
			data_warn_index = 0;
			for (int i = 0; i < 30; i++)
			{
				fire_open_alve.buffer[i] = 0;
			}
			fire_open_alve.write_index = 0;
			fire_open_alve.buffer_full = false;

			samples_count = 0;
			flow_index = 0;
			for (int i = 0; i < 4; i++)
			{
				flow_array[i] = 0;
			}

			current_state = STATE_SECURITY_CHECK;
		}

		samples_count = 0;
		flow_index = 0;
		for (int i = 0; i < 4; i++)
		{
			flow_array[i] = 0;
		}
	}

	static float now_err_pre, now_err_t;

	if (data_warn_flag)
	{
		data_warn_index++;
	}

	if (gauge_f > ov_p_th)
	{
		now_err_pre = gauge_f;
		data_warn_flag = 1;
		over_p_count++;
	}
	if (gauge_f < ow_p_th)
	{
		now_err_pre = gauge_f;
		data_warn_flag = 1;
		owe_p_count++;
	}
	if ((receive_value_6862i.sensorb_t > ov_t_th) || (receive_value_6862i.sensorf_t > ov_t_th))
	{
		if (receive_value_6862i.sensorb_t > receive_value_6862i.sensorf_t)
		{
			now_err_t = receive_value_6862i.sensorb_t;
		}
		else
		{
			now_err_t = receive_value_6862i.sensorf_t;
		}
		data_warn_flag = 1;
		over_t_count++;
	}

	if (data_warn_index >= 32)
	{
		if ((float)over_p_count / 32.0f > 0.8f)
		{
			happen_warn = true;
			maintask_close_valve(1);
			vTaskDelay(1000);
			xQueueReset(hpw6862i_queue);
			mqtt_send_warn(1, 3, 0, 0, 0, 0, now_err_pre, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", real_flow);
			current_state = STATE_BLOCKED;
		}
		else if ((float)owe_p_count / 32.0f > 0.8f)
		{
			happen_warn = true;
			maintask_close_valve(1);
			vTaskDelay(1000);
			xQueueReset(hpw6862i_queue);
			mqtt_send_warn(1, 4, 0, 0, 0, 0, now_err_pre, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", real_flow);
			current_state = STATE_BLOCKED;
		}
		else if ((float)over_t_count / 32.0f > 0.8f)
		{
			happen_warn = true;
			maintask_close_valve(1);
			vTaskDelay(1000);
			xQueueReset(hpw6862i_queue);
			mqtt_send_warn(1, 5, 0, 0, 0, 0, gauge_b, 0.0f, now_err_t, 0.0f, "null", real_flow);
			current_state = STATE_BLOCKED;
		}
		over_p_count = 0;
		owe_p_count = 0;
		over_t_count = 0;
		data_warn_flag = 0;
		data_warn_index = 0;
	}
}

/**
 * @brief  去掉四个最大值四个最小值取平均
 * @param  none
 * @retval none
 */

void process_flow_samples(float *samples, uint8_t count, float *result)
{
	if (count != 16 || samples == NULL || result == NULL)
	{
		*result = 0.0f;
		return;
	}

	float temp_samples[16];
	for (int j = 0; j < 16; j++)
	{
		temp_samples[j] = samples[j];
	}
	qsort(temp_samples, 16, sizeof(float), compare_floats);

	float sum = 0.0f;
	for (uint8_t i = 4; i < 12; i++)
	{
		sum += temp_samples[i];
	}

	*result = sum / 8.0f;
}
/**
 * @brief  点火开阀，熄火关阀
 * @param  none
 * @retval none
 */
void Ai_control(void)
{
	if (valve_state)
	{
		AutomaticValveShutdown(receive_value_6862i.sensorb_p);
	}
	else
	{
		static volatile bool clear_buf = false;
		if (fire_open_alve.buffer_full)
		{
			// 30个数据
			float oldest = fire_open_alve.buffer[fire_open_alve.write_index];
			// 改成kx+b的阈值判定，应对可能出现的高楼层压力问题。
			// 需要k暂定0.25 b暂定200Pa，达成原本700Pa效果，
			// 假设压力平时处于1500Pa,则对应的参数
			// 0.25* 1500 + 200 = 575Pa，降低了点火开阀的阈值
			if ((oldest > 1000) && (gauge_b < 500) && ((oldest - gauge_b) > 500)) // 楼层高度引起气压变化
			{
				maintask_open_valve(1);
				clear_buf = true;
			}
			else if (((oldest - gauge_b) > (0.024 * oldest)))
			{
				found_ledk = true;
			}
			else if ((gauge_f > 1000) && (oldest < 1000.0))
			{
				maintask_open_valve(1);
				current_state = STATE_ADD_GAS;
			}

			if (found_ledk)
			{
				over_leack_samples[over_leack_index] = 1;
				uint8_t leak_num = 0;
				for (int l = 0; l < 150; l++)
				{
					leak_num = leak_num + over_leack_samples[l];
				}
				if (leak_num > 135)
				{
					for (int o = 0; o < 150; o++)
					{
						over_leack_samples[o] = 0;
					}

					happen_warn = true;

					/********************清空缓存数组*******************/
					for (int i = 0; i < 30; i++)
					{
						fire_open_alve.buffer[i] = 0;
					}
					fire_open_alve.write_index = 0;
					fire_open_alve.buffer_full = false;
					/********************清空缓存数组*******************/
					mqtt_send_warn(1, 1, 0, 0, 0, 0, gauge_b, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", 0);
					current_state = STATE_BLOCKED;
				}

				found_ledk = false;
			}
			else
			{
				over_leack_samples[over_leack_index] = 0;
			}

			over_leack_index = (over_leack_index + 1) % 150;
		}

		fire_open_alve.buffer[fire_open_alve.write_index] = gauge_b;
		fire_open_alve.write_index = (fire_open_alve.write_index + 1) % 30;
		if (!fire_open_alve.buffer_full && fire_open_alve.write_index == 0)
		{
			fire_open_alve.buffer_full = true;
		}

		if (clear_buf)
		{
			for (int i = 0; i < 30; i++)
			{
				fire_open_alve.buffer[i] = 0;
			}
			fire_open_alve.write_index = 0;
			fire_open_alve.buffer_full = false;
			clear_buf = false;
		}
	}
}
/**
 * @brief  一键安检
 * @param  none
 * @retval none
 */
void check_security(void)
{

	if (checkleak_index < 5)
	{
		checkleak_five_f[checkleak_index] = gauge_b;
		checkleak_index++;
	}
	else if ((5 <= checkleak_index) && (checkleak_index <= 1920))
	{
		if (checkleak_index == 5)
		{
			avg_check_e = (checkleak_five_f[0] + checkleak_five_f[1] + checkleak_five_f[2] + checkleak_five_f[3] + checkleak_five_f[4]) / 5.0f;
		}
		checkleak_index++;
	}
	else if ((1920 < checkleak_index) && (checkleak_index < 1926))
	{
		checkleak_five_b[checkleak_index - 1921] = gauge_b;
		if (checkleak_index == 1925)
		{
			avg_check_l = (checkleak_five_b[0] + checkleak_five_b[1] + checkleak_five_b[2] + checkleak_five_b[3] + checkleak_five_b[4]) / 5.0f;
			if ((avg_check_e - avg_check_l) > 150.0f)
			{
				happen_warn = true;
				mqtt_send_warn(1, 1, 0, 0, 0, 0, gauge_b, 0.0f, receive_value_6862i.sensorb_t, 0.0f, "null", 0);
				current_state = STATE_BLOCKED;
			}
			else
			{
				if (AI_Flage)
				{
					led_show_green();
				}
				else
				{
					maintask_close_valve(1);
				}
				xSemaphoreTake(bt_change_valve_sem, 0);
				xSemaphoreTake(bt_security_check_sem, 0);
				current_state = STATE_NORMAL_OPERATION;
			}
		}
		checkleak_index++;
	}
}

/**
 * @brief  报警模式
 * @param  none
 * @retval none
 */
void back_warn(void)
{
	checkleak_index = 0;
	if (!add_gas_flag)
	{
		led_red_show_blink();
		check_valve_state();
	}

	if (!happen_warn)
	{
		if (cal_time_forwait == 0)
		{
			add_gas_flag = true;
			maintask_open_valve(1);
		}

		if (cal_time_forwait <= 32)
		{
			if (cal_time_forwait == 32)
			{
				add_gas_flag = false;
				maintask_close_valve(1);
				xQueueReset(hpw6862i_queue);

				// 清空报警计数
				over_p_count = 0;
				owe_p_count = 0;
				over_t_count = 0;
				data_warn_flag = 0;
				data_warn_index = 0;
				for (int i = 0; i < 30; i++)
				{
					fire_open_alve.buffer[i] = 0;
				}
				fire_open_alve.write_index = 0;
				fire_open_alve.buffer_full = false;

				samples_count = 0;
				flow_index = 0;
				for (int i = 0; i < 4; i++)
				{
					flow_array[i] = 0;
				}

				xSemaphoreTake(bt_change_valve_sem, 0);
				xSemaphoreTake(bt_security_check_sem, 0);
				current_state = STATE_NORMAL_OPERATION;
			}
			cal_time_forwait++;
		}
	}
}

/**
 * @brief  手柄被拉起立即关阀
 * @param  none
 * @retval none
 */
void check_valve_state(void)
{
	if ((!gpio_input_data_bit_read(GPIOB, GPIO_PINS_15)))
	{
		vTaskDelay(50);
		if ((!gpio_input_data_bit_read(GPIOB, GPIO_PINS_15)))
		{
			maintask_close_valve(1);
		}
	}
}



void maintask_open_valve(uint8_t source)
{
	if (xSemaphoreGive(ValveControl_open_sem) == pdTRUE)
	{
	}
	//
	if (xSemaphoreTake(valve_action_success_sem, pdMS_TO_TICKS(1000)) != pdFALSE)
	{
		// 收到了关阀信号
	}
	else
	{
		// 没有收到关阀信号
	}

	xQueueReset(hpw6862i_queue);

	for (int o = 0; o < 150; o++)
	{
		over_leack_samples[o] = 0;
	}

	valve_state = true;

	if (source == 1)
	{
		mqtt_send_warn(2, 0, 1, 1, 0, 0, 0, 0, 0, 0, "local", (150 + ActionTimeCount));
		ActionTimeCount = 0;
	}
	else if (source == 0)
	{
		mqtt_send_warn(2, 0, 1, 1, 0, 0, 0, 0, 0, 0, "user", (150 + ActionTimeCount));
		ActionTimeCount = 0;
	}
}
void maintask_close_valve(uint8_t source)
{
	if (xSemaphoreGive(ValveControl_close_sem) == pdTRUE)
	{
	}
	//  xSemaphoreTake(valve_action_success_sem, portMAX_DELAY);
	if (xSemaphoreTake(valve_action_success_sem, pdMS_TO_TICKS(1000)) != pdFALSE)
	{
		// 收到了关阀信号
	}
	else
	{
		// 没有收到关阀信号
	}
	xQueueReset(hpw6862i_queue);
	// 重置熄火关阀
	last512_index = 0;
	NoUseGas_flage = 0;
	three_hours_flag = 0;

	valve_state = false;

	if (source == 1)
	{
		mqtt_send_warn(2, 0, 0, 0, 0, 0, 0, 0, 0, 0, "local", (150 + ActionTimeCount));
		ActionTimeCount = 0;
	}
	else if (source == 0)
	{
		mqtt_send_warn(2, 0, 0, 0, 0, 0, 0, 0, 0, 0, "user", (150 + ActionTimeCount));
		ActionTimeCount = 0;
	}
}

void Add_gas(void)
{
	if (cal_time_foraddgas <= 32)
	{
		if (cal_time_foraddgas == 32)
		{
			for (int i = 0; i < 30; i++)
			{
				fire_open_alve.buffer[i] = 0;
			}
			fire_open_alve.write_index = 0;
			fire_open_alve.buffer_full = false;

			maintask_close_valve(1);

			xSemaphoreTake(bt_change_valve_sem, 0);
			xSemaphoreTake(bt_security_check_sem, 0);
			current_state = STATE_NORMAL_OPERATION;
		}
		cal_time_foraddgas++;
	}
}

void check_control_singal()
{
	if (xSemaphoreTake(bt_change_valve_sem, 0) == pdTRUE)
	{
		if (valve_state)
		{
			if (!AI_Flage)
			{
				happen_warn = true;
				maintask_close_valve(0);
				current_state = STATE_BLOCKED;
			}
			else
			{
				maintask_close_valve(0);
			}
		}
		else
		{
			maintask_open_valve(0);
		}
	}
}
