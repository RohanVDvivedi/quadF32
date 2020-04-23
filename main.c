#include<delay/delay.h>
#include<d_serializer/d_serializer.h>

#include<regs/rcc.h>
#include<regs/gpio.h>

#include<sysclock/sysclock.h>
#include<uart/uart.h>
#include<i2c/i2c.h>
#include<backup_data/backup_data.h>

#include<pid/pid.h>

#include<gy86/gy86.h>
#include<bldc/quad_bldc.h>
#include<rc_receiver/rc_receiver.h>

#define map(val, a_min, a_max, b_min, b_max)	(((double)val) * (b_max - b_min)) / (a_max - a_min);

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

	// setup communications
	i2c_init();
	uart_init(9600);

	// initialize rc receiver
	init_rc_receiver();

	// initialize motors
	init_bldc();
	set_motors(0, 0, 0, 0);

	// without backup data pid can not be initialized
	backup_data_init();

	// initialize all necessary sensors
	mpu_init();

	// initialize pid variables
	pid_state x_rate_pid; pid_init(&x_rate_pid, 0, 0, 0, 300);
	pid_state y_rate_pid; pid_init(&y_rate_pid, 0, 0, 0, 300);
	pid_state z_rate_pid; pid_init(&z_rate_pid, 0, 0, 0, 300);


	while(1)
	{
		GPIOC->GPIO_ODR ^= (1 << 13);

		MPUdatascaled mpuData;
		get_scaled_MPUdata(&mpuData);

		uint32_t chan_ret[6];
		get_rc_channels(chan_ret);

		double throttle = chan_ret[4];
		double x_rc_req = map(chan_ret[6], 0.0, 1000.0, -30.0, 30.0);
		double y_rc_req = map(chan_ret[5], 0.0, 1000.0, -30.0, 30.0);
		double z_rc_req = map(chan_ret[3], 0.0, 1000.0, -30.0, 30.0);

		double x_motor_corr = 0;
		double y_motor_corr = 0;
		double z_motor_corr = 0;

		if(throttle < 200)
		{
			pid_reinit(&x_rate_pid);
			pid_reinit(&y_rate_pid);
			pid_reinit(&z_rate_pid);
		}
		else
		{
			x_motor_corr = pid_update(&x_rate_pid, mpuData.gyro.xi, x_rc_req);
			y_motor_corr = pid_update(&y_rate_pid, mpuData.gyro.xi, y_rc_req);
			z_motor_corr = pid_update(&z_rate_pid, mpuData.gyro.xi, z_rc_req);
		}

		uint32_t motor_LF = throttle + x_motor_corr - y_motor_corr + z_motor_corr;
		uint32_t motor_RF = throttle - x_motor_corr - y_motor_corr - z_motor_corr;
		uint32_t motor_LB = throttle + x_motor_corr + y_motor_corr - z_motor_corr;
		uint32_t motor_RB = throttle - x_motor_corr + y_motor_corr + z_motor_corr;

		set_motors(motor_LF, motor_LF, motor_LF, motor_LF);

		delay_for_ms(1000);
	}
}