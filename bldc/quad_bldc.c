#include<bldc/quad_bldc.h>

void init_bldc()
{
	// using pins PA0,1,2,3 with Timer 2
	RCC->RCC_APB1ENR |= (1<<0);
	RCC->RCC_APB2ENR |= (1<<2);

	GPIOA->GPIO_CRL = (GPIOA->GPIO_CRL & ~(0xffff)) | 0xdddd;

	TIM2->TIM_CR1 = 0;
	TIM2->TIM_CR1 |= (1<<7);

	TIM2->TIM_SMCR = 0;

	// initialize counter value to 0
	TIM2->TIM_CNT = 0;
	// the APB1 clock is 36 MHz, we want to create 1 microsecond per timer tick
	TIM2->TIM_PSC = 36;
	// we want the signal frequency of output compare equal to 2500 microsecond only (400Hz bldc motor update) 
	TIM2->TIM_ARR = 2500;
	TIM2->TIM_RCR = 0;
	// initial values for the ocr registers, for all four motors
	TIM2->TIM_CCR1 = 1000;
	TIM2->TIM_CCR2 = 1000;
	TIM2->TIM_CCR3 = 1000;
	TIM2->TIM_CCR4 = 1000;

	TIM2->TIM_CCMR1 &= ~(0x0303);
	TIM2->TIM_CCMR2 &= ~(0x0303);

	TIM2->TIM_CCMR1 = 0x6868;
	TIM2->TIM_CCMR2 = 0x6868;

	TIM2->TIM_CCER = 0;
	TIM2->TIM_CCER |= ((1<<12) | (1<<8) | (1<<4) | (1<<0));

	TIM2->TIM_CR1 |= (1<<0);
}

static uint32_t compare_and_map_and_range(uint32_t value)
{
	value = value + 1000;
	if(value < 1000)
	{
		value = 1000;
	}
	else if(value > 2000)
	{
		value = 2000;
	}
	return value;
}

void set_motors(uint32_t m1, uint32_t m2, uint32_t m3, uint32_t m4)
{
	TIM2->TIM_CCR1 = compare_and_map_and_range(m1);
	TIM2->TIM_CCR2 = compare_and_map_and_range(m2);
	TIM2->TIM_CCR3 = compare_and_map_and_range(m3);
	TIM2->TIM_CCR4 = compare_and_map_and_range(m4);
}