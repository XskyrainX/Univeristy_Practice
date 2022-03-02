#ifndef __MPU6050_H
#define __MPU6050_H

#include "sys.h"
#include "iic.h"

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);





#endif

