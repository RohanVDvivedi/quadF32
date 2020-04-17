#include<delay/delay.h>

void delay_for(volatile unsigned int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void delay_for_ms(unsigned int millis)
{
	delay_for_us(millis * 1000);
}

void delay_for_us(unsigned int micros)
{
	unsigned int now = get_now_micros();
	while(get_now_micros() - now < micros);
}