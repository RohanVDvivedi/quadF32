#ifndef PID_H
#define PID_H

#include<math.h>

typedef struct pid_const pid_const;
struct pid_const
{
	float Kp;
	float Ki;
	float Kd;
	float range;
};

typedef struct pid_state pid_state;
struct pid_state
{
	pid_const constants;
	float previous_value;
	float accumulated_error;
};

void pid_init(pid_state* pid, float Kp, float Ki, float Kd, float range);

void pid_reinit(pid_state* pid);

// updates pid constants
void pid_update_constants(pid_state* pid, float Kp, float Ki, float Kd);

// it is your responsibility to call pid_update at a regular interval in your program
float pid_update(pid_state* pid, float current_value, float set_point);

#endif