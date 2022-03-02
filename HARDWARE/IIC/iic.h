#ifndef __IIC_H
#define __IIC_H
#include "sys.h"

//���AD0��(9��)�ӵ�,IIC��ַΪ0X68(���������λ).
//�����V3.3,��IIC��ַΪ0X69(���������λ).
#define MPU_ADDR				0X68


//��Ϊģ��AD0Ĭ�Ͻ�GND,����תΪ��д��ַ��,Ϊ0XD1��0XD0(�����VCC,��Ϊ0XD3��0XD2)  
#define MPU_READ    0XD1
#define MPU_WRITE   0XD0

/*IO��������*/
#define SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=(u32)3<<28;}

/*IO��������*/	 
#define IIC_SCL    PCout(6) //SCL
#define IIC_SDA    PCout(7) //SDA	 
#define READ_SDA   PCin(7)  //����SDA 

/*IIC���в�������*/
void IIC_Init(void);                	//��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(u8 ack);			//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);				//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Write_Byte(u8 reg,u8 data); 				 
u8 IIC_Read_ONEByte(u8 reg);
#endif

