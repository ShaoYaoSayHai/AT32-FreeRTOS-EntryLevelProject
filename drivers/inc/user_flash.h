#ifndef _USER_FLASH_H
#define _USER_FLASH_H

#include "user_source.h"

#include "spi_flash.h"

#define TH_VALUE_WR_FLAG 0x01
#define SW_MODE_WR_FLAG 0x01

void write_flash_userParameters(float *ove_p, float *owe_p, float *ove_t, float *ove_flow, bool *aicontrol);

void read_flash_userParameters(float *ove_p, float *owe_p, float *ove_t, float *ove_flow );

void read_flash_sw_mode(bool *sw_mode) ; // read sw mode value

/**
 * @brief ���ض�ȡ�����Ƿ�д�����־λ��Ϣ
 * 
 * @return uint8_t* 
 */
uint8_t *en_read_flash_wr_flag(void) ;

#endif
