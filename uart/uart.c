#include<uart/uart.h>

int uart_init(uint32_t baud_rate)
{
	RCC->RCC_APB2ENR |= ((1<<14) | (1<<2));

	GPIOA->GPIO_CRH   &= 0xFFFFF00F;
	GPIOA->GPIO_CRH   |= 0x000004B0;

	USART1->USART_BRR = 8000000/baud_rate;
	
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