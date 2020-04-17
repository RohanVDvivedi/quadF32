#include<delay/delay.h>
#include<d_serializer/d_serializer.h>

#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>

#include<gy86/gy86.h>
#include<bldc/quad_bldc.h>
#include<rc_receiver/rc_receiver.h>

void main(void)
{
	// setup clock to run at 72 MHz
	change_sys_clock_source(HSE, 8000000);
	setup_pll_module(HSE, 72000000);
	change_sys_clock_source(PLL, 72000000);

	// to enable port c GPIO
	RCC->RCC_APB2ENR |= (1<<4);

	// GPIO Port C pin 13 setup as output, it is the blink led on the bluepill board
	GPIOC->GPIO_CRH &= 0xFF0FFFFF;
	GPIOC->GPIO_CRH |= 0x00200000;
	GPIOC->GPIO_ODR |= (1 << 13);

	// init rtc clock
	rtc_init();
	i2c_init();
	uart_init(9600);

	mpu_init();

	uint8_t dev_addr = 0;
	char c_dev_addr[3]; c_dev_addr[2] = '\n';
	while(dev_addr < 0x7f)
	{
		delay_for_ms(10);
		if(i2c_detect(dev_addr))
		{
			stringify_8(c_dev_addr, dev_addr);
			uart_write_blocking(c_dev_addr, 3);
		}
		dev_addr++;
	}

	while(1)
	{
		GPIOC->GPIO_ODR ^= (1 << 13);

		MPUdatascaled mpuData;
		get_scaled_MPUdata(&mpuData);

		uint32_t x_acc = ((uint32_t)(mpuData.accl.zk));
		char c_x_acc[11];
		c_x_acc[10] = '\n';
		stringify_32(c_x_acc, x_acc);
		uart_write_blocking(c_x_acc, 11);

		delay_for_ms(1000);
	}
}