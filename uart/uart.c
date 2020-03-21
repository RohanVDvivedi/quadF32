#include<uart/uart.h>

#define USART 			(volatile uint32_t*) 0x00

#define USART_SR		0x00
#define USART_DR		0x04
#define USART_BRR		0x08
#define USART_CR1		0x0c
#define USART_CR2		0x10
#define USART_CR3		0x14
#define USART_GTPR		0x18

int uart_init(uint32_t baud_rate)
{
	volatile uint32_t* USART_BRRx = USART + USART_BRR;
	volatile uint32_t* USART_CR1x = USART + USART_CR1;

	*USART_CR1x |= (1<<13);
	*USART_CR1x &= (~(1<<12));

	*USART_BRRx = 0x00;

	*USART_CR1x |= (1<<3);
	*USART_CR1x |= (1<<2);
}

void uart_write_byte(char byte)
{

}

char uart_read_byte()
{

}

int uart_write(char* str, unsigned int bytes_to_write)
{

}

int uart_read(char* str, unsigned int bytes_to_read)
{

}

int uart_destroy()
{
	volatile uint32_t* USART_CR1x = USART + USART_CR1;
	*USART_CR1x &= (~(1<<13));
}