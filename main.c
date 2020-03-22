#include<stdint.h>

#include<regs/gpio.h>


#include<uart/uart.h>

#define RCC_BASE 0x40021000
#define RCC_CR (*((volatile uint32_t*)(RCC_BASE + 0x00)))
#define RCC_CFGR (*((volatile uint32_t*)(RCC_BASE + 0x04)))
#define RCC_APB2ENR (*((volatile uint32_t*)(RCC_BASE + 0x18)))

void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

#define FLASH_ACR (*(volatile uint32_t*)(0x40022000))

void main(void)
{
	/*
	// enable HSE
    RCC_CR = RCC_CR | 0x00010001;
    while ((RCC_CR & 0x00020000) == 0); // for it to come on 
    // enable flash prefetch buffer 
    FLASH_ACR = 0x00000012;
    // Configure PLL 
    RCC_CFGR = RCC_CFGR | 0x001D0400; // pll=72Mhz,APB1=36Mhz,AHB=72Mhz 
    RCC_CR = RCC_CR | 0x01000000; // enable the pll 
    while ((RCC_CR & 0x03000000) == 0);         // wait for it to come on 
    // Set SYSCLK as PLL 
    RCC_CFGR = RCC_CFGR | 0x00000002;
    while ((RCC_CFGR & 0x00000008) == 0); // wait for it to come on 
    */


	RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;

	uart_init(9600);

	char c = 'X';
	while(c = uart_read_byte())
	{
		GPIOC->GPIO_ODR |= (1 << 13);
		delay_for(1000000);
		GPIOC->GPIO_ODR &= (~(1 << 13));
		delay_for(1000000);

		uart_write_byte('H');
		uart_write_byte('i');
		uart_write_byte(' ');
		uart_write_byte(c);
		uart_write_byte('\r');
		uart_write_byte('\n');
	}
}