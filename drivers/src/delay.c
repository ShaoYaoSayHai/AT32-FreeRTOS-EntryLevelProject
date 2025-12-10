#include "delay.h"



void delay_us(uint32_t us)
{
	
			for( int i=0;i<(20 * us);i++ )
		{
			__NOP();
			__NOP();
			__NOP();
			__NOP();
		}
	
}

void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_us(1000);
    }
}