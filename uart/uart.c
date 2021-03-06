#include<uart/uart.h>

char tx_buffer[32] = "Hello DMA init\n";

int uart_init(uint32_t baud_rate)
{
	RCC->RCC_APB2ENR |= ((1<<14) | (1<<2));
	RCC->RCC_AHBENR |= (1<<0);

	GPIOA->GPIO_CRH   &= 0xFFFFF00F;
	GPIOA->GPIO_CRH   |= 0x000004B0;

	USART1->USART_CR1 |= (1<<13);
	USART1->USART_BRR = ((uint32_t)get_sys_clock_frequency())/baud_rate;
	USART1->USART_CR3 |= (1<<7);
	USART1->USART_CR1 |= ((1<<3) | (1<<2));
}

void uart_write_byte(char byte)
{
	while(!(USART1->USART_SR & (1<<7))){}
	USART1->USART_DR = byte & 0xff;
}

char uart_read_byte()
{
	while(!(USART1->USART_SR & (1<<5))){}
	return USART1->USART_DR;
}

void uart_write_blocking(char* str, unsigned int bytes_to_write)
{
	unsigned int i = 0;
	while(i < bytes_to_write)
	{
		uart_write_byte(str[i++]);
	}
}

void uart_write_through_dma(char* str, unsigned int bytes_to_write)
{
	while(!(USART1->USART_SR & (1<<6))){}
	while(DMA1_CHANNEL(4).DMA_CNDTR){}

	DMA1_CHANNEL(4).DMA_CCR &= ~(1<<0);
	DMA1_CHANNEL(4).DMA_CCR = 0x3090;
	DMA1_CHANNEL(4).DMA_CPAR = (uint32_t)(&(USART1->USART_DR));
	DMA1_CHANNEL(4).DMA_CMAR = ((uint32_t)str);
	DMA1_CHANNEL(4).DMA_CNDTR = bytes_to_write;
	USART1->USART_SR &= ~(1<<6);
	DMA1_CHANNEL(4).DMA_CCR |= (1<<0);
}

int uart_read(char* str, unsigned int bytes_to_read)
{
	while(bytes_to_read > 0)
	{
		*str = uart_read_byte();
		str++;
		bytes_to_read--;
	}
}

int uart_destroy()
{
	USART1->USART_CR1 &= (~(1<<13));
}