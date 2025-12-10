#ifndef __BT_DEVICE_H__
#define __BT_DEVICE_H__
#include "mobile_device.h"
#include "gt_uart.h"

extern TaskHandle_t bt_dev_task_handle;

char *get_bt_version(void);
char *get_bt_name(void);
char *get_bt_mac(void);
int set_bt_mac_to_dev(char *mac);
int set_bt_name_to_dev(char *name);
void reset_bt_dev(void);

void bt_dev_create_task(void);
void bt_dev_task(void *pvParameters);
#endif