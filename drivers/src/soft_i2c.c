/**
 **************************************************************************
 * @file     soft_i2c.c
 * @version  v2.0.0
 * @date     2020-11-02
 * @brief    the driver library of the soft i2c
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */
#include "soft_i2c.h"
//#include "at32f403a_407_board.h"

/**
 * @brief i2c address direction
 */
#define I2C_DIR_TRANSMIT 0x00
#define I2C_DIR_RECEIVE 0x01

#define I2C_ACK_TIMEOUT 10

#define I2C_SCL_HIGH() gpio_bits_set(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN)
#define I2C_SCL_LOW() gpio_bits_reset(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN)

#define I2C_SDA_HIGH() gpio_bits_set(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN)
#define I2C_SDA_LOW() gpio_bits_reset(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN)

#define I2C_SDA_READ() (gpio_input_data_bit_read(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN) == SET)

/**
 * @brief  i2c gpio initialization.
 * @param  none.
 * @retval none.
 */
void soft_i2c_config(void)
{
  gpio_init_type gpio_initstructure;

  /* i2c gpio clock enable */
  crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

  I2C_SDA_HIGH();
  I2C_SCL_HIGH();

  /* gpio configuration */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

  /* configure i2c pins: scl */
  gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
  gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

  /* configure i2c pins: sda */
  gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
  gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);
}

/**
 * @brief  used to set the i2c clock frequency.
 * @param  none.
 * @retval none.
 */
void i2c_delay(void)
{
  //delay_us(5);
	//dwt_delay_us(5);
	delay_us(5);
}

/**
 * @brief  used to generate start conditions.
 * @param  none.
 * @retval none.
 */
void i2c_start(void)
{
  i2c_delay();

  I2C_SDA_HIGH();
  I2C_SCL_HIGH();
  i2c_delay();

  I2C_SDA_LOW();
  i2c_delay();

  I2C_SCL_LOW();
}

/**
 * @brief  used to generate stop conditions.
 * @param  none.
 * @retval none.
 */
void i2c_stop(void)
{
  I2C_SCL_LOW();
  I2C_SDA_LOW();
  i2c_delay();

  I2C_SCL_HIGH();
  i2c_delay();

  I2C_SDA_HIGH();
  i2c_delay();
}

/**
 * @brief  used to generate ack conditions.
 * @param  none.
 * @retval none.
 */
void i2c_ack(void)
{
  I2C_SCL_LOW();
  I2C_SDA_LOW();
  i2c_delay();

  I2C_SCL_HIGH();
  i2c_delay();

  I2C_SCL_LOW();
}

/**
 * @brief  used to generate nack conditions.
 * @param  none.
 * @retval none.
 */
void i2c_no_ack(void)
{
  I2C_SCL_LOW();
  I2C_SDA_HIGH();
  i2c_delay();

  I2C_SCL_HIGH();
  i2c_delay();

  I2C_SCL_LOW();
}

/**
 * @brief  used to wait ack conditions.
 * @param  none.
 * @retval ack receive status.
 *         - 1: no ack received.
 *         - 0: ack received.
 */
uint8_t i2c_wait_ack(uint8_t timeout)
{
  I2C_SCL_LOW();
  I2C_SDA_HIGH();

  i2c_delay();

  while (timeout)
  {
    if (I2C_SDA_READ() == 0)
    {
      I2C_SCL_HIGH();

      i2c_delay();

      I2C_SCL_LOW();

      return 0;
    }

    i2c_delay();

    timeout--;
  }

  I2C_SCL_HIGH();
  i2c_delay();

  I2C_SCL_LOW();

  i2c_delay();

  return 1;
}

/**
 * @brief  send a byte.
 * @param  data: byte to be transmitted.
 * @retval none.
 */
uint8_t i2c_send_byte(uint8_t data)
{
  uint8_t cnt;
	for(cnt =0;cnt<8;cnt++)
	{
		I2C_SCL_LOW();
		i2c_delay();
		
		if(data&0x80)
		I2C_SDA_HIGH();
    else
    I2C_SDA_LOW();
    data <<= 1;
    i2c_delay();
    I2C_SCL_HIGH();
		i2c_delay();
    		
	}
	I2C_SCL_LOW();
	i2c_delay();
	return i2c_wait_ack(I2C_ACK_TIMEOUT);
	
}

