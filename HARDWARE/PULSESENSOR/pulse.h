#ifndef __PULSE_H
#define __PULSE_H

#include "sys.h"


void Pulse_Sensor_Init(void);
u16 Get_Adc_Pulse(u8 ch);

extern int Signal;
extern int BPM;                   //  用于保存脉搏频率


#endif


