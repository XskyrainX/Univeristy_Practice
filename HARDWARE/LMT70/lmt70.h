#ifndef __LMT70_H
#define __LMT70_H

#include "sys.h"

void  Adc_Init(void);
u16 Get_Adc(u8 ch);  
u16 Get_Adc_Average(u8 ch,u8 times);
float LMT(void);

#endif

