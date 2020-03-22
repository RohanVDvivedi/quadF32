#ifndef GPIO_REGS_H
#define GPIO_REGS_H

#include<stdint.h>

typedef struct GPIO_t GPIO_t;
struct GPIO_t
{
	uint32_t GPIO_CRL;
	uint32_t GPIO_CRH;
	uint32_t GPIO_IDR;
	uint32_t GPIO_ODR;
	uint32_t GPIO_BSRR;
	uint32_t GPIO_BRR;
	uint32_t GPIO_LCKR;
};

#define GPIOA ((volatile GPIO_t*)(0x40010800))
#define GPIOB ((volatile GPIO_t*)(0x40010C00))
#define GPIOC ((volatile GPIO_t*)(0x40011000))
#define GPIOD ((volatile GPIO_t*)(0x40011400))
#define GPIOE ((volatile GPIO_t*)(0x40011800))
#define GPIOF ((volatile GPIO_t*)(0x40011C00))
#define GPIOG ((volatile GPIO_t*)(0x40012000))

#endif