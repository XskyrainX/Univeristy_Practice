/*  MAX30102.c   */
/*============================================================================
* 文件编码： Encoding in UTF-8 without signature                             *
* 文件描述： 该文件是MAX30102的源文件，主要实现用户API                       *
*                                                                            *
* 基本功能： 实现用户API                                                     *
* 作者    ： ElecM                                                           *
* 版权    ： 遵守BSD开源协议                                                 *
* 时间    ： 2020.10.05                                                      *
* 责任声明： 使用该代码所造成的一切后果均由使用者承担，作者不负任何法律责任。*
============================================================================*/
 
#include "MAX30102.h"
 
 
static void DelayUs(unsigned char us)
{ 
   unsigned int i;
   while(us--)
   {
     for(i=0;i<100;i++)
     ;
   }
}
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_GPIO_Init(void)
* 描述：MAX30102引脚初始化
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void MAX30102_GPIO_Init(void)
{
    RCC->APB2ENR |= (1<<2);              //开启GPIOA时钟
    GPIOA->CRL &= 0XFFFFF000;            //设置GPIOA2为上拉输入，设置GPIOA0,1为推挽输出
    GPIOA->CRL |= 0X00000833;
    GPIOA->ODR |= ((1<<2)|(1<<1)|(1<<0));//挂起IIC总线
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_Start(void)
* 描述：IIC主机发出开始条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_Start(void)
{
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_H       //IIC开始条件
    MAX30102_SDA_SET_H
    DelayUs(5);
    MAX30102_SDA_SET_L
    DelayUs(5);
    MAX30102_SCL_SET_L 
	
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_SendByte(void)
* 描述：IIC主机往从机发送数据
* 参数：无
* 返回：0：发送成功；   1：发送失败
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_SendByte(MAX30102_U8 byte)
{
    MAX30102_U8 i;
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_L
    for(i=0;i<8;i++)        //传输单个Byte
    {
       if(byte &(0x80))
          MAX30102_SDA_SET_H
       else
          MAX30102_SDA_SET_L
 
       byte <<= 1;
       DelayUs(5);
       MAX30102_SCL_SET_H
       DelayUs(5);
       MAX30102_SCL_SET_L		
       DelayUs(5);	
    }
    MAX30102_SDA_SET_H
}
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_ReceiveByte(void)
* 描述：IIC主机从从机接收数据
* 参数：无
* 返回：byte:接收到的数据
*---------------------------------------------------------------------------*/
static MAX30102_U8 MAX30102_IIC_ReceiveByte(void)
{ 
    MAX30102_U8 i,byte=0;
    MAX30102_SDA_SET_IN
    MAX30102_SDA_SET_H
    for(i=0;i<8;i++)        //传输单个Byte
    {
	byte <<= 1;
	MAX30102_SCL_SET_L		
	DelayUs(5);
	MAX30102_SCL_SET_H
	DelayUs(5);
      if(MAX30102_SDA_GET)
        byte |= 0x01;
      DelayUs(5);
     }
    MAX30102_SCL_SET_L
    MAX30102_SDA_SET_H	 
    DelayUs(5);
    return byte;
}
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_Stop(void)
* 描述：IIC主机发出停止条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_Stop(void)
{
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_L
    MAX30102_SDA_SET_L
    DelayUs(5);
    MAX30102_SCL_SET_H
    DelayUs(5);
    MAX30102_SDA_SET_H
    DelayUs(5);
}
 
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_WaitACK(void)
* 描述：IIC主机等待从机应答
* 参数：无
* 返回：1：从机无应答；   0：从机应答
*---------------------------------------------------------------------------*/
static MAX30102_U8 MAX30102_IIC_WaitACK(void)
{
     MAX30102_U16 time_out=0;
     MAX30102_SCL_SET_L
     MAX30102_SDA_SET_IN
     DelayUs(5);
     MAX30102_SCL_SET_H
     DelayUs(5);
     while(MAX30102_SDA_GET)
     {
        time_out ++ ;
        if(time_out>2000)       
        {
	   MAX30102_IIC_Stop();
	   return 1;
	 }
      }
		 
     MAX30102_SCL_SET_L
     DelayUs(5);		 
     return 0;
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_SendACK(void)
* 描述：IIC主机发出应答
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_SendACK(void)
{
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_L
    DelayUs(5);
    MAX30102_SDA_SET_L
    DelayUs(5);
    MAX30102_SCL_SET_H
    DelayUs(5);
    MAX30102_SDA_SET_L
    DelayUs(5);
    MAX30102_SCL_SET_L
    DelayUs(5);
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_IIC_SendNACK(void)
* 描述：IIC主机发出非应答
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_SendNACK(void)
{
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_L
    DelayUs(5);
    MAX30102_SDA_SET_H
    DelayUs(5);
    MAX30102_SCL_SET_H
    DelayUs(5);
    MAX30102_SCL_SET_L
}
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_WriteDatas(MAX30102_U8  Reg, MAX30102_U8 byte)
* 描述：把数据写入到相应的寄存器中
* 参数：无
* 返回：0：写入成功； 1：写入失败
*---------------------------------------------------------------------------*/
static MAX30102_U8 MAX30102_WriteByte(MAX30102_U8  Reg, MAX30102_U8 Byte)
{
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(0XAE);
    MAX30102_IIC_WaitACK();
	 
    MAX30102_IIC_SendByte(Reg);
    MAX30102_IIC_WaitACK();
	
    MAX30102_IIC_SendByte(Byte);
    MAX30102_IIC_WaitACK();
	
    MAX30102_IIC_Stop();
    return 0;
}
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_ReadOneByte(MAX30102_U8  Reg, MAX30102_U8 *Byte)
* 描述：把数据从寄存器中读出一个字节
* 参数：Reg：要读的寄存器；   *Byte：读出数据
* 返回：0：读出成功； 1：读出失败
*---------------------------------------------------------------------------*/
MAX30102_U8 MAX30102_ReadOneByte(MAX30102_U8  Reg, MAX30102_U8 *Byte)
{
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(0XAE);
    if(MAX30102_IIC_WaitACK())
        return 1;
	 
    MAX30102_IIC_SendByte(Reg);
    if(MAX30102_IIC_WaitACK())
        return 1;
	 
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(0XAF);
    if(MAX30102_IIC_WaitACK())
        return 1;
	
    (*Byte) = MAX30102_IIC_ReceiveByte();
    MAX30102_IIC_SendNACK();
    MAX30102_IIC_Stop();
        return 0;
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_ReadBytes(MAX30102_U8  Reg, MAX30102_U8 *Bytes,MAX30102_U8 Len)
* 描述：把数据从寄存器中读出
* 参数：Reg：要读的寄存器；   *Bytes：读出数据流；  Len：读取的数据长度
* 返回：0：读出成功； 1：读出失败
*---------------------------------------------------------------------------*/
MAX30102_U8 MAX30102_ReadBytes(MAX30102_U8  Reg, MAX30102_U8 *Bytes,MAX30102_U8 Len)
{
    MAX30102_U8 i,data;
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(0XAE);
    if(MAX30102_IIC_WaitACK())
       return 1;
	 
    MAX30102_IIC_SendByte(Reg);
    if(MAX30102_IIC_WaitACK())
       return 1;
	 
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(0XAF);
    if(MAX30102_IIC_WaitACK())
       return 1;
	 
    for(i=0;i<Len-1;i++)
    {
       *Bytes = MAX30102_IIC_ReceiveByte();
       MAX30102_IIC_SendACK();
       Bytes++;
     }
    *Bytes = MAX30102_IIC_ReceiveByte();
    MAX30102_IIC_SendNACK();
    MAX30102_IIC_Stop();
    return 0;
}
 
 
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_Init(void)
* 描述：MAX30102初始化
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void MAX30102_Init(void)
{
    MAX30102_GPIO_Init();
    MAX30102_WriteByte(MAX30102_CONFIG_MODE,0X40);     //复位
    MAX30102_WriteByte(MAX30102_CONFIG_MODE,0X03);     //开启SPO2模式
    MAX30102_WriteByte(MAX30102_INT_ENB_1,0X40);         //打开MAX30102的温度和SPO2数据采集完成中断  
    // MAX30102_WriteByte(MAX30102_INT_ENB_2,0X50);         //打开MAX30102的温度和SPO2数据采集完成中断 
	
	
    MAX30102_WriteByte(MAX30102_FIFO_WR_PTR,0X00);     //清零FIFO写寄存器
    MAX30102_WriteByte(MAX30102_FIFO_OVF_COUNTER,0X00);//清零FIFO溢出计数器
    MAX30102_WriteByte(MAX30102_FIFO_RD_PTR,0X00);     //清零FIFO读寄存器
	
	
    MAX30102_WriteByte(MAX30102_CONFIG_FIFO,0X00);     //每个FIFO样本中包含1个数据样本
    MAX30102_WriteByte(MAX30102_CONFIG_SPO2,0X63);     //开启18位ADC，设置脉冲宽度为411uS
    MAX30102_WriteByte(MAX30102_CONFIG_LED1_PA,0XbF);  //设置LED1为25.4mA
    MAX30102_WriteByte(MAX30102_CONFIG_LED2_PA,0XbF);  //设置LED1为25.4mA
    // MAX30102_WriteByte(MAX30102_CONFIG_PILOT_PA,0X9F);  //设置LED1为25.4mA
	
    //  MAX30102_WriteByte(MAX30102_PROX_INT_THRESH,0X01);      //设置RED和IR的LED电流均为27.1mA
	  
}  
 
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_ReadIrAndRedData(MAX30102_U16 *Ir,MAX30102_U16 *Red)
* 描述：读取MAX30102中的RED和IR数据
* 参数：*Ir：读出的红外数据指针； *Red：读出的红光数据指针
* 返回：1：读出失败；  0：读取成功
*---------------------------------------------------------------------------*/
MAX30102_U8 MAX30102_ReadIrAndRedData(MAX30102_U32 *Ir,MAX30102_U32 *Red)
{
    MAX30102_U8 LED[6];
	 
    if(!MAX30102_ReadBytes(MAX30102_FIFO_DATA,LED,6))
    {
        *Red = (((LED[0]&0x03)<<16)|(LED[1]<<8)|(LED[2]));
        *Ir  = (((LED[3]&0x03)<<16)|(LED[4]<<8)|(LED[5]));
     }
    else
        return 1;
    MAX30102_WriteByte(MAX30102_FIFO_WR_PTR,0X00);        //读写指针清零
    MAX30102_WriteByte(MAX30102_FIFO_OVF_COUNTER,0X00);
    MAX30102_WriteByte(MAX30102_FIFO_RD_PTR,0X00);
	 
    return 0;
}
 
 
/*---------------------------------------------------------------------------
* 函数：MAX30102_CheckIrAndRedDataRdy(void)
* 描述：检查RED和IR数据是否就位
* 参数：无
* 返回：1：未就位；  0：已就位
*---------------------------------------------------------------------------*/
MAX30102_U8 MAX30102_CheckIrAndRedDataRdy(void)
{
    MAX30102_U8 status;
    if(!MAX30102_INT_GET)
    { 
        MAX30102_ReadOneByte(MAX30102_INT_STATUS_1,&status);
 
        if(status &0x40)
            return 0;
        else
            return 1;
     }
    else
       return 1;
 
}
 
 
/*  2020.10.05  by ElecM  */


