#ifndef _MBRTU_H
#define _MBRTU_H

#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#include "hpw6862i.h"
#include "hp203b.h"
#include "user_task.h"
#include "gt_uart.h"
#include "gt_flash.h"


//#include "bsp_button.h"

#include "modbus_operation.h"

void modbus_init_devices(void);

void modbus_rtu_create_task(void);

int32_t modbus_data_handle(uint8_t *request, uint32_t length, uint8_t *rsp_buff, uint32_t *rsp_buff_len);

#endif
