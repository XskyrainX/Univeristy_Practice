#ifndef __MAX30100_H
#define __MAX30100_H
#include "iic.h"

/*---------------------------------------------------------------------------
*  MAX30100¼Ä´æÆ÷ÉùÃ÷
*  ²Î¿¼MAX30100Êý¾ÝÊÖ²á
---------------------------------------------------------------------------*/

/*×´Ì¬¼Ä´æÆ÷*/
#define MAX30100_INT_STATUS 		0x00
#define MAX30100_INT_ENB 		0x01
/*FIFO¼Ä´æÆ÷*/
#define MAX30100_FIFO_WR_PTR      	0x02
#define MAX30100_FIFO_OVF_COUNTER 	0x03
#define MAX30100_FIFO_RD_PTR      	0x04
#define MAX30100_FIFO_DATA        	0x05
/*ÅäÖÃ¼Ä´æÆ÷*/
#define MAX30100_CONFIG_MODE     	0x06
#define MAX30100_CONFIG_SPO2 		0x07
#define MAX30100_CONFIG_LED      	0x09
/*ÎÂ¶ÈÏà¹Ø¼Ä´æÆ÷*/
#define  MAX30100_TEMP_INTEGER  	0x16
#define  MAX30100_TEMP_FRACTION    	0x17
/*Ð¾Æ¬ID¼Ä´æÆ÷*/
#define MAX30100_REV_ID       	0xFE
#define MAX30100_PART_ID       	0xFF

#define MAX30100_Write_Adrres		0xAE
#define MAX30100_Read_Adrres		0xAF

u8 MAX30100_WriteDatas(u8 Reg,u8 Byte);
void MAX30100_Init();


#endif
