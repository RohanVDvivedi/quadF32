#ifndef USART_REGS_H
#define USART_REGS_H

#include<stdint.h>

typedef struct USART_t USART_t;
struct USART_t
{
	uint32_t USART_SR;
	uint32_t USART_DR;
	uint32_t USART_BRR;
	uint32_t USART_CR1;
	uint32_t USART_CR2;
	uint32_t USART_CR3;
	uint32_t USART_GTPR;
};

#define USART1 ((volatile USART_t*)(0x40013800))
#define USART2 ((volatile USART_t*)(0x40004400))
#define USART3 ((volatile USART_t*)(0x40004800))
#define USART4 ((volatile USART_t*)(0x40004C00))
#define USART5 ((volatile USART_t*)(0x40005000))

#endif