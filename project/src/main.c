/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
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
#include "at32f403a_407_wk_config.h"
#include "wk_debug.h"
#include "wk_i2c.h"
#include "spi_flash.h"
#include "wk_usart.h"
#include "wk_wdt.h"
#include "wk_gpio.h"
#include "wk_system.h"
#include "gt_flash.h"
#include "gt_adc.h"
#include "freertos_app.h"
#include "boot.h"
#include "version_info.h"
#include "cm_backtrace.h"


#include "user_time.h"
#include "modbus_operation.h"
#include "user_flash.h"

#include "ota_mcu.h"
extern i2c_handle_type hi2c1,hi2c2,hi2c3;




/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */


#define OFFSET_NVIC 1

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
	#if OFFSET_NVIC
  nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x8000);  
  #else
	
	#endif
	/* add user code end 1 */
  cm_backtrace_init("CmBacktrace", HARDWARE_VERSION, VERSION_INFO);
  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();
  
  adc_init();   
	
  time4_int_init();
  // /* init usart1 function. */
  wk_usart1_init();

  // /* init usart2 function. */
  wk_usart2_init();

  /* init spi1 function. */
  spiflash_init();

  // /* init i2c1 function. */
  // wk_i2c1_init();

  // /* init i2c2 function. */
  // wk_i2c2_init();
  i2c_config(&hi2c1);
  i2c_config(&hi2c2);
  i2c_config(&hi2c3);

  /* init wdt function. */
  wdt_counter_reload();
  wk_wdt_init();
  wdt_counter_reload();

  /* init gpio function. */
  wk_gpio_config();

  gt_flash_init();
  check_ota_success(); 
  calendar_type date_time;
  date_time.year = 2025;
  date_time.month = 11;
  date_time.date = 30;
  date_time.hour = 19;
  date_time.min = 12;
  date_time.sec = 00;
  rtc_init(&date_time);
  
	#if OFFSET_NVIC
  setStartOk();
  #endif
	
  // test_flash();
  
  //test_i2c2();

  //double v_test = adc1_get_voltage();

  /* init freertos function. */
  wk_freertos_init();

  /* add user code begin 2 */

  /* add user code end 2 */



  while(1)
  {
    /* add user code begin 3 */
    vTaskDelay(1000);		
    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

  /* add user code end 4 */
