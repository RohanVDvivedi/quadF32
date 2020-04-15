#include<delay/delay.h>

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}