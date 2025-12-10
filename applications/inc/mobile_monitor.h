#ifndef __MOBILE_MONITOR_H__
#define __MOBILE_MONITOR_H__
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t mobile_task_handle;

typedef enum  {
    MOBILE_STATUS_UNKNOWN = 0,
    MOBILE_STATUS_DEVICE_READY = 1,
    MOBILE_STATUS_SIM_CARD_READY = 2,
    MOBILE_STATUS_REG_READY = 3,
    MOBILE_STATUS_NETWORK_READY = 4

} MOBILE_STATUS_t;

void reset_mobile_device(void);
char *get_mobile_imei(void);
char *get_sim_imsi(void);
char *get_sim_iccid(void);
int get_rssi(void);
char *get_mobile_model(void);
char *get_mobile_ver(void);
char *get_ipv4_addr(void);
uint8_t get_sim_card_ready(void);
uint8_t get_network_ready(void);
void mobile_monitor_create_task(void);
#endif