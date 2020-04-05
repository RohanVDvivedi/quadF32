#ifndef EXTI_REGS_H
#define EXTI_REGS_H

#include<stdint.h>

typedef struct EXTI_t EXTI_t;
struct EXTI_t
{
	uint32_t EXTI_IMR;
	uint32_t EXTI_EMR;
	uint32_t EXTI_RTSR;
	uint32_t EXTI_FTSR;
	uint32_t EXTI_SWIER;
	uint32_t EXTI_PR;
};

#define EXTI ((volatile EXTI_t*)(0x40010400))

#endif