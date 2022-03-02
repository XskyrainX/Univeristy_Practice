#include "max30100.h"

u8 MAX30100_WriteDatas(vu8 Reg,vu8 Byte)
{
	IIC_Start();
	IIC_Send_Byte(MAX30100_Write_Adrres);
	IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(Reg);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(Byte);
	IIC_Wait_Ack();
	
	IIC_Stop();
	return 0;
}

u8 MAX30100_Read_One_Byte(vu8 Reg,vu8 *Byte)
{
	
}

