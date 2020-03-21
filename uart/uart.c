#include<uart/uart.h>

#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*((volatile uint32_t*)(RCC_BASE + 0x18)))

#define GPIOA_BASE 0x40010800
#define GPIOA_CRH (*((volatile uint32_t*)(GPIOA_BASE + 0x04)))
#define GPIOA_ODR (*((volatile uint32_t*)(GPIOA_BASE + 0x0c)))

#define USART 			0x40013800

#define USART_SR		0x00
#define USART_DR		0x04
#define USART_BRR		0x08
#define USART_CR1		0x0c
#define USART_CR2		0x10
#define USART_CR3		0x14
#define USART_GTPR		0x18

int uart_init(uint32_t baud_rate)
{
	volatile uint32_t* USART_BRRx = (volatile uint32_t*)(USART + USART_BRR);
	volatile uint32_t* USART_CR1x = (volatile uint32_t*)(USART + USART_CR1);

	RCC_APB2ENR |= ((1<<14) | (1<<2));

	GPIOA_CRH   &= 0xFFFFF00F;
	GPIOA_CRH   |= 0x000004B0;

	double USART_DIV = 72000000.0/(16.0 * baud_rate);
	uint32_t BRR_val = (((uint32_t)USART_DIV) << 4) + ((uint32_t)((USART_DIV-((double)((uint32_t)USART_DIV))) * 16));

	*USART_BRRx = 8000000/38400;

	*USART_CR1x |= ((1<<13) | (1<<3) | (1<<2));
}

void uart_write_byte(char byte)
{
	volatile uint32_t* USART_SRx = (volatile uint32_t*)(USART + USART_SR);
	volatile uint32_t* USART_DRx = (volatile uint32_t*)(USART + USART_DR);
	(*USART_DRx) = byte & 0xff;
	while(((*USART_SRx) & (1<<7))==0){}
}

char uart_read_byte()
{
	volatile uint32_t* USART_SRx = (volatile uint32_t*)(USART + USART_SR);
	volatile uint32_t* USART_DRx = (volatile uint32_t*)(USART + USART_DR);
	int i = 0;
	while(((*USART_SRx) & (1<<5))==0){}
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
	volatile uint32_t* USART_CR1x = (volatile uint32_t*)(USART + USART_CR1);
	*USART_CR1x &= (~(1<<13));
}