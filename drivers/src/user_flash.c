#include "user_flash.h"
#include "gt_flash.h"

uint8_t en_wr_flag_buffer[8];

void write_flash_userParameters(float *ove_p, float *owe_p, float *ove_t, float *ove_flow, bool *aicontrol)
{
	uint8_t data_buffer_write[19];

	// float类型写入
	memcpy(&data_buffer_write[0], ove_p, sizeof(float));
	memcpy(&data_buffer_write[4], owe_p, sizeof(float));
	memcpy(&data_buffer_write[8], ove_t, sizeof(float));
	memcpy(&data_buffer_write[12], ove_flow, sizeof(float));
	// 常开常闭模式
	data_buffer_write[16] = *aicontrol ? 0x01 : 0x00;
	data_buffer_write[17] = TH_VALUE_WR_FLAG; // 阈值标志位写入
	data_buffer_write[18] = SW_MODE_WR_FLAG;  // 常开常闭是否写入标志位
	spiflash_write_with_lock(data_buffer_write, 0x5000, 19);
}

void read_flash_userParameters(float *ove_p, float *owe_p, float *ove_t, float *ove_flow)
{
	uint8_t data_buffer_read[17];

	spiflash_read_with_lock(data_buffer_read, 0x5000, 17);
	memcpy(ove_p, &data_buffer_read[0], sizeof(float));
	memcpy(owe_p, &data_buffer_read[4], sizeof(float));
	memcpy(ove_t, &data_buffer_read[8], sizeof(float));
	memcpy(ove_flow, &data_buffer_read[12], sizeof(float));
	// *aicontrol = (data_buffer_read[16] == 0x01);
}

void read_flash_sw_mode(bool *sw_mode)
{
	uint8_t tempValue = 0;
	spiflash_read_with_lock(&tempValue, 0x5016, 1);
	*sw_mode = tempValue;
	return;
}

/**
 * @brief 返回读取到的是否写入过标志位信息
 *
 * @return uint8_t*
 */
uint8_t *en_read_flash_wr_flag(void)
{
	// en_wr_flag_buffer 用作缓存数据
	// 读取 0x5017 0x5018
	spiflash_read_with_lock(en_wr_flag_buffer, 0x5017, 2);
	return &en_wr_flag_buffer[0];
}
