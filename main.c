#include<delay/delay.h>
#include<d_serializer/d_serializer.h>

#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>
#include<backup_data/backup_data.h>

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

	//mpu_init();

	backup_data_init();

	while(1)
	{
		GPIOC->GPIO_ODR ^= (1 << 13);

		/*MPUdatascaled mpuData;
		get_scaled_MPUdata(&mpuData);

		char c_sensed[128];
		char* end = c_sensed;
		end = stringify_double(end, mpuData.accl.xi);*end = '\t';end++;*end = '\t';end++;
		end = stringify_double(end, mpuData.accl.yj);*end = '\t';end++;*end = '\t';end++;
		end = stringify_double(end, mpuData.accl.zk);*end = '\n';end++;
		uart_write_blocking(c_sensed, end - c_sensed);*/

		char op;
		char c_addr[2];
		char c_resp[128];

		uart_read(&op, 1);
		uart_write_blocking(&op, 1);
		uart_write_blocking("\n", 1);

		uart_read(c_addr, 2);
		uart_write_blocking(c_addr, 2);
		int addr = numify_integer(c_addr);

		switch(op)
		{
			case 'w' :
			{
				char c_data_w[3];
				char c_data_f[3];
				uart_read(c_data_w, 3);
				uart_read(c_data_f, 3);
				double data = ((double)(numify_integer(c_data_w))) + ((double)(numify_integer(c_data_f))) / 1000;
				
				char c_resp[128];
				char* end = c_resp;
				end = stringify_double(end, data);*end = '\n';end++;
				uart_write_blocking(c_resp, end - c_resp);

				write_backup_data(addr, data);

				uart_write_blocking("done\n", 5);
				break;
			}
			case 'r' :
			{
				uart_write_blocking(" => ", 4);

				char c_resp[128];
				char* end = c_resp;
				end = stringify_double(end, read_backup_data(addr));*end = '\n';end++;
				uart_write_blocking(c_resp, end - c_resp);
				break;
			}
			default :
			{
				uart_write_blocking("default operation\n", 18);
				break;
			}
		}

		//delay_for_ms(1000);
	}
}