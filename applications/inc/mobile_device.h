#ifndef __MOBILE_DEVICE_H__
#define __MOBILE_DEVICE_H__
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "gt_uart.h"

#define MOBILE_DEV_UART2_BUF_SIZE 2600



typedef void (*MqttUrcCb)(uint8_t *msg, uint8_t len);

typedef void (*HttpFileCb)(uint8_t *data, uint32_t len);

typedef struct {
    char len;
    char data[100];         
} AT_Response_t;


void mobile_dev_create_task(uint32_t baud_rate);
void mobile_dev_task(void *pvParameters);
int mobile_dev_send_cmd(char *cmd, char *resp, uint32_t resp_len);
int32_t mobile_dev_send_and_update_cell_firmware();
void mobile_dev_set_mqtt_urc_cb(MqttUrcCb cb);
void mobile_dev_set_http_file_cb(HttpFileCb cb);
int32_t mobile_dev_has_reboot(void);
uint8_t mobile_dev_is_upgrading(void);
#endif