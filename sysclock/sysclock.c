#include<sysclock/sysclock.h>

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

void setup_pll_module(clk_source source, uint32_t frequency)
{
	if(frequency > 72000000)
	{
		frequency = 72000000;
	}
	turn_off_PLL_clk();
	switch(source)
	{
		case HSE :
		{
			RCC->RCC_CFGR &= ~(1<<17);
			RCC->RCC_CFGR |= (1<<16);
			uint32_t multiplication_factor_field = (frequency/HSE_CLOCK)-2;
			if(multiplication_factor_field > 0xe)
			{
				multiplication_factor_field = 0xe;
			}
			RCC->RCC_CFGR &= ~(0xf<<18);
			RCC->RCC_CFGR |= (multiplication_factor_field<<18);
			break;
		}
		case HSI :
		{
			RCC->RCC_CFGR &= ~(1<<17);
			RCC->RCC_CFGR &= ~(1<<16);
			uint32_t multiplication_factor_field = (frequency/(HSI_CLOCK/2))-2;
			if(multiplication_factor_field > 0xe)
			{
				multiplication_factor_field = 0xe;
			}
			RCC->RCC_CFGR &= ~(0xf<<18);
			RCC->RCC_CFGR |= (multiplication_factor_field<<18);
			break;
		}
		default :
		{
			break;
		}
	}
}

void change_clk(clk_source source)
{
	uint32_t new_clk_src = source & 0x3;
	RCC->RCC_CFGR = (RCC->RCC_CFGR & (~0x3)) | new_clk_src;
	while(((RCC->RCC_CFGR>>2) & 0x3) != new_clk_src){}
}

void change_sys_clock_source(clk_source source, uint32_t frequency)
{
	if(frequency > 72000000)
	{
		frequency = 72000000;
	}

	// increse the wait states to 2, so that we may not be reading false data from Flash memory
	FLASH_ACR = (FLASH_ACR & ~(0x7)) | 0x2;

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
			turn_on_PLL_clk();
			change_clk(PLL);
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

	// setup flash according to the new frequency
	if(frequency <= 24000000)
	{
		if(!(RCC->RCC_CFGR & (1<<13)))
		{
			FLASH_ACR |= (1<<4);
		}
		FLASH_ACR &= ~(0x7);
	}
	else if(24000000 < frequency && frequency <= 48000000)
	{
		FLASH_ACR = (FLASH_ACR & ~(0x7)) | 0x1;
	}
	else if(48000000 < frequency && frequency <= 72000000)
	{
		FLASH_ACR = (FLASH_ACR & ~(0x7)) | 0x2;
	}
}

clk_source get_sys_clock_source()
{
	return (RCC->RCC_CFGR>>2) & 0x3;
}

uint32_t get_sys_clock_frequency()
{
	switch(get_sys_clock_source())
	{
		case HSI :
		{
			return HSI_CLOCK;
		}
		case HSE :
		{
			return HSE_CLOCK;
		}
		case PLL :
		{
			uint32_t multiplication_factor = ((RCC->RCC_CFGR>>18) & 0xf);
			if(multiplication_factor == 0xf)
			{
				multiplication_factor = 0xe;
			}
			multiplication_factor += 2;
			if(!(RCC->RCC_CFGR & (1<<16)))
			{
				return (HSI_CLOCK/2) * multiplication_factor;
			}
			else
			{
				uint32_t HSE_PLL_INPUT_FREQUENCY = HSE_CLOCK;
				if(RCC->RCC_CFGR & (1<<17))
				{
					HSE_PLL_INPUT_FREQUENCY /= 2;
				}
				return HSE_PLL_INPUT_FREQUENCY * multiplication_factor;
			}
		}
	}
}

uint32_t get_AHB_clock_frequency();

uint32_t get_APB2_clock_frequency();

uint32_t get_APB1_clock_frequency();