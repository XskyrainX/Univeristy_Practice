#include "iic.h"
#include "delay.h"
/*---------------------------------------------------------------------------
* 函数：IIC_Init(void)
* 描述：IIC引脚初始化
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_6|GPIO_Pin_7);
}
/*---------------------------------------------------------------------------
* 函数：IIC_Start(void)
* 描述：IIC主机发出开始条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_Start()
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
/*---------------------------------------------------------------------------
* 函数：IIC_Stop(void)
* 描述：IIC主机发出停止条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_Stop()
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);				
}
/*---------------------------------------------------------------------------
* 函数：IIC_Wait_Ack(void)
* 描述：IIC主机等待从机应答信号
* 参数：无
* 返回：0：从机应答；   1：从机无应答
*---------------------------------------------------------------------------*/
u8 IIC_Wait_Ack()
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
}
/*---------------------------------------------------------------------------
* 函数：IIC_Ack(void)
* 描述：IIC主机发出应答信号
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_Ack()
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
/*---------------------------------------------------------------------------
* 函数：IIC_NAck(void)
* 描述：IIC主机发出不应答信号
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_NAck()
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
/*---------------------------------------------------------------------------
* 函数：IIC_Send_Byte(void)
* 描述：IIC主机往从机发送数据
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void IIC_Send_Byte(u8 txd)
{
	u8 t;   
	SDA_OUT(); 	    
	IIC_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}	 
} 	
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_ReceiveByte(u8 ack)
* 描述：IIC主机从从机接收数据
* 参数：无
* 返回：receive:接收到的数据
*---------------------------------------------------------------------------*/
u8 IIC_Read_Byte(u8 ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++)
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)
			receive++;   
		delay_us(1); 
	}						 
    if (!ack)
		IIC_NAck();//发送nACK
    else
		IIC_Ack(); //发送ACK   
    return receive;
}
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);	//发送数据
		if(IIC_Wait_Ack())		//等待ACK
		{
			IIC_Stop();	 
			return 1;		 
		}		
	}    
	IIC_Stop();	 
	return 0;	
} 
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	IIC_Start();
	IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
	IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)
			*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else 
			*buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    IIC_Stop();	//产生一个停止条件 
	return 0;	
}
//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 IIC_Write_Byte(u8 reg,u8 data) 				 
{ 
	IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答 
	IIC_Send_Byte(data);//发送数据
	if(IIC_Wait_Ack())	//等待ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
	IIC_Stop();	 
	return 0;
}
//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 IIC_Read_ONEByte(u8 reg)
{
	u8 res;
	IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	IIC_Wait_Ack();		//等待应答 
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
	IIC_Wait_Ack();		//等待应答 
	res=IIC_Read_Byte(0);//读取数据,发送nACK 
	IIC_Stop();			//产生一个停止条件 
	return res;		
}
