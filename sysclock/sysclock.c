#include<sysclock/sysclock.h>

void change_sys_clock_source(clk_source source, uint32_t frequency)
{
	if(frequency >= 72000000)
	{
		frequency = 72000000;
	}
	else if(frequency < (2 * HSI_CLOCK) && source == HSI_WITH_PLL)
	{
		source = HSI;
	}
	else if(frequency <= (2 * HSE_CLOCK) && source == HSE_WITH_PLL)
	{
		source = HSE;
	}
	if(source == HSE)
	{
		frequency = HSE_CLOCK;
	}
	else if(source == HSI)
	{
		frequency = HSI_CLOCK;
	}
	switch(source)
	{
		case HSI:
		{
			RCC->RCC_CR |= (1<<0);
			while(!(RCC->RCC_CR & (1<<1))){}

			RCC->RCC_CFGR = (RCC->RCC_CFGR & ~(0x7<<8)) | (1<<10);
			RCC->RCC_CFGR &= ~(0x7<<11);
			RCC->RCC_CFGR &= ~(0xf<<4);

			RCC->RCC_CFGR &= (~0x3);
			while((RCC->RCC_CFGR & 0xc)==0x0){}
			break;
		}
		case HSI_WITH_PLL:
		{
			RCC->RCC_CR |= (1<<0);
			while(!(RCC->RCC_CR & (1<<1))){}
			RCC->RCC_CFGR &= ~(1<<16);
			uint32_t multiplication_factor = frequency / (HSI_CLOCK/2);
			RCC->RCC_CFGR &= ~(0xf<<18); 
			RCC->RCC_CFGR |= (multiplication_factor - 2)<<18;
			RCC->RCC_CR |= (1<<24);
			while(!(RCC->RCC_CR & (1<<25))){}

			RCC->RCC_CFGR = (RCC->RCC_CFGR & ~(0x7<<8)) | (1<<10);
			RCC->RCC_CFGR &= ~(0x7<<11);
			RCC->RCC_CFGR &= ~(0xf<<4);

			RCC->RCC_CFGR = (RCC->RCC_CFGR | (1<<1)) & (~(1<<0));
			while((RCC->RCC_CFGR & 0xc)==0x8){}
			break;
		}
		case HSE:
		{
			RCC->RCC_CR |= (1<<16);
			while(!(RCC->RCC_CR & (1<<17))){}

			RCC->RCC_CFGR = (RCC->RCC_CFGR & ~(0x7<<8)) | (1<<10);
			RCC->RCC_CFGR &= ~(0x7<<11);
			RCC->RCC_CFGR &= ~(0xf<<4);

			RCC->RCC_CFGR = (RCC->RCC_CFGR | (1<<0)) & (~(1<<1));
			while((RCC->RCC_CFGR & 0xc)==0x4){}
			break;
		}
		case HSE_WITH_PLL:
		{
			RCC->RCC_CR |= (1<<16);
			while(!(RCC->RCC_CR & (1<<17))){}
			RCC->RCC_CFGR &= ~(1<<17);
			RCC->RCC_CFGR |= (1<<16);
			uint32_t multiplication_factor = frequency / HSE_CLOCK;
			RCC->RCC_CFGR &= ~(0xf<<18); 
			RCC->RCC_CFGR |= (multiplication_factor - 2)<<18;
			RCC->RCC_CR |= (1<<24);
			while(!(RCC->RCC_CR & (1<<25))){}

			RCC->RCC_CFGR = (RCC->RCC_CFGR & ~(0x7<<8)) | (1<<10);
			RCC->RCC_CFGR &= ~(0x7<<11);
			RCC->RCC_CFGR &= ~(0xf<<4);

			RCC->RCC_CFGR = (RCC->RCC_CFGR | (1<<1)) & (~(1<<0));
			while((RCC->RCC_CFGR & 0xc)==0x8){}
			break;
		}
	}
}

clk_source get_sys_clock_source()
{
	int source = (RCC->RCC_CFGR & 0xc)>>2;
	if(source<2)
	{
		return source;
	}
	else
	{
		return ((RCC->RCC_CFGR>>16) & 0x1) | 0x2;
	}
}

uint32_t get_sys_clock_frequency();

uint32_t get_AHB_clock_frequency();

uint32_t get_APB2_clock_frequency();

uint32_t get_APB1_clock_frequency();