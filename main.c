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

#define map(val, a_min, a_max, b_min, b_max)	b_min + ((((float)val) - a_min) * (b_max - b_min)) / (a_max - a_min)
#define insensitivity_limit(val, limit)			((val <= limit) && (val >= -limit)) ? 0 : val

#define GYRO_ACCL_MIX     0.98
#define LOOP_EVERY_MICROS 2500

//#define CALIBRATE_ESC_ON_START_UP

//#define DEBUG_OVER_UART
//#define PID_TO_TUNE

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
	#if defined CALIBRATE_ESC_ON_START_UP
		set_motors(1000, 1000, 1000, 1000);
		delay_for_ms(3000);
	#endif
	set_motors(0, 0, 0, 0);

	delay_for_ms(1000);

	#if defined DEBUG_OVER_UART
		if(i2c_detect(0x68))
			uart_write_blocking("MPU detected\n", 13);
		else
			uart_write_blocking("MPU not detected\n", 17);
	#endif

	// initialize all necessary sensors
	const MPUdatascaled* mpuInit = mpu_init();

	// initialize pid variables
	pid_state x_rate_pid; pid_init(&x_rate_pid, 1.3, 0.000625, 0, 400);
	pid_state y_rate_pid; pid_init(&y_rate_pid, 1.3, 0.000625, 0, 400);
	pid_state z_rate_pid; pid_init(&z_rate_pid, 0, 0, 0, 400);
	// as tested several times, Kp must not exceed 3.5 even value of 3 gives controller saturation
	// flyable values
	/*
	pid_state x_rate_pid; pid_init(&x_rate_pid, 1.3, 0.000625, 0, 400);
	pid_state y_rate_pid; pid_init(&y_rate_pid, 1.3, 0.000625, 0, 400);
	pid_state z_rate_pid; pid_init(&z_rate_pid, 1.8, 0.0005, 0.25, 400);
	*/

	uint64_t begin_micros = get_now_micros() - LOOP_EVERY_MICROS;

	uint64_t print_iter = 0;
	char print_str[256];

	float abs_roll = 0;
	float abs_pitch = 0;

	MPUdatascaled mpuData = *mpuInit;

	while(1)
	{
		float time_delta_in_seconds = ((float)(get_now_micros() - begin_micros))/1000000.0;
		begin_micros = get_now_micros();

		GPIOC->GPIO_ODR ^= (1 << 13);

		MPUdatascaled mpuTemp;
		get_scaled_MPUdata(&mpuTemp);
		mpuData.accl = mpuTemp.accl;
		multiply_scalar(&(mpuTemp.gyro), &(mpuTemp.gyro), 0.3);
		multiply_scalar(&(mpuData.gyro), &(mpuData.gyro), 0.7);
		sum(&(mpuData.gyro), &(mpuData.gyro), &(mpuTemp.gyro));

		abs_roll  = (abs_roll  + mpuData.gyro.xi * time_delta_in_seconds) * (GYRO_ACCL_MIX)
		+ ((atanf( mpuData.accl.yj/mpuData.accl.zk) - atanf( mpuInit->accl.yj/mpuInit->accl.zk)) * 180 / M_PI) * (1.0 - GYRO_ACCL_MIX);
		abs_pitch = (abs_pitch + mpuData.gyro.yj * time_delta_in_seconds) * (GYRO_ACCL_MIX)
		+ ((atanf(-mpuData.accl.xi/mpuData.accl.zk) - atanf(-mpuInit->accl.xi/mpuInit->accl.zk)) * 180 / M_PI) * (1.0 - GYRO_ACCL_MIX);

		uint32_t chan_ret[6];
		int is_rc_active = get_rc_channels(chan_ret);

		float throttle = chan_ret[3];
		float x_rc_req = map(chan_ret[5], 0.0, 1000.0, -20.0, 20.0);
		float y_rc_req = map(chan_ret[4], 0.0, 1000.0, -20.0, 20.0);
		float z_rc_req = map(chan_ret[2], 0.0, 1000.0, 20.0, -20.0);
			chan_ret[1] = (chan_ret[1] < 3) ? 0 : chan_ret[1];
		float aux1 = map(chan_ret[1], 0.0, 1000.0, 0.0, 3);
			chan_ret[0] = (chan_ret[0] < 3) ? 0 : chan_ret[0];
		float aux2 = map(chan_ret[0], 0.0, 1000.0, 0.0, 0.005);

		#if defined PID_TO_TUNE
			pid_update_constants(&x_rate_pid, aux1, aux2, 0);
			pid_update_constants(&y_rate_pid, aux1, aux2, 0);
		#endif

		x_rc_req = insensitivity_limit(x_rc_req, 3.0);
		y_rc_req = insensitivity_limit(y_rc_req, 3.0);
		z_rc_req = insensitivity_limit(z_rc_req, 3.0);

		float x_rate_req = 2 * (x_rc_req -  abs_roll);
		float y_rate_req = 2 * (y_rc_req - abs_pitch);
		float z_rate_req = z_rc_req;

		float x_motor_corr = 0;
		float y_motor_corr = 0;
		float z_motor_corr = 0;

		if(throttle < 60)
		{
			pid_reinit(&x_rate_pid);
			pid_reinit(&y_rate_pid);
			pid_reinit(&z_rate_pid);
		}
		else
		{
			x_motor_corr = pid_update(&x_rate_pid, mpuData.gyro.xi, x_rate_req);
			y_motor_corr = pid_update(&y_rate_pid, mpuData.gyro.yj, y_rate_req);
			z_motor_corr = pid_update(&z_rate_pid, mpuData.gyro.zk, z_rate_req);
		}

		float motor_LF = throttle + x_motor_corr - y_motor_corr - z_motor_corr;
		float motor_RF = throttle - x_motor_corr - y_motor_corr + z_motor_corr;
		float motor_LB = throttle + x_motor_corr + y_motor_corr + z_motor_corr;
		float motor_RB = throttle - x_motor_corr + y_motor_corr - z_motor_corr;

		float min_val = fminf(fminf(motor_LF, motor_RF), fminf(motor_LB, motor_RB));
		float max_val = fmaxf(fmaxf(motor_LF, motor_RF), fmaxf(motor_LB, motor_RB));

		if(min_val < 0.0)
		{
			float diff = 0.0 - min_val;
			motor_LF += diff;
			motor_RF += diff;
			motor_LB += diff;
			motor_RB += diff;
		}

		if(max_val > 1000.0)
		{
			float diff = max_val - 1000.0;
			motor_LF -= diff;
			motor_RF -= diff;
			motor_LB -= diff;
			motor_RB -= diff;
		}

		set_motors(((uint32_t)motor_LF), ((uint32_t)motor_RF), ((uint32_t)motor_LB), ((uint32_t)motor_RB));

		if(print_iter == 100)
		{
			print_iter = 0;

			#if defined DEBUG_OVER_UART
				char* end_ps = print_str;

				//end_ps = stringify_integer(end_ps, is_rc_active); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_integer(end_ps, motor_LF); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_integer(end_ps, motor_RF); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_integer(end_ps, motor_LB); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_integer(end_ps, motor_RB); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				end_ps = stringify_float(end_ps, aux1); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				end_ps = stringify_float(end_ps, aux2); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, mpuData.gyro.xi); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuData.gyro.yj); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuData.gyro.zk); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, mpuInit->gyro.xi); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuInit->gyro.yj); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuInit->gyro.zk); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, x_motor_corr); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, y_motor_corr); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, abs_roll); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, abs_pitch); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, x_rc_req); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, y_rc_req); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, x_rate_req); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, y_rate_req); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				
				//end_ps = stringify_float(end_ps, time_delta_in_seconds); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				*end_ps = '\n'; end_ps++;
				uart_write_through_dma(print_str, end_ps - print_str);
			#endif
		}
		else
		{
			print_iter++;
		}

		delay_until_us(begin_micros + LOOP_EVERY_MICROS);
	}
}