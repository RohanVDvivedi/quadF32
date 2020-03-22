#include<uart/uart.h>

#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*((volatile uint32_t*)(RCC_BASE + 0x18)))

int uart_init(uint32_t baud_rate)
{
	RCC_APB2ENR |= ((1<<14) | (1<<2));

	GPIOA->GPIO_CRH   &= 0xFFFFF00F;
	GPIOA->GPIO_CRH   |= 0x000004B0;

	double USART_DIV = 72000000.0/(16.0 * baud_rate);
	uint32_t BRR_val = (((uint32_t)USART_DIV) << 4) + ((uint32_t)((USART_DIV-((double)((uint32_t)USART_DIV))) * 16));

	USART1->USART_BRR = 8000000/38400;
	USART1->USART_CR1 |= ((1<<13) | (1<<3) | (1<<2));
}

void uart_write_byte(char byte)
{
	USART1->USART_DR = byte & 0xff;
	while(!(USART1->USART_SR & (1<<7))){}
}

char uart_read_byte()
{
	while(!(USART1->USART_SR & (1<<5))){}
	return USART1->USART_DR;
}

int uart_write(char* str, unsigned int bytes_to_write)
{
}

int uart_read(char* str, unsigned int bytes_to_read)
{
}

int uart_destroy()
{
	USART1->USART_CR1 &= (~(1<<13));
}