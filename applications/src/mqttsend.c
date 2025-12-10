#include "mqttsend.h"
#include "gt_flash.h"
#include "user_task.h"

#define MUTEX_ENABLE 0

SemaphoreHandle_t xMutexMqttWriteBufferProtectHandler = NULL;

#define SC_TYPE1_BUFFER_SIZE 256
#define SC_TYPE2_BUFFER_SIZE 128
#define SC_TYPE3_BUFFER_SIZE 128

char sc_warntype_1_buffer[SC_TYPE1_BUFFER_SIZE];
char sc_warntype_2_buffer[SC_TYPE2_BUFFER_SIZE];
char sc_warntype_3_buffer[SC_TYPE3_BUFFER_SIZE];

void mqtt_send_warn(uint8_t type, DataCollectionType ErrType, DataValveStatus ValveState1,
					DataValveStatus ValveState2, DataEquipmentInfo BatteryErrType, DataSensorInfo EAT, float press,
					float tof, float temperature, float flow, char *control_source, int ValveUseTime)
{
	rtc_time_get();
	if (type == 1)
	{
		gt_flash_write_warning_info(&warning);
#if MUTEX_ENABLE
		if (xSemaphoreTake(*pxMutexHandler, 100) != pdFALSE)
		{
			snprintf(sc_warntype_1_buffer, 256,
					 "{\"type\":%d,\"serialNumber\":\"%s\",\"collectStatus\":%d,\"press\":%.2f,\"tof\":%.2f,\"temperature\":%.1f,\"flow\":%.1f}",
					 type, Device_serial_number.sn2, ErrType, press, tof, temperature, flow);
			xSemaphoreGive(*pxMutexHandler);
			xSemaphoreGive();
		}
#else
		snprintf(sc_warntype_1_buffer, 256,
				 "{\"type\":%d,\"serialNumber\":\"%s\",\"collectStatus\":%d,\"press\":%.2f,\"tof\":%.2f,\"temperature\":%.1f,\"flow\":%.1f}",
				 type, Device_serial_number.sn2, ErrType, press, tof, temperature, flow);
		xSemaphoreGive(mqtt_type1_send_signal);
#endif
	}
	else if (type == 2)
	{
#if MUTEX_ENABLE
		if (xSemaphoreTake(*pxMutexHandler, 100) != pdFALSE)
		{
			snprintf(sc_warntype_2_buffer, 256,
					 "{\"type\":%d,\"serialNumber\":\"%s\",\"sw_sta\":%d,\"sw_opa\":%d,\"controlSource\":\"%s\",\"vacTime\":%d}",
					 type, Device_serial_number.sn2, ValveState1, ValveState2, control_source, ValveUseTime);
			xSemaphoreGive(*pxMutexHandler);
		}
#else
		snprintf(sc_warntype_2_buffer, SC_TYPE2_BUFFER_SIZE,
				 "{\"type\":%d,\"serialNumber\":\"%s\",\"sw_sta\":%d,\"sw_opa\":%d,\"controlSource\":\"%s\",\"vacTime\":%d}",
				 type, Device_serial_number.sn2, ValveState1, ValveState2, control_source, ValveUseTime);
		xSemaphoreGive(mqtt_type2_send_signal);
#endif
	}
	else if (type == 3)
	{
#if MUTEX_ENABLE
		if (xSemaphoreTake(*pxMutexHandler, 100) != pdFALSE)
		{
			snprintf(sc_warntype_3_buffer, 256,
					 "{\"type\":%d,\"serialNumber\":\"%s\",\"pwrStatus\":%d,\"prsStatus\":%d}",
					 type, Device_serial_number.sn2, BatteryErrType, EAT);
			xSemaphoreGive(*pxMutexHandler);
		}
#else
		snprintf(sc_warntype_3_buffer, SC_TYPE3_BUFFER_SIZE,
				 "{\"type\":%d,\"serialNumber\":\"%s\",\"pwrStatus\":%d,\"prsStatus\":%d}",
				 type, Device_serial_number.sn2, BatteryErrType, EAT);
		xSemaphoreGive(mqtt_type3_send_signal);

#endif
	}
	// xSemaphoreGive(mqtt_send_sem);
}

char *pcTakeTypeBuffer1(void)
{
	return sc_warntype_1_buffer;
}

char *pcTakeTypeBuffer2(void)
{
	return sc_warntype_2_buffer;
}

char *pcTakeTypeBuffer3(void)
{
	return sc_warntype_3_buffer;
}