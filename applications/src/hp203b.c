#include "hp203b.h"

#include "soft_i2c.h"
#include "i2c_application.h"

extern i2c_handle_type hi2c3;

static inline void I2C_PutAck(uint8_t Ack)
{
    if (I2C_ACK == Ack)
        i2c_ack();
    else
        i2c_no_ack(); 
}

uint8_t HP303B_IIC_Write(uint8_t IIC_Address, uint8_t *buffer, uint16_t count)
{
    uint16_t i;
    i2c_start();
    if (i2c_send_byte(IIC_Address) == 0)
    {
        for (i = 0; i < count; i++)
        {
            i2c_send_byte(buffer[i]);
        }
        i2c_stop();

        return 1;
    }
        return 0;
}

uint8_t HP303B_IIC_Read(uint8_t IIC_Address, uint8_t *buffer, uint16_t count)
{
    uint16_t i = 0;

    count = count - 1;
    i2c_start();
    if (i2c_send_byte(IIC_Address | 0x01) == 0)
    {
        for (i = 0; i < count; i++)
        {
            buffer[i] = i2c_receive_byte();
            I2C_PutAck(0);
        }
        buffer[i] = i2c_receive_byte();
        I2C_PutAck(1);

        // i2c_receive_byte();
        i2c_stop();
        return 1;
    }
    return 0;
}

uint8_t HP303B_IIC_WriteSingle(uint8_t IIC_Address, uint8_t buffer)
{
    if (HP303B_IIC_Write(IIC_Address, &buffer, 1))
        return 1;
    else
        return 0;

}



uint8_t Hp203bReadPressureTemperature(float *Hp203bPressure, float *Hp203bTemp, float *Hp203bAltitude)
{
    uint8_t DataBuf[6];
    long Hp203bPressure_temp, Hp203b_Pressure, Hp203bAltitude_temp;
    uint8_t Osr_Cfg = HP20X_CONVERT_OSR4096;
    if (HP303B_IIC_WriteSingle(HP20X_ADDRESSCMD, (HP20X_WR_CONVERT_CMD | Osr_Cfg))) {
        delay_ms(140);
		HP303B_IIC_WriteSingle(HP20X_ADDRESSCMD, HP20X_READ_PT); 
        HP303B_IIC_Read(HP20X_ADDRESSCMD, DataBuf, 6);

        Hp203bPressure_temp = DataBuf[0];
        Hp203bPressure_temp <<= 8;
        Hp203bPressure_temp |= DataBuf[1];
        Hp203bPressure_temp <<= 8;
        Hp203bPressure_temp |= DataBuf[2];

        if (Hp203bPressure_temp & 0x800000)   
            Hp203bPressure_temp |= 0xff000000; 

        *Hp203bTemp = (float)Hp203bPressure_temp / 100;

        Hp203b_Pressure = DataBuf[3];
        Hp203b_Pressure <<= 8;
        Hp203b_Pressure |= DataBuf[4];
        Hp203b_Pressure <<= 8;
        Hp203b_Pressure |= DataBuf[5];
        *Hp203bPressure = (float)Hp203b_Pressure ;
        HP303B_IIC_WriteSingle(HP20X_ADDRESSCMD, HP20X_READ_A);
        HP303B_IIC_Read(HP20X_ADDRESSCMD, DataBuf, 3);
        Hp203bAltitude_temp = DataBuf[0];
        Hp203bAltitude_temp <<= 8;
        Hp203bAltitude_temp |= DataBuf[1];
        Hp203bAltitude_temp <<= 8;
        Hp203bAltitude_temp |= DataBuf[2];
        if (Hp203bAltitude_temp & 0x800000)
            Hp203bAltitude_temp |= 0xff000000;
        *Hp203bAltitude = (float)Hp203bAltitude_temp / 100;

        return (1);
    }
    else
        return (0);
}
void HP203_relocation(void)
{
    HP303B_IIC_WriteSingle(HP20X_ADDRESSCMD, HP20X_SOFT_RST);
    vTaskDelay(5);
}

void HP203_relocation2(void)
{
    uint8_t data = HP20X_SOFT_RST;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, &data, 1, 1000);
    vTaskDelay(1);
}

