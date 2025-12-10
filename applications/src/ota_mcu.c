#include "ota_mcu.h"
#include "gt_flash.h"
#include "flash_partition.h"
#include "http_client.h"
#include "boot.h"
#include "version_info.h"
#include "mainprocess.h"
#include "led_colour.h"
#include "factory_mode.h"

ota_param OTA_PARAM;

TaskHandle_t ota_task_handle;
TaskHandle_t OTA_TaskTimeoutReviewHandler = NULL;
QueueHandle_t ota_cmd_queue = NULL;

SemaphoreHandle_t xSmphrOTATimeoutSignal = NULL; // 超时等待信号量

void OTA_timeout_task(void *pvParameter);

void vResetTimeoutTick(void);

int32_t http_out_rcv_cb(uint32_t package_index, uint8_t *data, uint16_t len)
{
	return gt_flash_write_fw_with_cache(package_index, data, len);
}

unsigned char md5_bytes[16] = {0};

OTA_FW_INFO info = {0};

void ota_task(void *pvParameters)
{
	while (1)
	{
		if (xQueueReceive(ota_cmd_queue, &OTA_PARAM, 1000))
		{
			xSmphrOTATimeoutSignal = xSemaphoreCreateBinary();
			if (xSmphrOTATimeoutSignal == NULL)
			{
				return;
			}
			user_enter_factory_mode(1);

			xTaskCreate(
				OTA_timeout_task,
				"TIMEOUT-TASK",
				128, NULL,
				4,
				&OTA_TaskTimeoutReviewHandler);

			if (OTA_TaskTimeoutReviewHandler == NULL)
			{
				// 创建任务失败，复位
				user_enter_factory_mode(0);
				vTaskDelay(1000);
				nvic_system_reset();
			}

			// led_show_green_blink();
			if (OTA_PARAM.ota_type == 1)
			{
				info.fwType = FW_TYPE_MCU;
			}
			info.fwSize = OTA_PARAM.file_size;

			strncpy(info.fwVersion, OTA_PARAM.version, (sizeof(info.fwVersion) - 1));
			info.fwVersion[sizeof(info.fwVersion) - 1] = '\0';

			if (!hex_string_to_bytes(OTA_PARAM.md5, md5_bytes, sizeof(md5_bytes)))
			{
				return;
			}

			if (convert_https_to_http(OTA_PARAM.host, sizeof(OTA_PARAM.host)))
			{
				OTA_PARAM.port = 80;
			}

			memcpy(info.fwMd5, md5_bytes, sizeof(md5_bytes));
			//  写入OTA信息到Flash
			gt_flash_write_ota_fw_info(&info);
			//  擦除下载区
			spiflash_sector_erase_with_lock(DOWNLOAD_ADDR, DOWNLOAD_SIZE);
			// 下载固件文件
			int32_t res = http_client_get_file("oss.oakocq.com", OTA_PARAM.port, OTA_PARAM.path,
											   OTA_PARAM.file_size, http_out_rcv_cb);

			if (res != 0)
			{
				return;
			}
			// 校验md5
			res = gt_flash_check_fw_md5();
			if (res != 0)
			{
				return;
			}
			led_show_red();
			// 备份当前固件到备份区
			res = backupApp();
			if (res != 0)
			{
				return;
			}
			// 设置OTA标志位
			setOTA(1);
			nvic_system_reset(); // 重启设备，进入bootloader更新固件
		}

		vTaskDelay(100);
	}
}

char hex_byte[3];
// 将字符串形式的MD5转换为字节数组
bool hex_string_to_bytes(const char *hex_str, unsigned char *byte_array, size_t byte_len)
{
	if (!hex_str || !byte_array || strlen(hex_str) != byte_len * 2)
	{
		return false;
	}

	for (size_t i = 0; i < byte_len; i++)
	{
		//        hex_byte[3] = {hex_str[i * 2], hex_str[i * 2 + 1], '\0'};
		hex_byte[0] = hex_str[i * 2];
		hex_byte[1] = hex_str[i * 2 + 1];
		hex_byte[2] = 0;
		uint8_t temp_hex = (unsigned char)strtoul(hex_byte, NULL, 16);
		byte_array[i] = temp_hex;
	}
	return true;
}

bool string_to_port(const char *port_str, uint16_t *rcport)
{
	if (!port_str || !rcport)
	{
		return false;
	}

	// 检查字符串是否为空或过长
	size_t len = strlen(port_str);
	if (len == 0 || len > 5)
	{ // 端口号最大5位（65535）
		return false;
	}

	// 检查所有字符是否为数字
	for (size_t i = 0; i < len; i++)
	{
		if (!isdigit((unsigned char)port_str[i]))
		{
			return false;
		}
	}

	// 转换为长整型并检查范围
	char *endptr;
	long port_long = strtol(port_str, &endptr, 10);

	// 检查转换是否成功
	if (*endptr != '\0')
	{
		return false;
	}

	// 检查是否溢出
	if (port_long == LONG_MAX || port_long == LONG_MIN)
	{
		return false;
	}

	// 检查端口范围是否有效
	if (port_long < 1 || port_long > 65535)
	{
		return false;
	}

	*rcport = (uint16_t)port_long;
	return true;
}

void check_ota_success(void)
{
	if (isOTA())
	{
		OTA_FW_INFO check_info = {0};
		gt_flash_read_ota_fw_info(&check_info);
		if (!strcmp(check_info.fwVersion, VERSION_INFO))
		{
		}
		else
		{
		}
		setOTA(0);
	}
}

bool convert_https_to_http(char *url, size_t url_size)
{
	if (!url || url_size == 0)
	{
		return 0;
	}

	const char *https_prefix = "https://";
	const char *http_prefix = "http://";
	size_t https_len = strlen(https_prefix);

	// 检查是否以https://开头
	if (strncmp(url, https_prefix, https_len) == 0)
	{

		// 计算剩余部分的长度
		size_t remaining_len = strlen(url + https_len);

		// 确保转换后不会溢出缓冲区
		if (remaining_len + strlen(http_prefix) < url_size)
		{
			// 创建临时缓冲区存储剩余部分
			char temp[256] = {0}; // 或使用动态分配
			strncpy(temp, url + https_len, sizeof(temp) - 1);

			// 重新组合为http://开头
			strcpy(url, http_prefix);
			strcat(url, temp);
			return 1;
		}
		else
		{
		}
	}
	return 0;
}

void OTA_timeout_task(void *pvParameter)
{
	uint16_t usTimeoutCnt = 0;
	while (1)
	{
		if (xSemaphoreTake(xSmphrOTATimeoutSignal, pdMS_TO_TICKS(1000)) != pdFALSE)
		{
			usTimeoutCnt = 0;
		}
		else
		{
			usTimeoutCnt++;
			if (usTimeoutCnt >= 130)
			{
				usTimeoutCnt = 0;
				user_enter_factory_mode(0);
				nvic_system_reset();
			}
		}
	}
}

void vResetTimeoutTick(void)
{
	xSemaphoreGive(xSmphrOTATimeoutSignal);
}
