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

void stringify_32(char* num, uint32_t n)
{
	int j = 0; int i = 0;
	num[j++] = '0';
	num[j++] = 'x';
	for(i = 1; i <= 8; i++, j++)
	{
		num[j] = (char)((n>>((8-i)*4))&0x0f);
		if(num[j] <= 0x09)
		{
			num[j] += '0';
		}
		else if(num[j] <= 0x0f)
		{
			num[j] -= 0x0a;
			num[j] += 'A';
		}
	}
}

uint32_t r = 0;

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

	char details[30] = "r => 0x********\n";
	stringify_32(details + 5, r);
	uart_write_blocking(details, 16);

	while(1)
	{
		//char c = uart_read_byte();

		GPIOC->GPIO_ODR &= (~(1 << 13));

		/*if(c == 'c')
		{
			char clock_details[30] = "X => 0x********\n";

			clock_details[0] = '0' + (((char)get_sys_clock_source())&0xff);
			stringify_32(clock_details + 5, get_sys_clock_frequency());

			uart_write_blocking(clock_details, 16);
			uart_write_through_dma(clock_details, 16);
		}
		else
		{
			char data[30] = "Hello World, you sent me X\n";
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
				uart_write_blocking(data, 27);
				uart_write_through_dma(data, 27);
			}
		}*/

		delay_for(500000);

		GPIOC->GPIO_ODR |= (1 << 13);

		delay_for(500000);
	}
}