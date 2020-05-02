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

#define map(val, a_min, a_max, b_min, b_max)	b_min + ((((double)val) - a_min) * (b_max - b_min)) / (a_max - a_min)
#define insensitivity_limit(val, limit)			((val <= limit) && (val >= -limit)) ? 0 : val

#define max2(a, b)			((a > b) ? a : b)
#define min2(a, b)			((a < b) ? a : b)
#define max4(a, b, c, d)	max2(max2(a,b), max2(c,d))
#define min4(a, b, c, d)	min2(min2(a,b), min2(c,d))

#define PID_TO_TUNE_VAR y_rate_pid

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
	uart_init(2000000);

	// initialize rc receiver
	init_rc_receiver();

	// initialize motors
	init_bldc();
	set_motors(0, 0, 0, 0);

	// initialize all necessary sensors
	mpu_init();

	// initialize pid variables
	pid_state x_rate_pid; pid_init(&x_rate_pid, 0, 0, 0, 300);
	pid_state y_rate_pid; pid_init(&y_rate_pid, 0, 0, 0, 300);
	pid_state z_rate_pid; pid_init(&z_rate_pid, 0, 0, 0, 300);
	// flyable values
	/*
	pid_init(&x_rate_pid, 10, 0, 0, 300);
	pid_init(&y_rate_pid, 10, 0, 0, 300);
	pid_init(&z_rate_pid, 10, 0, 0, 300);
	*/
	// test bench empirical values
	/*
	pid_init(&x_rate_pid, 10, 0, 0, 300);
	pid_init(&y_rate_pid, 10, 0, 0, 300);
	pid_init(&z_rate_pid, 10, 0, 0, 300);
	*/
	// test
	/*
	pid_init(&x_rate_pid, 10, 0, 0, 300);
	pid_init(&y_rate_pid, 10, 0, 0, 300);
	pid_init(&z_rate_pid, 10, 0, 0, 300);
	*/

	uint64_t begin_micros;

	uint64_t print_iter = 0;
	char print_str[256];

	while(1)
	{
		begin_micros = get_now_micros();

		GPIOC->GPIO_ODR ^= (1 << 13);

		MPUdatascaled mpuData;
		get_scaled_MPUdata(&mpuData);

		uint32_t chan_ret[6];
		int is_rc_active = get_rc_channels(chan_ret);

		double throttle = chan_ret[3];
		double x_rc_req = map(chan_ret[5], 0.0, 1000.0, -20.0, 20.0);
		double y_rc_req = map(chan_ret[4], 0.0, 1000.0, -20.0, 20.0);
		double z_rc_req = map(chan_ret[2], 0.0, 1000.0, 20.0, -20.0);
		double aux1 = map(chan_ret[1], 0.0, 1000.0, 0.0, 1000.0);
		double aux2 = map(chan_ret[0], 0.0, 1000.0, 0.0, 1000.0);

		#if defined PID_TO_TUNE_VAR
			pid_update_constants(&PID_TO_TUNE_VAR, aux1, aux2, 0.0);
		#endif

		x_rc_req = insensitivity_limit(x_rc_req, 3.0);
		y_rc_req = insensitivity_limit(y_rc_req, 3.0);
		z_rc_req = insensitivity_limit(z_rc_req, 3.0);

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
			y_motor_corr = pid_update(&y_rate_pid, mpuData.gyro.yj, y_rc_req);
			z_motor_corr = pid_update(&z_rate_pid, mpuData.gyro.zk, z_rc_req);
		}

		double motor_LF = throttle + x_motor_corr - y_motor_corr + z_motor_corr;
		double motor_RF = throttle - x_motor_corr - y_motor_corr - z_motor_corr;
		double motor_LB = throttle + x_motor_corr + y_motor_corr - z_motor_corr;
		double motor_RB = throttle - x_motor_corr + y_motor_corr + z_motor_corr;

		double min_val = min4(motor_LF, motor_RF, motor_LB, motor_RB);
		double max_val = max4(motor_LF, motor_RF, motor_LB, motor_RB);

		if(min_val < 0.0)
		{
			double diff = 0.0 - min_val;
			motor_LF += diff;
			motor_RF += diff;
			motor_LB += diff;
			motor_RB += diff;
		}

		if(max_val > 1000.0)
		{
			double diff = max_val - 1000.0;
			motor_LF -= diff;
			motor_RF -= diff;
			motor_LB -= diff;
			motor_RB -= diff;
		}

		set_motors(((uint32_t)motor_LF), ((uint32_t)motor_RF), ((uint32_t)motor_LB), ((uint32_t)motor_RB));

		if(print_iter == 100)
		{
			print_iter = 0;
			char* end_ps = print_str;

			end_ps = stringify_integer(end_ps, is_rc_active); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_integer(end_ps, motor_LF); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_integer(end_ps, motor_RF); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_integer(end_ps, motor_LB); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_integer(end_ps, motor_RB); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_double(end_ps, aux1); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
			end_ps = stringify_double(end_ps, aux2); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

			*end_ps = '\n'; end_ps++;
			uart_write_through_dma(print_str, end_ps - print_str);
		}
		else
		{
			print_iter++;
		}

		delay_until_us(begin_micros + 2500);

		//delay_for_ms(100);
	}
}