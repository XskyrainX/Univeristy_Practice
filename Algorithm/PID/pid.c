#include "pid.h"

void PID_Init()
{
	PID.ActualSpeed=0.0;
	PID.err=0.0;
	PID.err_last=0.0;
	PID.err_next=0.0;
	PID.Kd=0.2;
	PID.Ki=0.015;
	PID.Kp=0.2;
	PID.SetSpeed=0.0;	
}

float PID_realize(float speed)
{
	float incrementSpeed;
	
	PID.SetSpeed=speed;
	PID.err=PID.SetSpeed-PID.ActualSpeed;
	incrementSpeed=PID.Kp*(PID.err-PID.err_next)+PID.Ki*PID.err+PID.Kd*(PID.err-2*PID.err_next+PID.err_last);
	PID.ActualSpeed+=incrementSpeed;
	PID.err_last=PID.err_next;
	PID.err_next=PID.err;
	
	return PID.ActualSpeed;
}

