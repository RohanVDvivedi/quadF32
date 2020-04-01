#ifndef TIMER_REGS_H
#define TIMER_REGS_H

#include<stdint.h>

typedef struct TIMER_t TIMER_t;
struct TIMER_t
{
	uint32_t TIM_CR1;
	uint32_t TIM_CR2;
	uint32_t TIM_SMCR;
	uint32_t TIM_DIER;
	uint32_t TIM_SR;
	uint32_t TIM_EGR;
	uint32_t TIM_CCMR1;
	uint32_t TIM_CCMR2;
	uint32_t TIM_CCER;
	uint32_t TIM_CNT;
	uint32_t TIM_PSC;
	uint32_t TIM_ARR;
	uint32_t TIM_RCR;
	uint32_t TIM_CCR1;
	uint32_t TIM_CCR2;
	uint32_t TIM_CCR3;
	uint32_t TIM_CCR4;
	uint32_t TIM_BDTR;
	uint32_t TIM_DCR;
	uint32_t TIM_DMAR;
};

#define TIM1 ((volatile TIMER_t*)(0x40012C00))
#define TIM2 ((volatile TIMER_t*)(0x40000000))
#define TIM3 ((volatile TIMER_t*)(0x40000400))
#define TIM4 ((volatile TIMER_t*)(0x40000800))
#define TIM5 ((volatile TIMER_t*)(0x40000C00))
#define TIM6 ((volatile TIMER_t*)(0x40001000))
#define TIM7 ((volatile TIMER_t*)(0x40001400))
#define TIM8 ((volatile TIMER_t*)(0x40013400))
#define TIM9 ((volatile TIMER_t*)(0x40014C00))
#define TIM10 ((volatile TIMER_t*)(0x40015000))
#define TIM11 ((volatile TIMER_t*)(0x40015400))
#define TIM12 ((volatile TIMER_t*)(0x40001800))
#define TIM13 ((volatile TIMER_t*)(0x40001C00))
#define TIM14 ((volatile TIMER_t*)(0x40002000))

#endif