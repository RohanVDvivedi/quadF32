#ifndef GPIO_H
#define GPIO_H

#include<stdint.h>

typedef struct GPIO GPIO;
struct GPIO
{
	uint32_t GPIO_CRL;
	uint32_t GPIO_CRH;
	uint32_t GPIO_IDR;
	uint32_t GPIO_ODR;
	uint32_t GPIO_BSRR;
	uint32_t GPIO_BRR;
	uint32_t GPIO_LCKR;
};

#define GPIOA (*((volatile GPIO*)(0x40010800)))
#define GPIOB (*((volatile GPIO*)(0x40010C00)))
#define GPIOC (*((volatile GPIO*)(0x40011000)))
#define GPIOD (*((volatile GPIO*)(0x40011400)))
#define GPIOE (*((volatile GPIO*)(0x40011800)))
#define GPIOG (*((volatile GPIO*)(0x40011C00)))
#define GPIOG (*((volatile GPIO*)(0x40012000)))

#endif