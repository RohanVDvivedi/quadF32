#ifndef ADVANCED_TIMER_H
#define ADVANCED_TIMER_H

typedef struct ADV_TIM_t ADV_TIM_t;
struct ADV_TIM_t
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

#endif