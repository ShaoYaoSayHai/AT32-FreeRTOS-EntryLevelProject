#include "hpw6862i.h"

HPW6862I_COEFFICIENT sensor1,sensor2;

static uint8_t BMP6862I_ReadReg(uint8_t reg,i2c_handle_type *hi2c);
static void BMP6862I_WriteReg(uint8_t reg, uint8_t value,i2c_handle_type *hi2c);
static void BMP6862I_ReadCalibrationData(i2c_handle_type *hi2c,HPW6862I_COEFFICIENT *sensor);
static float BMP6862I_CompensatePressure(int32_t pressure_raw, int32_t temperature_raw,HPW6862I_COEFFICIENT sensor);
static float BMP6862I_CompensateTemperature(int32_t temperature_raw,HPW6862I_COEFFICIENT sensor);


static uint8_t BMP6862I_ReadReg(uint8_t reg,i2c_handle_type *hi2c)
{
	uint8_t value = 0;
	i2c_memory_read(hi2c,BMP6862I_I2C_ADDR,reg,&value,1,1000);
  return value;
}

static void BMP6862I_WriteReg(uint8_t reg, uint8_t value,i2c_handle_type *hi2c)
{
	i2c_memory_write(hi2c,BMP6862I_I2C_ADDR,reg,&value,1,1000);
}	

static void BMP6862I_ReadCalibrationData(i2c_handle_type *hi2c,HPW6862I_COEFFICIENT *sensor)  //读取传感器校准系数(每个传感器唯一)
{
    uint8_t buf[18];
    i2c_memory_read(hi2c,BMP6862I_I2C_ADDR,BMP6862I_REG_COEF_START,buf,18,1000);
    sensor->c0 = (int16_t)((buf[0] << 4) | ((buf[1] >> 4) & 0x0F));
    if(sensor->c0 > 2047) sensor->c0 -= 4096;
    
    sensor->c1 = (int16_t)(((buf[1] & 0x0F) << 8) | buf[2]);
    if(sensor->c1 > 2047) sensor->c1 -= 4096;
    
    sensor->c00 = (int32_t)((buf[3] << 12) | (buf[4] << 4) | ((buf[5] >> 4) & 0x0F));
    if(sensor->c00 > 524287) sensor->c00 -= 1048576;
    
    sensor->c10 = (int32_t)(((buf[5] & 0x0F) << 16) | (buf[6] << 8) | buf[7]);
    if(sensor->c10 > 524287) sensor->c10 -= 1048576;
    
    sensor->c20 = (int16_t)((buf[12] << 8) | buf[13]);
    if(sensor->c20 > 32767) sensor->c20 -= 65536;
    
    sensor->c30 = (int16_t)((buf[16] << 8) | buf[17]);
    if(sensor->c30 > 32767) sensor->c30 -= 65536;
    
    sensor->c01 = (int16_t)((buf[8] << 8) | buf[9]);
    if(sensor->c01 > 32767) sensor->c01 -= 65536;
    
    sensor->c11 = (int16_t)((buf[10] << 8) | buf[11]);
    if(sensor->c11 > 32767) sensor->c11 -= 65536;
    
    sensor->c21 = (int16_t)((buf[14] << 8) | buf[15]);
    if(sensor->c21 > 32767) sensor->c21 -= 65536;
}

int32_t BMP6862I_Init(i2c_handle_type *hi2c)
{

  uint8_t retry_time = 10;

  BMP6862I_WriteReg(BMP6862I_REG_RESET, 0x09,hi2c);
  vTaskDelay(pdMS_TO_TICKS(50));
	

  while(!(BMP6862I_ReadReg(BMP6862I_REG_MEAS_CFG,hi2c) & 0x40))
  {
    retry_time --;
    if(retry_time == 0)
    {
      return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
	
	if(hi2c->i2cx==I2C1)
	{
	BMP6862I_ReadCalibrationData(hi2c,&sensor1);	
	}
	else if(hi2c->i2cx==I2C2)
	{
  BMP6862I_ReadCalibrationData(hi2c,&sensor2);
	}
	
	BMP6862I_WriteReg(BMP6862I_REG_PRS_CFG, 0x42, hi2c);
	
  BMP6862I_WriteReg(BMP6862I_REG_TMP_CFG, 0xA2, hi2c);
	
  BMP6862I_WriteReg(BMP6862I_REG_CFG_REG, 0x00, hi2c);
	
  BMP6862I_WriteReg(BMP6862I_REG_MEAS_CFG, BMP6862I_MODE_BKG_BOTH, hi2c);	
  return 0;
}

uint8_t BMP6862I_ReadData(BMP6862I_Data_t *data,i2c_handle_type *hi2c,HPW6862I_COEFFICIENT sensor)  //读取传感器压力温度数据
{
    uint8_t ucRxBuff[6];
    int32_t pressure_raw, temperature_raw;
    
	  if(i2c_memory_read(hi2c,BMP6862I_I2C_ADDR,BMP6862I_REG_PRS_B2,ucRxBuff,6,1000)!=I2C_OK)
		{
        return 0;
		}
    
    pressure_raw = (int32_t)((ucRxBuff[0] << 16) | (ucRxBuff[1] << 8) | ucRxBuff[2]);
    if(pressure_raw & 0x00800000) {
        pressure_raw |= 0xFF000000; 
    }
    
    temperature_raw = (int32_t)((ucRxBuff[3] << 16) | (ucRxBuff[4] << 8) | ucRxBuff[5]);
    if(temperature_raw & 0x00800000) {
        temperature_raw |= 0xFF000000; 
    }
    data->temperature = BMP6862I_CompensateTemperature(temperature_raw,sensor);
    data->pressure = BMP6862I_CompensatePressure(pressure_raw, temperature_raw,sensor);
    return 1;
}

static float BMP6862I_CompensatePressure(int32_t pressure_raw, int32_t temperature_raw,HPW6862I_COEFFICIENT sensor)
{
    float p_raw_sc = (float)pressure_raw / 3670016.0f;
    float t_raw_sc = (float)temperature_raw / 3670016.0f;
    
    float p_comp = sensor.c00 + p_raw_sc * (sensor.c10 + p_raw_sc * (sensor.c20 + p_raw_sc * sensor.c30)) 
                 + t_raw_sc * sensor.c01 + t_raw_sc * p_raw_sc * (sensor.c11 + p_raw_sc * sensor.c21);
    
    return p_comp; 
}

static float BMP6862I_CompensateTemperature(int32_t temperature_raw,HPW6862I_COEFFICIENT sensor)
{
    float t_raw_sc = (float)temperature_raw / 3670016.0f;
    
    float t_comp = sensor.c0 * 0.5f + sensor.c1 * t_raw_sc;
    
    return t_comp; 
}

int32_t BMP6862I_ReadId(i2c_handle_type *hi2c,  uint8_t *id)
{
    id[0] = BMP6862I_ReadReg(0x25,hi2c);
    id[1] = BMP6862I_ReadReg(0x26,hi2c);
    id[2] = BMP6862I_ReadReg(0x27,hi2c);
    id[3] = BMP6862I_ReadReg(0x28,hi2c);
    return 0;
}
