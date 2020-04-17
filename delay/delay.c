#include<delay/delay.h>

void delay_for(volatile unsigned int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void delay_for_ms(uint64_t millis)
{
	delay_for_us(millis * 1000);
}

void delay_for_us(uint64_t micros)
{
	uint64_t now = get_now_micros();
	while(get_now_micros() - now < micros);
}