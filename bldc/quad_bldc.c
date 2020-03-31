#include<bldc/quad_bldc.h>

void init_bldc()
{
	RCC->RCC_APB2ENR |= (1<<11);
}

void set_motors(uint32_t m1, uint32_t m2, uint32_t m3, uint32_t m4)
{

}