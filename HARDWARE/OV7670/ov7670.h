#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"

#define OV7670_MID				0X7FA2    
#define OV7670_PID				0X7721

#define OV7670_VSYNC  	PAin(8)			//ͬ���źż��IO
#define OV7670_WRST		PDout(6)		//дָ�븴λ
#define OV7670_WREN		PBout(3)		//д��FIFOʹ��
#define OV7670_RCK_H	GPIOB->BSRR=1<<4//���ö�����ʱ�Ӹߵ�ƽ
#define OV7670_RCK_L	GPIOB->BRR=1<<4	//���ö�����ʱ�ӵ͵�ƽ
#define OV7670_RRST		PGout(14)  		//��ָ�븴λ
#define OV7670_CS		PGout(15)  		//Ƭѡ�ź�(OE)
								  					 
#define OV7670_DATA   	GPIO_ReadInputData(GPIOC,0x00FF) 	//��������˿�



	    				 
u8   OV7670_Init(void);		  	   		 
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(s8 sat);
void OV7670_Brightness(s8 bright);
void OV7670_Contrast(s8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 width,u16 height,u8 mode);
#endif





















