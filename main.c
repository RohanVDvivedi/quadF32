#include<regs/rcc.h>
#include<regs/gpio.h>

#include<uart/uart.h>

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void main(void)
{
	RCC->RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;

	GPIOC->GPIO_ODR |= (1 << 13);

	uart_init(9600);

	char c;
	while(c = uart_read_byte())
	{
		GPIOC->GPIO_ODR &= (~(1 << 13));

		char data[30] = "Hello World, you sent me X\n";
		data[25] = c;
		uart_write_blocking(data, 27);

		//uart_write_through_dma("Hello from DMA\n", 15);

		delay_for(500000);

		GPIOC->GPIO_ODR |= (1 << 13);
	}
}