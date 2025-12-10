#ifndef __MQTT_BUSINESS_TASK_H__
#define __MQTT_BUSINESS_TASK_H__
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
#include "mqtt_client.h"

#define MQTT_BUSSINESS_RETRY_TIME_MAX (15)


typedef struct {
    uint8_t new_msg;
    char topic[128];
    char msg[256];
    uint8_t len;
} MqttMsgRcvc_t;

typedef enum  {
    MQTT_BUSINESS_UNKNOWN = 0,
    MQTT_BUSINESS_CONNECTING = 1,
    MQTT_BUSINESS_CONNECTED = 2,
    MQTT_BUSINESS_SUBING = 3,
    MQTT_BUSINESS_READY = 4,
    MQTT_BUSINESS_MSG_RCVED = 5
} MQTT_BUSINESS_STATUS_t;

void mqtt_business_task(void *pvParameters);
extern TaskHandle_t mqtt_business_task_handle;

#endif // __MQTT_BUSINESS_TASK_H__
