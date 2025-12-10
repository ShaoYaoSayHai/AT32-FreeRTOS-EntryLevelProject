#ifndef _MQTTSEND_H
#define _MQTTSEND_H

#include "user_source.h"


typedef enum{
NO_ERROR = 0 , // ??
LEAK ,  // ??
OVER_FLOW, // ??
OVER_PRESS, // ??
INSUFFICIENT_PRESS, // ??
EXCESS_TEMP ,  // ??
UNKOWN_WARNING // ????
}DataCollectionType ;


typedef enum{
VV_CLOSED = 0 , // ????
VV_OPEN , // ???? 
VV_ERROR // ??????
}DataValveStatus; 


typedef enum{
EI_OK,
LOW_BATTERY_LEVEL, // ??????
LOSS_of_PWR_SUPPLY , //??????
EI_UNKOWN_ERROR // ????
}DataEquipmentInfo;

typedef enum{
SENSOR_OK, // ?????
SENSOR_ERROR , //?????
SENSOR_WRONG_DATA , //???????
}DataSensorInfo; 

void mqtt_send_warn(uint8_t type,DataCollectionType ErrType,DataValveStatus ValveState1,
	DataValveStatus ValveState2,DataEquipmentInfo BatteryErrType,DataSensorInfo EAT,float press,
		float tof,float temperature,float flow,char *control_source,int ValveUseTime);

#endif
