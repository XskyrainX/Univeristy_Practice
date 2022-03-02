#ifndef __PID_H
#define __PID_H

typedef struct
{
	float SetSpeed;           //�����趨ֵ
	float ActualSpeed;        //����ʵ��ֵ
	float err;               //����ƫ��ֵ
	float err_next;           //������һ��ƫ��ֵ
	float err_last;           //��������ǰ��ƫ��ֵ
	float Kp,Ki,Kd;           //������������֡�΢��ϵ��
}PID_InitTypeDef;

PID_InitTypeDef PID;

void PID_Init(void);


#endif
