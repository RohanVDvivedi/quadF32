/*
	A simple blink program built for stm32f103 on a blue pill board or alike
*/

#include<stdint.h>

#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*((volatile uint32_t*)(RCC_BASE + 0x18)))

#define GPIOC_BASE 0x40011000

#define GPIOC_CRH (*((volatile uint32_t*)(GPIOC_BASE + 0x04)))
#define GPIOC_ODR (*((volatile uint32_t*)(GPIOC_BASE + 0x0c)))

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void main(void)
{
	RCC_APB2ENR |= (1<<4);
	GPIOC_CRH   &= 0xFF0FFFFF;
	GPIOC_CRH   |= 0x00200000;
	while(1)
	{
		GPIOC_ODR |= (1 << 13);
		delay_for(1000000);
		GPIOC_ODR &= (~(1 << 13));
		delay_for(1000000);
	}
}