/**
 * @brief  receive a byte.
 * @param  data: byte to be received.
 * @retval none.
 */
uint8_t i2c_receive_byte(void)
{
  uint8_t i = 8;
  uint8_t byte = 0;

  I2C_SDA_HIGH();

  while (i--)
  {
    byte <<= 1;

    I2C_SCL_LOW();
    i2c_delay();

    I2C_SCL_HIGH();
    i2c_delay();

    if (I2C_SDA_READ())
    {
      byte |= 0x01;
    }
  }

  I2C_SCL_LOW();

  return byte;
}

/**
 * @brief  master transmits data.
 * @param  address: slave address.
 * @param  pdata: data buffer.
 * @param  size: data size.
 * @retval transfer status.
 *         - 1: transfer failed.
 *         - 0: transfer successfully.
 */
uint8_t soft_i2c_master_transmit(uint16_t address, uint8_t *pdata, uint16_t size)
{
  uint16_t i;

  i2c_start();

  i2c_send_byte(address | I2C_DIR_TRANSMIT);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  for (i = 0; i < size; i++)
  {
    i2c_send_byte(pdata[i]);

    if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
    {
      i2c_stop();

      return 1;
    }
  }

  i2c_stop();

  return 0;
}

/**
 * @brief  master receive data.
 * @param  address: slave address.
 * @param  pdata: data buffer.
 * @param  size: data size.
 * @retval transfer status.
 *         - 1: transfer failed.
 *         - 0: transfer successfully.
 */
uint8_t soft_i2c_master_receive(uint16_t address, uint8_t *pdata, uint16_t size)
{
  uint16_t i;

  i2c_start();

  i2c_send_byte(address | I2C_DIR_RECEIVE);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  for (i = 0; i < size; i++)
  {
    pdata[i] = i2c_receive_byte();

    if (i < (size - 1))
    {
      i2c_ack();
    }
    else
    {
      i2c_no_ack();
    }
  }

  i2c_stop();

  return 0;
}

/**
 * @brief  write data to the memory device.
 * @param  address: memory device address.
 * @param  mem_address: memory address.
 * @param  pdata: data buffer.
 * @param  size: data size.
 * @retval transfer status.
 *         - 1: transfer failed.
 *         - 0: transfer successfully.
 */
uint8_t soft_i2c_memory_write(uint16_t address, uint16_t mem_address, uint8_t *pdata, uint16_t size)
{
  uint16_t i;

  i2c_start();

  i2c_send_byte(address | I2C_DIR_TRANSMIT);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  i2c_send_byte(mem_address);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  for (i = 0; i < size; i++)
  {
    i2c_send_byte(pdata[i]);

    if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
    {
      i2c_stop();

      return 1;
    }
  }

  i2c_stop();

  return 0;
}

/**
 * @brief  read data from memory device.
 * @param  address: memory device address.
 * @param  mem_address: memory address.
 * @param  pdata: data buffer.
 * @param  size: data size.
 * @retval transfer status.
 *         - 1: transfer failed.
 *         - 0: transfer successfully.
 */
uint8_t soft_i2c_memory_read(uint16_t address, uint16_t mem_address, uint8_t *pdata, uint16_t size)
{
  uint16_t i;

  i2c_start();

  i2c_send_byte(address | I2C_DIR_TRANSMIT);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  i2c_send_byte(mem_address);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  i2c_start();

  i2c_send_byte(address | I2C_DIR_RECEIVE);

  if (i2c_wait_ack(I2C_ACK_TIMEOUT) == 1)
  {
    i2c_stop();

    return 1;
  }

  for (i = 0; i < size; i++)
  {
    pdata[i] = i2c_receive_byte();

    if (i < (size - 1))
    {
      i2c_ack();
    }
    else
    {
      i2c_no_ack();
    }
  }

  i2c_stop();

  return 0;
}
