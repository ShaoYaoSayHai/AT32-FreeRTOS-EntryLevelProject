#ifndef _ML307A_H
#define _ML307A_H

#include "user_source.h"
#include "user_task.h"

#define TX_BUFFER_SIZE 512

typedef struct _data_sensor_measurement
{
	  int   type;
    char *serial_number;
    int  collectStatus; 
    float press;
    float tof;
    float temperature;
    float flow;
//·§ÃÅ×´Ì¬·µ»Ø
	  int  valveStatus;
	  int  valveOperate;
//´íÎó×´Ì¬ÍÆËÍ
	  int  pwrStatus;
	  int  prsStatus;
} _data_sensor_measurement;





extern _data_sensor_measurement ml307_send_data;

extern  char userid[18];

void MQTT_Init(void);
void MQTT_Connect(void);
void Send_AT_Command(const char* cmd);
void MQTT_Publish(_data_sensor_measurement *sensor_data);

void EN_4G_GPIO(void);
void ML307_get_time(void);

void return_okml307a(void);
void return_valvestate(_data_sensor_measurement *sensor_valve);

void return_error(_data_sensor_measurement *sensor_valve);

#endif
