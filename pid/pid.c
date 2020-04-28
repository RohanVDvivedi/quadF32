#include<pid/pid.h>

void pid_init(pid_state* pid, double Kp, double Ki, double Kd, double range)
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

void pid_update_constants(pid_state* pid, double Kp, double Ki, double Kd)
{
	if(pid->constants.Kp != Kp)
	{
		pid->constants.Kp = Kp;
	}
	if(pid->constants.Ki != Ki)
	{
		pid->accumulated_error = 0.0;
		pid->constants.Ki      = Ki;
	}
	if(pid->constants.Kd != Kd)
	{
		pid->previous_value    = NAN;
		pid->constants.Kd      = Kd;
	}
}

double pid_update(pid_state* pid, double current_value, double set_point)
{
	// this condition is when the pid_update is forst called
	// we set it to current value so that the derivative component is 0
	if(isnan(pid->previous_value))
	{
		pid->previous_value = current_value;
	}

	// calculate error
	double error = set_point - current_value;

	// propotional component
	double propotional = pid->constants.Kp * error;

	// integral component
	pid->accumulated_error += error;
	double integral = pid->constants.Ki * pid->accumulated_error;

	// do not consider setpoint in error derivative, since this can create un wanted spikes
	// so we calculate the previous error only using the current setpoint, effectively nullyfying the effect of the setpoint
	double previous_error = set_point - pid->previous_value;
	double derivative  = pid->constants.Kd * (error - previous_error);
	pid->previous_value = current_value;

	double result = propotional + integral + derivative;

	// bound result if it is out of bounds
	if(result > pid->constants.range)
	{
		result = pid->constants.range;
	}
	else if(result < -pid->constants.range)
	{
		result = -pid->constants.range;
	}

	return result;
}