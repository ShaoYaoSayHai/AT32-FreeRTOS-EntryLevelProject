#ifndef _USER_TASK_H
#define _USER_TASK_H

#include "user_source.h"

// #include "key.h"
#include "user_uart.h"
#include "data_process.h"
#include "i2c_application.h"
#include "hpw6862i.h"
#include "soft_i2c.h"
#include "hp203b.h"
#include "valve_control.h"
#include "mainprocess.h"
#include "w25q.h"

#define TASK1_BIT (1 << 0)
#define TASK2_BIT (1 << 1)
#define ALL_TASKS_BITS (TASK1_BIT | TASK2_BIT)

#define HPW203B_RCV_SIG_TIMEOUT_MS 1000

typedef enum
{

	IN_UNKOWN_MODE,
	IN_FACTORY_MODE,
	IN_APP_MODE,

} FACTORY_MODE_t;

extern i2c_handle_type hi2c1, hi2c2;
extern volatile float air_p;

extern char pub_buffer[256];

char *pcTakeTypeBuffer1(void);
char *pcTakeTypeBuffer2(void);
char *pcTakeTypeBuffer3(void);

extern uint16_t ActionTimeCount;

typedef struct _data_sensor_sendnet
{
	int type;
	char *serial_number;
	int collectStatus;

	float press;
	float tof;
	float temperature;
	float flow;

	int valveStatus;
	int valveOperate;

	char *controlSource;

	int pwrStatus;
	int prsStatus;
} _data_sensor_sendnet;
extern _data_sensor_sendnet mqtt_net_send_data;

typedef struct
{
	float sensorf_p;
	float sensorf_t;
	float sensorb_p;
	float sensorb_t;
} hpw6862i_data_format;

typedef struct
{
	float sensor_p;
	float sensor_t;
	float sensor_h;
} hp203b_data_format;

typedef struct
{
	uint8_t *data;
	uint16_t length;
} uart_rx_data_t;



extern bool happen_warn;

void user_queue_create(void);
void user_sem_create(void);
void user_Mute_create(void);
void user_task_create(void);
void user_eventgroup_create(void);

void key_task(void *pvParameters);
void uart2_task(void *pvParameters);
void ml307a_send_task(void *pvParameters);
void BMP6862I_Task(void *pvParameters);
void hp203b_task(void *pvParameters);
void valva_control_task(void *pvParameters);
void main_process_task(void *pvParameters);
void watchdog_task(void *pvParameters);
void mqtt_send_receive_task(void *pvParameters);

extern volatile bool netSetValveState;


extern TaskHandle_t uart2_task_handle;
extern TaskHandle_t ml307a_send_task_handle;
extern TaskHandle_t hpw6862i_task_handle;
extern TaskHandle_t hp203b_task_handle;
extern TaskHandle_t valva_control_task_handle;
extern TaskHandle_t main_process_task_handle;
extern TaskHandle_t xWatchdogTaskHandle;
extern TaskHandle_t mqtt_send_receive_task_handle;

// 产测模式下看门狗任务
extern TaskHandle_t xFactoryModeWatchDogTaskHandler;

extern SemaphoreHandle_t bt_change_valve_sem;
extern SemaphoreHandle_t net_change_valve_sem;
extern SemaphoreHandle_t bt_security_check_sem;
extern SemaphoreHandle_t ml307a_sendat_sem;
extern SemaphoreHandle_t ml307a_sendover_sem;
extern SemaphoreHandle_t xOnem_sem;
extern SemaphoreHandle_t ValveControl_open_sem;
extern SemaphoreHandle_t ValveControl_close_sem;
extern SemaphoreHandle_t valve_action_success_sem;
extern SemaphoreHandle_t Remove_4g_close_valve_sem;
extern SemaphoreHandle_t mqtt_send_sem;

extern SemaphoreHandle_t mqtt_type1_send_signal;
extern SemaphoreHandle_t mqtt_type2_send_signal;
extern SemaphoreHandle_t mqtt_type3_send_signal;

extern SemaphoreHandle_t valve_protect_Mute;

// 创建一个FACTORY MODE �?
extern SemaphoreHandle_t xMutexFactoryMode;

extern SemaphoreHandle_t xMutexMqttWriteBufferProtectHandler ; 

extern QueueHandle_t hpw6862i_queue;

extern EventGroupHandle_t xEventGroup;

void mqtt_message_received_callback(char *topic, uint8_t *msg, uint8_t len);
void process_mqtt_message(char *message_buffer, uint8_t len);

FACTORY_MODE_t xGetFactoryModeCode(void);
#endif
