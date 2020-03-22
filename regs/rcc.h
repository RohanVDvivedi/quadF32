#ifndef RCC_REGS_H
#define RCC_REGS_H

#include<stdint.h>

typedef struct RCC_t RCC_t;
struct RCC_t
{
	uint32_t RCC_CR;
	uint32_t RCC_CFGR;
	uint32_t RCC_CIR;
	uint32_t RCC_APB2RSTR;
	uint32_t RCC_APB1RSTR;
	uint32_t RCC_AHBENR;
	uint32_t RCC_APB2ENR;
	uint32_t RCC_APB1ENR;
	uint32_t RCC_BDCR;
	uint32_t RCC_CSR;
};

#define RCC ((volatile RCC_t*)(0x40021000))

#endif