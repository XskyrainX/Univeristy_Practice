#ifndef __SRF05_H
#define __SRF05_H

#include "sys.h"

void StartModule(void);
void SRF_05_Init(void);
float SRF_05_Count(void);

#define Trig PCout(0)
#define Echo PAin(0)

#endif
