#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
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
	change_sys_clock_source(HSE, 8000000);
	setup_pll_module(HSE, 72000000);
	change_sys_clock_source(PLL, 72000000);

	RCC->RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;
	GPIOC->GPIO_ODR |= (1 << 13);

	uart_init(9600);

	while(1)
	{
		//char c = uart_read_byte();

		uint32_t clock_source = get_sys_clock_source() + 1;
		while(clock_source > 0){
		GPIOC->GPIO_ODR &= (~(1 << 13));

		/*char data[30] = "Hello World, you sent me X\n";
		data[25] = c;

		if('A' <= c && c <= 'Z')
		{
			uart_write_blocking(data, 27);
		}
		else if('a' <= c && c <= 'z')
		{
			uart_write_through_dma(data, 27);
		}
		else
		{
			data[25] = '0' + (((char)get_sys_clock_source())&0xff);
			uart_write_blocking(data, 27);
			uart_write_through_dma(data, 27);
		}*/

		delay_for(500000);

		GPIOC->GPIO_ODR |= (1 << 13);

		delay_for(500000);
		clock_source--;
		}

		delay_for(500000 * 5);
	}
}