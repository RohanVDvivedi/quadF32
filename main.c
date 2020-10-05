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

//#define STABILIZE_MODE
#define STABILIZATION_SENSITIVITY	0.3

#define map(val, a_min, a_max, b_min, b_max)	b_min + ((((float)val) - a_min) * (b_max - b_min)) / (a_max - a_min)
#define insensitivity_limit(val, limit)			((fabsf(val) <= fabsf(limit)) ? 0 : val)

#define GYRO_ACCL_MIX     			0.98
#define LOOP_EVERY_MICROS 			2500

#define THROTTLE_MIN_VALUE			  0.0
#define THROTTLE_PID_ACTIVATE 		200.0
#define THROTTLE_MAX_VALUE			800.0

#define ATTITUDE_INPUT_LIMIT		60.0
#define ANGULAR_RATE_INPUT_LIMIT	60.0

#define MOTOR_MIN_PWM 				110.0
#define MOTOR_MAX_PWM 				990.0

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

	// initialize MPUdatascaled variable used inside the loop
	MPUdatascaled mpuData = *mpuInit;

	// initialize pid variables
	// angular rate control pids, these cause differential motor corrections to attain required angular rates along local axis
		pid_state x_ang_rate_pid; pid_init(&x_ang_rate_pid, 2.4, 0.017, 0.000030, 400);
		pid_state y_ang_rate_pid; pid_init(&y_ang_rate_pid, 2.4, 0.017, 0.000030, 400);
		pid_state z_ang_rate_pid; pid_init(&z_ang_rate_pid, 5.2, 0.037, 0.000065, 400);
	// altitude rate pid will mainly work to make 0 rate of change of altitude
		pid_state z_alt_rate_pid; pid_init(&z_alt_rate_pid, 0, 0, 0, 400);
	// flyable values
	/*
		pid_state x_ang_rate_pid; pid_init(&x_ang_rate_pid, 2.4, 0.017, 0.000030, 400);
		pid_state y_ang_rate_pid; pid_init(&y_ang_rate_pid, 2.4, 0.017, 0.000030, 400);
		pid_state z_ang_rate_pid; pid_init(&z_ang_rate_pid, 5.2, 0.037, 0.000065, 400);
	*/

	uint64_t begin_micros = get_now_micros() - LOOP_EVERY_MICROS;

	uint64_t print_iter = 0;
	char print_str[256];

	float abs_roll_init  = atanf( mpuInit->accl.yj/mpuInit->accl.zk) * 180 / M_PI;
	float abs_pitch_init = atanf(-mpuInit->accl.xi/mpuInit->accl.zk) * 180 / M_PI;

	float abs_roll  =  abs_roll_init;
	float abs_pitch = abs_pitch_init;

	float alt_rate = 0;

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
		+ (atanf( mpuData.accl.yj/mpuData.accl.zk) * 180/M_PI) * (1.0 - GYRO_ACCL_MIX);
		abs_pitch = (abs_pitch + mpuData.gyro.yj * time_delta_in_seconds) * (GYRO_ACCL_MIX)
		+ (atanf(-mpuData.accl.xi/mpuData.accl.zk) * 180/M_PI) * (1.0 - GYRO_ACCL_MIX);

		uint32_t chan_ret[6];
		int is_rc_active = get_rc_channels(chan_ret);

		float throttle = map(chan_ret[3], 0.0, 1000.0, THROTTLE_MIN_VALUE, THROTTLE_MAX_VALUE);
		float x_rc_req = map(chan_ret[5], 0.0, 1000.0, -ATTITUDE_INPUT_LIMIT, ATTITUDE_INPUT_LIMIT);
		float y_rc_req = map(chan_ret[4], 0.0, 1000.0, -ATTITUDE_INPUT_LIMIT, ATTITUDE_INPUT_LIMIT);
		float z_rc_req = map(chan_ret[2], 0.0, 1000.0, ANGULAR_RATE_INPUT_LIMIT, -ANGULAR_RATE_INPUT_LIMIT);
			chan_ret[1] = (chan_ret[1] < 3) ? 0 : chan_ret[1];
		float aux1 = map(chan_ret[1], 0.0, 1000.0, 0.0, 1.2);
			chan_ret[0] = (chan_ret[0] < 3) ? 0 : chan_ret[0];
		float aux2 = map(chan_ret[0], 0.0, 1000.0, 0.0, 0.008);

		x_rc_req = insensitivity_limit(x_rc_req, 0.1);
		y_rc_req = insensitivity_limit(y_rc_req, 0.1);
		z_rc_req = insensitivity_limit(z_rc_req, 2.0);

		#if defined PID_TO_TUNE
			pid_update_constants(&x_ang_rate_pid, 2.4 + aux1, 0.017 + aux2, x_ang_rate_pid.constants.Kd);
			pid_update_constants(&y_ang_rate_pid, 2.4 + aux1, 0.017 + aux2, y_ang_rate_pid.constants.Kd);
			//pid_update_constants(&z_ang_rate_pid, 7.0 + aux1, aux2, 0);
		#endif

		float motor_LF = throttle, motor_RF = throttle, motor_LB = throttle, motor_RB = throttle;

		if(throttle < THROTTLE_PID_ACTIVATE)
		{
			pid_reinit(&x_ang_rate_pid);
			pid_reinit(&y_ang_rate_pid);
			pid_reinit(&z_ang_rate_pid);
			pid_reinit(&z_alt_rate_pid);
		}
		else
		{
			#if defined STABILIZE_MODE
				float x_rate_req = aux1 * insensitivity_limit((x_rc_req - (abs_roll  /*-  abs_roll_init*/)), STABILIZATION_SENSITIVITY);
				float y_rate_req = aux1 * insensitivity_limit((y_rc_req - (abs_pitch /*- abs_pitch_init*/)), STABILIZATION_SENSITIVITY);
			#else
				float x_rate_req = x_rc_req;
				float y_rate_req = y_rc_req;
			#endif

			float z_rate_req = z_rc_req;

			float x_motor_corr = pid_update(&x_ang_rate_pid, mpuData.gyro.xi, x_rate_req);
			float y_motor_corr = pid_update(&y_ang_rate_pid, mpuData.gyro.yj, y_rate_req);
			float z_motor_corr = pid_update(&z_ang_rate_pid, mpuData.gyro.zk, z_rate_req);

			// we always strive to make 0 altitude rate
			float comm_motor_corr = pid_update(&z_alt_rate_pid, 0, 0);

			motor_LF += (comm_motor_corr + x_motor_corr - y_motor_corr - z_motor_corr);
			motor_RF += (comm_motor_corr - x_motor_corr - y_motor_corr + z_motor_corr);
			motor_LB += (comm_motor_corr + x_motor_corr + y_motor_corr + z_motor_corr);
			motor_RB += (comm_motor_corr - x_motor_corr + y_motor_corr - z_motor_corr);

			if(motor_LF < MOTOR_MIN_PWM)	motor_LF = MOTOR_MIN_PWM;
			if(motor_RF < MOTOR_MIN_PWM)	motor_RF = MOTOR_MIN_PWM;
			if(motor_LB < MOTOR_MIN_PWM)	motor_LB = MOTOR_MIN_PWM;
			if(motor_RB < MOTOR_MIN_PWM)	motor_RB = MOTOR_MIN_PWM;

			if(motor_LF > MOTOR_MAX_PWM)	motor_LF = MOTOR_MAX_PWM;
			if(motor_RF > MOTOR_MAX_PWM)	motor_RF = MOTOR_MAX_PWM;
			if(motor_LB > MOTOR_MAX_PWM)	motor_LB = MOTOR_MAX_PWM;
			if(motor_RB > MOTOR_MAX_PWM)	motor_RB = MOTOR_MAX_PWM;
		}

		set_motors(motor_LF, motor_RF, motor_LB, motor_RB);

		#if defined DEBUG_OVER_UART
			if(print_iter == 100)
			{
				print_iter = 0;

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

				//end_ps = stringify_float(end_ps, mpuData.accl.xi); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuData.accl.yj); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuData.accl.zk); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

				//end_ps = stringify_float(end_ps, mpuInit->accl.xi); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuInit->accl.yj); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;
				//end_ps = stringify_float(end_ps, mpuInit->accl.zk); *end_ps = ' '; end_ps++; *end_ps = '\t'; end_ps++;

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
			}
			else
				print_iter++;
		#endif

		delay_until_us(begin_micros + LOOP_EVERY_MICROS);
	}
}