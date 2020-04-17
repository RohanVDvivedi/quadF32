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

void delay_until_ms(uint64_t until_millis)
{
	delay_until_us(until_millis * 1000);
}

void delay_for_us(uint64_t micros)
{
	uint64_t now = get_now_micros();
	while(get_now_micros() - now < micros);
}

void delay_until_us(uint64_t until_micros)
{
	while(get_now_micros() < until_micros);
}