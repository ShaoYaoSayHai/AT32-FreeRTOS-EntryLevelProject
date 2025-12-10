#ifndef _HP203B_H
#define _HP203B_H
#include "user_source.h"

#define I2C_ACK 0

#define HP20X_CONVERT_OSR4096 0 << 2

#define HP20X_ADDRESSCMD 0xec
#define HP20X_ADDRESSRD  0xed


#define HP20X_SOFT_RST 0x06
#define HP20X_READ_PT 0x10
#define HP20X_READ_A 0x31
#define HP20X_WR_CONVERT_CMD 0x40


void HP203_relocation(void);

uint8_t HP303B_IIC_Write(uint8_t IIC_Address, uint8_t *buffer, uint16_t count);
uint8_t HP303B_IIC_Read(uint8_t IIC_Address, uint8_t *buffer, uint16_t count);
uint8_t HP303B_IIC_WriteSingle(uint8_t IIC_Address, uint8_t buffer);

uint8_t Hp203bReadPressureTemperature(float *Hp203bPressure, float *Hp203bTemp, float *Hp203bAltitude);

void HP203_relocation2(void);
uint8_t Hp203bReadPressureTemperature2(float *Hp203bPressure, float *Hp203bTemp, float *Hp203bAltitude);
void Hp203bReadId(uint8_t *id);
#endif 


