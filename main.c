/*
	A simple blink program built for stm32f103 on a blue pill board or alike
*/

#include<stdint.h>

#include<uart/uart.h>

#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*((volatile uint32_t*)(RCC_BASE + 0x18)))

#define GPIOC_BASE 0x40011000
#define GPIOC_CRH (*((volatile uint32_t*)(GPIOC_BASE + 0x04)))
#define GPIOC_ODR (*((volatile uint32_t*)(GPIOC_BASE + 0x0c)))

#define GPIOA_BASE 0x40010800
#define GPIOA_CRH (*((volatile uint32_t*)(GPIOC_BASE + 0x04)))
#define GPIOA_ODR (*((volatile uint32_t*)(GPIOC_BASE + 0x0c)))

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void main(void)
{
	RCC_APB2ENR |= ((1<<14) | (1<<4) | (1<<2) | (1<<0));

	GPIOC_CRH   &= 0xFF0FFFFF;
	GPIOC_CRH   |= 0x00200000;

	GPIOA_CRH   &= 0xFFFFF00F;
	GPIOA_CRH   |= 0x000004d0;

	uart_init(57600);

	char c = 'X';

	while(c = uart_read_byte())
	{
		GPIOC_ODR |= (1 << 13);
		delay_for(1000000);
		GPIOC_ODR &= (~(1 << 13));
		delay_for(1000000);

		uart_write_byte('H');
		uart_write_byte('i');
		uart_write_byte(' ');
		uart_write_byte(c);
		uart_write_byte('\r');
		uart_write_byte('\n');
	}
}