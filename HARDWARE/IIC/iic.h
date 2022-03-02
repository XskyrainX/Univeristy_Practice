#ifndef __IIC_H
#define __IIC_H
#include "sys.h"

//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X68


//因为模块AD0默认接GND,所以转为读写地址后,为0XD1和0XD0(如果接VCC,则为0XD3和0XD2)  
#define MPU_READ    0XD1
#define MPU_WRITE   0XD0

/*IO方向设置*/
#define SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=(u32)3<<28;}

/*IO操作函数*/	 
#define IIC_SCL    PCout(6) //SCL
#define IIC_SDA    PCout(7) //SDA	 
#define READ_SDA   PCin(7)  //输入SDA 

/*IIC所有操作函数*/
void IIC_Init(void);                	//初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(u8 ack);			//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);				//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Write_Byte(u8 reg,u8 data); 				 
u8 IIC_Read_ONEByte(u8 reg);
#endif

