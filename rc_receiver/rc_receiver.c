#include<rc_receiver/rc_receiver.h>

void init_rc_receiver()
{
	// using pins PA6,7 and PB0,1 with Timer 3
	RCC->RCC_APB1ENR |= (1<<1);
	RCC->RCC_APB2ENR |= ((1<<3) | (1<<2));

	GPIOA->GPIO_CRL = (GPIOA->GPIO_CRL & ~(0xff<<24)) | 0xbb;
	GPIOB->GPIO_CRL = (GPIOB->GPIO_CRL & ~(0xff)) | 0xbb;

	//TODO :: setup timer counters appropriately, as input capture

	TIM3->TIM_CR1 |= (1<<0);
}

static uint32_t compare_and_map_and_range(uint32_t value)
{
	if(value < 1000)
	{
		value = 1000;
	}
	else if(value > 2000)
	{
		value = 2000;
	}
	value = value - 1000;
	return value;
}

void get_rc_channels(uint32_t* c1, uint32_t* c2, uint32_t* c3, uint32_t* c4)
{
	(*c1) = compare_and_map_and_range(TIM2->TIM_CCR1);
	(*c2) = compare_and_map_and_range(TIM2->TIM_CCR2);
	(*c3) = compare_and_map_and_range(TIM2->TIM_CCR3);
	(*c4) = compare_and_map_and_range(TIM2->TIM_CCR4);
}