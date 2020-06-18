#include<pid/pid.h>

void pid_init(pid_state* pid, float Kp, float Ki, float Kd, float range)
{
	pid->previous_value    = NAN;
	pid->accumulated_error = 0.0;
	pid->constants.Kp = Kp;
	pid->constants.Ki = Ki;
	pid->constants.Kd = Kd;
	pid->constants.range = range;
}

void pid_reinit(pid_state* pid)
{
	pid->previous_value    = NAN;
	pid->accumulated_error = 0.0;
}

void pid_update_constants(pid_state* pid, float Kp, float Ki, float Kd)
{
	pid->constants.Kp = Kp;
	pid->constants.Ki = Ki;
	pid->constants.Kd = Kd;
}

float pid_update(pid_state* pid, float current_value, float set_point)
{
	// this condition is when the pid_update is forst called
	// we set it to current value so that the derivative component is 0
	if(isnan(pid->previous_value))
		pid->previous_value = current_value;

	// calculate error
	float error = set_point - current_value;

	// propotional component
	float propotional = pid->constants.Kp * error;

	// integral component
	pid->accumulated_error += error;
	float integral = pid->constants.Ki * pid->accumulated_error;
	int check = 1;
	
	if(integral > pid->constants.range)
		integral =  pid->constants.range;
	else if(integral < -pid->constants.range)
		integral = -pid->constants.range;
	else
		check = 0;

	if(check)
		pid->accumulated_error = integral/pid->constants.Ki;

	// do not consider setpoint in error derivative, since this can create un wanted spikes
	// so we calculate the previous error only using the current setpoint, effectively nullyfying the effect of the setpoint
	float previous_error = set_point - pid->previous_value;
	float derivative  = pid->constants.Kd * (error - previous_error);
	pid->previous_value = current_value;

	float result = propotional + integral + derivative;

	// bound result if it is out of bounds
	if(result > pid->constants.range)
		result = pid->constants.range;
	else if(result < -pid->constants.range)
		result = -pid->constants.range;

	return result;
}