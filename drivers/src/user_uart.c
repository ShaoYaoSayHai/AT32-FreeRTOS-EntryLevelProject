#include "user_uart.h"


void uart1_send_data(const uint8_t *data,uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
        usart_data_transmit(USART1, data[i]);
    }	
}

void uart2_send_data(const uint8_t *data,uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
        usart_data_transmit(USART2, data[i]);
    }	
}

