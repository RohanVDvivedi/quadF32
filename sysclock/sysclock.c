#include<sysclock/sysclock.h>

#include<regs/gpio.h>

static void delay_for(volatile int clocks)
{
	while(clocks--)
	{
		asm("nop");
	}
}

void turn_on_HSE_clk()
{
	RCC->RCC_CR |= (1<<16);
	while(!(RCC->RCC_CR & (1<<17))){}
}

void turn_on_HSI_clk()
{
	RCC->RCC_CR |= (1<<0);
	while(!(RCC->RCC_CR & (1<<1))){}
}

void turn_on_PLL_clk()
{
	RCC->RCC_CR |= (1<<24);
	while(!(RCC->RCC_CR & (1<<25))){}
}

void turn_off_HSE_clk()
{
	RCC->RCC_CR &= ~(1<<16);
	while(RCC->RCC_CR & (1<<17)){}
}

void turn_off_HSI_clk()
{
	RCC->RCC_CR &= ~(1<<0);
	while(RCC->RCC_CR & (1<<1)){}
}

void turn_off_PLL_clk()
{
	RCC->RCC_CR &= ~(1<<24);
	while(RCC->RCC_CR & (1<<25)){}
}

void change_clk(clk_source source)
{
	uint32_t new_clk_src = source & 0x3;
	RCC->RCC_CFGR = (RCC->RCC_CFGR & (~0x3)) | new_clk_src;
	while(((RCC->RCC_CFGR>>2) & 0x3) != new_clk_src){}
}

void change_sys_clock_source(clk_source source, uint32_t frequency)
{
	switch(source)
	{
		case HSI:
		{
			turn_on_HSI_clk();
			change_clk(HSI);
			break;
		}
		case HSE:
		{
			turn_on_HSE_clk();
			change_clk(HSE);
			break;
		}
		case PLL:
		{
			break;
		}
	}
	// HCLK, PCLK1 and PCLK2 prescalers
	if(frequency <= 36000000)
	{
		RCC->RCC_CFGR &= ~(0x7<<8);
		RCC->RCC_CFGR &= ~(0x7<<11);
		RCC->RCC_CFGR &= ~(0xf<<4);
	}
	else if(36000000 < frequency && frequency <= 72000000)
	{
		RCC->RCC_CFGR = (RCC->RCC_CFGR & ~(0x7<<8)) | (0x4<<8);
		RCC->RCC_CFGR &= ~(0x7<<11);
		RCC->RCC_CFGR &= ~(0xf<<4);
	}
}

clk_source get_sys_clock_source()
{
	return RCC->RCC_CFGR>>2;
}

uint32_t get_sys_clock_frequency();

uint32_t get_AHB_clock_frequency();

uint32_t get_APB2_clock_frequency();

uint32_t get_APB1_clock_frequency();