/* add user code begin Header */
/**
  **************************************************************************
  * @file     wk_i2c.c
  * @brief    work bench config program
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
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "wk_i2c.h"
#include "delay.h"
#include "i2c_application.h"

/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief  init i2c1 function.
  * @param  none
  * @retval none
  */
void wk_i2c1_init(void)
{
  /* add user code begin i2c1_init 0 */

  /* add user code end i2c1_init 0 */

  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the SDA pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init(GPIOB, &gpio_init_struct);

  nvic_irq_enable(I2C1_EVT_IRQn, 0, 0);
  nvic_irq_enable(I2C1_ERR_IRQn, 0, 0);


  i2c_init(I2C1, I2C_FSMODE_DUTY_2_1, 100000);
  i2c_own_address1_set(I2C1, I2C_ADDRESS_MODE_7BIT, 0xA0);
  // i2c_ack_enable(I2C1, TRUE);
  // i2c_clock_stretch_enable(I2C1, TRUE);
  // i2c_general_call_enable(I2C1, FALSE);

  /**
   * Users need to configure I2C1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding I2C1 interrupt.
   *     --i2c_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32a403a_int.c file.
   *     --void I2C1_EVT_IRQHandler(void)
   *     --void I2C1_ERR_IRQHandler(void)
   */

  /* add user code begin i2c1_init 2 */

  /* add user code end i2c1_init 2 */

  // i2c_enable(I2C1, TRUE);

  /* add user code begin i2c1_init 3 */

  /* add user code end i2c1_init 3 */
}

/**
  * @brief  init i2c2 function.
  * @param  none
  * @retval none
  */
void wk_i2c2_init(void)
{
  /* add user code begin i2c2_init 0 */

  /* add user code end i2c2_init 0 */

  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c2_init 1 */

  /* add user code end i2c2_init 1 */
  
  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOB, &gpio_init_struct);

  /* configure the SDA pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_11;
  gpio_init(GPIOB, &gpio_init_struct);


  nvic_irq_enable(I2C2_EVT_IRQn, 0, 0);
  nvic_irq_enable(I2C2_ERR_IRQn, 0, 0);
  i2c_init(I2C2, I2C_FSMODE_DUTY_2_1, 100000);
  i2c_own_address1_set(I2C2, I2C_ADDRESS_MODE_7BIT, 0xA0);
  // i2c_ack_enable(I2C2, TRUE);
  // i2c_clock_stretch_enable(I2C2, TRUE);
  // i2c_general_call_enable(I2C2, FALSE);

  /* add user code begin i2c2_init 2 */

  /* add user code end i2c2_init 2 */

  // i2c_enable(I2C2, TRUE);

  /* add user code begin i2c2_init 3 */

  /* add user code end i2c2_init 3 */
}

/**
  * @brief  init i2c3 function.
  * @param  none
  * @retval none
  */
void wk_i2c3_init(void)
{
  /* add user code begin i2c3_init 0 */

  /* add user code end i2c3_init 0 */

  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c3_init 1 */

  /* add user code end i2c3_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure the SDA pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_remap_config(I2C3_GMUX_0001, TRUE);
  nvic_irq_enable(I2C3_EVT_IRQn, 0, 0);
  nvic_irq_enable(I2C3_ERR_IRQn, 0, 0);

  i2c_init(I2C3, I2C_FSMODE_DUTY_2_1, 100000);
  i2c_own_address1_set(I2C3, I2C_ADDRESS_MODE_7BIT, 0xA0);
  // i2c_ack_enable(I2C3, TRUE);
  // i2c_clock_stretch_enable(I2C3, TRUE);
  // i2c_general_call_enable(I2C3, FALSE);

  /* add user code begin i2c3_init 2 */

  /* add user code end i2c3_init 2 */

  // i2c_enable(I2C3, TRUE);

  /* add user code begin i2c3_init 3 */

  /* add user code end i2c3_init 3 */
}

void gt_i2c1_reboot_device(void)
{
  int i = 0;
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init(GPIOB, &gpio_init_struct);
  for(i = 0; i< 10; i++){
  gpio_bits_reset(GPIOB, GPIO_PINS_6);
  delay_us(2);
  gpio_bits_set(GPIOB, GPIO_PINS_6);
  delay_us(2);
  }
}

void gt_i2c2_reboot_device(void)
{
  int i = 0;
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOB, &gpio_init_struct);
  for(i = 0; i< 10; i++){
  gpio_bits_reset(GPIOB, GPIO_PINS_10);
  delay_us(2);
  gpio_bits_set(GPIOB, GPIO_PINS_10);
  delay_us(2);
  }
}

void gt_i2c3_reboot_device(void)
{
  int i = 0;
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init(GPIOA, &gpio_init_struct);
  for(i = 0; i< 10; i++){
  gpio_bits_reset(GPIOA, GPIO_PINS_8);
  delay_us(2);
  gpio_bits_set(GPIOA, GPIO_PINS_8);
  delay_us(2);
  }
}

void i2c_lowlevel_reboot_device(i2c_handle_type* hi2c)
{
  if(hi2c->i2cx == I2C1)
  {
    gt_i2c1_reboot_device();
  } else if(hi2c->i2cx == I2C2)
  {
    gt_i2c2_reboot_device();
  } else if(hi2c->i2cx == I2C3)
  {
    gt_i2c3_reboot_device();
  }
}



void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_initstructure;

  if(hi2c->i2cx == I2C1)
  {
    wk_i2c1_init();
  } else if(hi2c->i2cx == I2C2)
  {
    wk_i2c2_init();
  } else if(hi2c->i2cx == I2C3)
  {
    wk_i2c3_init();
  }
}

/* add user code begin 1 */

/* add user code end 1 */
