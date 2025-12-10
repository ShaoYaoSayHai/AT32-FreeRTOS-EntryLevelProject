/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f403a_407_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"

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

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

//uint8_t uart_rx_buffer[256] = {0};
//uint32_t uart_rx_index = 0;

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
//void HardFault_Handler(void)
//{
//  /* add user code begin HardFault_IRQ 0 */

//  /* add user code end HardFault_IRQ 0 */
//  /* go to infinite loop when hard fault exception occurs */
//  while (1)
//  {
//    /* add user code begin W1_HardFault_IRQ 0 */

//    /* add user code end W1_HardFault_IRQ 0 */
//  }
//}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}


/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

extern void xPortSysTickHandler(void);

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  /* add user code begin SysTick_IRQ 0 */

  /* add user code end SysTick_IRQ 0 */


#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */

  /* add user code begin SysTick_IRQ 1 */

  /* add user code end SysTick_IRQ 1 */
}

// /**
//   * @brief  this function handles USART1 handler.
//   * @param  none
//   * @retval none
//   */
// void USART1_IRQHandler(void)
// {
//   /* add user code begin USART1_IRQ 0 */

//   /* add user code end USART1_IRQ 0 */
//   /* add user code begin USART1_IRQ 1 */

//   /* add user code end USART1_IRQ 1 */
// }

// /**
//   * @brief  this function handles USART2 handler.
//   * @param  none
//   * @retval none
//   */
// void USART2_IRQHandler(void)
// {
//   /* add user code begin USART2_IRQ 0 */
//   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//   // 接收中断
//   if (usart_flag_get(USART2, USART_RDBF_FLAG) != RESET)
//   {
//     // 读取接收到的数据
//     uint8_t data = usart_data_receive(USART2);

//     // 存储到缓冲区
//     if (uart_rx_index < UART_RX_BUF_SIZE)
//     {
//       uart_rx_buffer[uart_rx_index++] = data;
//     }
//     else
//     {
// 			for(int i=0;i<uart_rx_index;i++)
// 			{
// 				uart_rx_buffer[i]=0;
// 			}
//       uart_rx_index = 0;
//     }
//   }

//   // 空闲中断
//   if (usart_flag_get(USART2, USART_IDLEF_FLAG) != RESET)
//   {
//     // 清除空闲中断标志
//     usart_data_receive(USART2);

//     if (uart_rx_index > 0)
//     {
//       // 创建接收数据结构
//                   uart_rx_data_t rx_data;
//                   rx_data.length = uart_rx_index;
// 			            for(int l=0;l<uart_rx_index;l++)
// 			            {
// 										rx_data.data[l]=uart_rx_buffer[l];
// 			            }

//       // 发送到队列
// 			if(uart2_rx_queue!=NULL)
// 			{
//           xQueueSendFromISR(uart2_rx_queue, &rx_data, &xHigherPriorityTaskWoken);
// 			}
//     //  xSemaphoreGiveFromISR(uart_tx_sem, &xHigherPriorityTaskWoken);
//       // 重置接收索引
// 			for(int i=0;i<uart_rx_index;i++)
// 			{
// 				uart_rx_buffer[i]=0;
// 			}
//       uart_rx_index = 0;
//     }
//   }

//   // 如果有更高优先级任务被唤醒，执行上下文切�?
//   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//   /* add user code end USART2_IRQ 0 */
//   /* add user code begin USART2_IRQ 1 */

//   /* add user code end USART2_IRQ 1 */
// }

/* add user code begin 1 */

/* add user code end 1 */

extern i2c_handle_type hi2c1,hi2c2,hi2c3;

void I2C1_EVT_IRQHandler(void)
{
  i2c_evt_irq_handler(&hi2c1);
}

/**
  * @brief  this function handles i2c error interrupt request.
  * @param  none
  * @retval none
  */
void I2C1_ERR_IRQHandler(void)
{
  i2c_err_irq_handler(&hi2c1);
}

void I2C2_EVT_IRQHandler(void)
{
  i2c_evt_irq_handler(&hi2c2);
}

/**
  * @brief  this function handles i2c error interrupt request.
  * @param  none
  * @retval none
  */
void I2C2_ERR_IRQHandler(void)
{
  i2c_err_irq_handler(&hi2c2);
}



void I2C3_EVT_IRQHandler(void)
{
  i2c_evt_irq_handler(&hi2c3);
}

/**
  * @brief  this function handles i2c error interrupt request.
  * @param  none
  * @retval none
  */
void I2C3_ERR_IRQHandler(void)
{
  i2c_err_irq_handler(&hi2c2);
}

void DMA1_Channel1_IRQHandler(void)
{

}

/**
  * @brief  this function handles TMR6 handler.
  * @param  none
  * @retval none
  */
void TMR4_GLOBAL_IRQHandler(void)
{
	if (tmr_interrupt_flag_get(TMR4, TMR_OVF_FLAG) != RESET)
	{ 
		ActionTimeCount++;
		tmr_flag_clear(TMR4, TMR_OVF_FLAG);
		
	}
}