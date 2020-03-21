#include<uart/uart.h>

#define USART 			(volatile uint32_t*) 0x40013800

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
	volatile uint32_t* USART_CR2x = USART + USART_CR2;
	volatile uint32_t* USART_CR3x = USART + USART_CR3;

	*USART_CR1x &= (~(1<<12));

	double USART_DIV = 72000000.0/(16.0 * baud_rate);
	uint32_t BRR_val = (((uint32_t)USART_DIV) << 4) + ((uint32_t)((USART_DIV-((double)((uint32_t)USART_DIV))) * 16));

	*USART_BRRx = BRR_val;

	*USART_CR1x |= ((1<<13) | (1<<3) | (1<<2));
}

void uart_write_byte(char byte)
{
	volatile uint32_t* USART_SRx = USART + USART_SR;
	volatile uint32_t* USART_DRx = USART + USART_DR;
	(*USART_DRx) = byte;
	while(((*USART_SRx) & (1<<7))==0){}
}

char uart_read_byte()
{
	volatile uint32_t* USART_SRx = USART + USART_SR;
	volatile uint32_t* USART_DRx = USART + USART_DR;
	int i = 0;
	while(((*USART_SRx) & (1<<5))==0 && i < 1000000){i++;}
	return i==1000000 ? 'X' : ((*USART_DRx) & 0xff);
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