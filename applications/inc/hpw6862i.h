#ifndef __HPW6862I_H
#define __HPW6862I_H

#include "user_source.h"
#include "i2c_application.h"


#define BMP6862I_I2C_ADDR   0xEE

#define BMP6862I_REG_PRS_B2      0x00
#define BMP6862I_REG_PRS_B1      0x01
#define BMP6862I_REG_PRS_B0      0x02
#define BMP6862I_REG_TMP_B2      0x03
#define BMP6862I_REG_TMP_B1      0x04
#define BMP6862I_REG_TMP_B0      0x05
#define BMP6862I_REG_PRS_CFG     0x06
#define BMP6862I_REG_TMP_CFG     0x07
#define BMP6862I_REG_MEAS_CFG    0x08
#define BMP6862I_REG_CFG_REG     0x09
#define BMP6862I_REG_FIFO_STS    0x0B
#define BMP6862I_REG_RESET       0x0C
#define BMP6862I_REG_ID          0x0D
#define BMP6862I_REG_COEF_START  0x10

#define BMP6862I_MODE_STANDBY    0x00
#define BMP6862I_MODE_CMD_PRS    0x01
#define BMP6862I_MODE_CMD_TMP    0x02
#define BMP6862I_MODE_BKG_PRS    0x05
#define BMP6862I_MODE_BKG_TMP    0x06
#define BMP6862I_MODE_BKG_BOTH   0x07

typedef struct {
    float pressure;
    float temperature;
} BMP6862I_Data_t;

typedef struct {
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;  
} HPW6862I_COEFFICIENT;

extern HPW6862I_COEFFICIENT sensor1,sensor2;

int32_t BMP6862I_Init(i2c_handle_type *hi2c);
uint8_t BMP6862I_ReadData(BMP6862I_Data_t *data,i2c_handle_type *hi2c,HPW6862I_COEFFICIENT sensor);
int32_t BMP6862I_ReadId(i2c_handle_type *hi2c,  uint8_t *id);


#endif

