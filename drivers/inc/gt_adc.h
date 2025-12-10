#ifndef __GT_ADC_H__
#define __GT_ADC_H__
#include "at32f403a_407.h"
#include "at32f403a_407_dma.h"
#include "at32f403a_407_crm.h"
#include "at32f403a_407_adc.h"
#include "at32f403a_407_gpio.h"

//初始化ADC1
void adc_init(void);

//获取电压值(单位:V)
float adc1_get_voltage(void);

float adc2_get_voltage(void);

float adc_get_temperature(void);
#endif