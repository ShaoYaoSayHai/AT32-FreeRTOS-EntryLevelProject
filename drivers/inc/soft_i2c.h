/**
 **************************************************************************
 * @file     soft i2c.h
 * @version  v2.0.0
 * @date     2020-11-02
 * @brief    i2c application libray header file
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

/*!< define to prevent recursive inclusion -------------------------------------*/
#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ------------------------------------------------------------------*/
#include "user_source.h"
#include "delay.h"
  /** @addtogroup AT32F403A_407_middlewares_i2c_application_library
   * @{
   */

//#define I2Cx_SCL_PIN GPIO_PINS_13
//#define I2Cx_SCL_GPIO_PORT GPIOB
//#define I2Cx_SCL_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK

//#define I2Cx_SDA_PIN GPIO_PINS_12
//#define I2Cx_SDA_GPIO_PORT 

#define I2Cx_SCL_PIN GPIO_PINS_12
#define I2Cx_SCL_GPIO_PORT GPIOB
#define I2Cx_SCL_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK

#define I2Cx_SDA_PIN GPIO_PINS_13
#define I2Cx_SDA_GPIO_PORT GPIOB
#define I2Cx_SDA_GPIO_CLK CRM_GPIOB_PERIPH_CLOCK

  /** @defgroup I2C_library_exported_functions
   * @{
   */

  void soft_i2c_config(void);

  uint8_t soft_i2c_master_transmit(uint16_t address, uint8_t *pdata, uint16_t size);
  uint8_t soft_i2c_master_receive(uint16_t address, uint8_t *pdata, uint16_t size);

  uint8_t soft_i2c_memory_write(uint16_t address, uint16_t mem_address, uint8_t *pdata, uint16_t size);
  uint8_t soft_i2c_memory_read(uint16_t address, uint16_t mem_address, uint8_t *pdata, uint16_t size);

  /**
   * @brief  send a byte.
   * @param  data: byte to be transmitted.
   * @retval none.
   */
  uint8_t i2c_send_byte(uint8_t data);

  /**
   * @brief  receive a byte.
   * @param  data: byte to be received.
   * @retval none.
   */
  uint8_t i2c_receive_byte(void);

  /**
   * @brief  used to generate start conditions.
   * @param  none.
   * @retval none.
   */
  void i2c_start(void);

  /**
   * @brief  used to generate stop conditions.
   * @param  none.
   * @retval none.
   */
  void i2c_stop(void);

  /**
   * @brief  used to generate ack conditions.
   * @param  none.
   * @retval none.
   */
  void i2c_ack(void);

  /**
   * @brief  used to generate nack conditions.
   * @param  none.
   * @retval none.
   */
  void i2c_no_ack(void);

  /**
   * @}
   */

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif
