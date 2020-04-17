#include<delay/delay.h>
#include<d_serializer/d_serializer.h>

#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>
#include<bldc/quad_bldc.h>
#include<rc_receiver/rc_receiver.h>

void main(void)
{
	change_sys_clock_source(HSE, 8000000);
	setup_pll_module(HSE, 72000000);
	change_sys_clock_source(PLL, 72000000);

	RCC->RCC_APB2ENR |= (1<<4);

	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;
	GPIOC->GPIO_ODR |= (1 << 13);

	rtc_init();

	while(1)
	{
		GPIOC->GPIO_ODR ^= (1 << 13);

		//delay_for_ms(1000);
		delay_for(1000000);
	}
}