uint8_t Hp203bReadPressureTemperature2(float *Hp203bPressure, float *Hp203bTemp, float *Hp203bAltitude)
{
    uint8_t DataBuf[6];

    long Hp203bPressure_temp, Hp203b_Pressure, Hp203bAltitude_temp;
    uint8_t Osr_Cfg = HP20X_WR_CONVERT_CMD | HP20X_CONVERT_OSR4096;
    if (i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, &Osr_Cfg, 1, 1000) == I2C_OK)
    {
        delay_ms(140);    
        Osr_Cfg = HP20X_READ_PT;
        i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, &Osr_Cfg, 1, 1000); 
        i2c_master_receive(&hi2c3, HP20X_ADDRESSRD, DataBuf, 6, 1000);

        Hp203bPressure_temp = DataBuf[0];
        Hp203bPressure_temp <<= 8;
        Hp203bPressure_temp |= DataBuf[1];
        Hp203bPressure_temp <<= 8;
        Hp203bPressure_temp |= DataBuf[2];

        if (Hp203bPressure_temp & 0x800000)   
            Hp203bPressure_temp |= 0xff000000; 

        *Hp203bTemp = (float)Hp203bPressure_temp / 100;

        Hp203b_Pressure = DataBuf[3];
        Hp203b_Pressure <<= 8;
        Hp203b_Pressure |= DataBuf[4];
        Hp203b_Pressure <<= 8;
        Hp203b_Pressure |= DataBuf[5];
        *Hp203bPressure = (float)Hp203b_Pressure ;
        Osr_Cfg = HP20X_READ_A;
        i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, &Osr_Cfg, 1, 1000);
        i2c_master_receive(&hi2c3, HP20X_ADDRESSRD, DataBuf, 3, 1000);
        Hp203bAltitude_temp = DataBuf[0];
        Hp203bAltitude_temp <<= 8;
        Hp203bAltitude_temp |= DataBuf[1];
        Hp203bAltitude_temp <<= 8;
        Hp203bAltitude_temp |= DataBuf[2];
        if (Hp203bAltitude_temp & 0x800000)
            Hp203bAltitude_temp |= 0xff000000;
        *Hp203bAltitude = (float)Hp203bAltitude_temp / 100;

        return (1);
    }
    else
        return (0);
}

void Hp203bReadId(uint8_t *id)
{
    uint8_t DataBuf[4] = {0};
    uint8_t data[2];
    data[0] = 0x20;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
    data[0] = 0x04;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	data[0] = 0x2A;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	delay_ms(10);
	
    data[0] = 0xDF;
    data[1] = 0x00;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0x26;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	data[0] = 0xDE;
	data[1] = 0x00;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDF;
	data[1] = 0x41;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDE;
	data[1] = 0x05;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDF;
	data[1] = 0x43;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	delay_ms(1);
    data[0] = 0x9D;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_receive(&hi2c3, HP20X_ADDRESSCMD, &DataBuf[0], 1, 1000);
	data[0] = 0xDE;
	data[1] = 0x06;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDF;
	data[1] = 0x43;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	delay_ms(1);
    data[0] = 0x9D;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_receive(&hi2c3, HP20X_ADDRESSCMD, &DataBuf[1], 1, 1000);
	data[0] = 0xDE;
	data[1] = 0x07;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDF;
	data[1] = 0x43;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	delay_ms(1);
    data[0] = 0x9D;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_receive(&hi2c3, HP20X_ADDRESSCMD, &DataBuf[2], 1, 1000);
	data[0] = 0xDE;
	data[1] = 0x7F;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	data[0] = 0xDF;
	data[1] = 0x43;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 2, 1000);
	delay_ms(1);
    data[0] = 0x9D;
    i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	i2c_master_receive(&hi2c3, HP20X_ADDRESSCMD, &DataBuf[3], 1, 1000);
	
	data[0] = 0x2C;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
	data[0] = 0x22;
	i2c_master_transmit(&hi2c3, HP20X_ADDRESSCMD, data, 1, 1000);
    id[0] = DataBuf[0];
    id[1] = DataBuf[1];
    id[2] = DataBuf[2];
    id[3] = DataBuf[3];
}
