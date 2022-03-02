/*  MAX30102.c   */
/*============================================================================
* �ļ����룺 Encoding in UTF-8 without signature                             *
* �ļ������� ���ļ���MAX30102��Դ�ļ�����Ҫʵ���û�API                       *
*                                                                            *
* �������ܣ� ʵ���û�API                                                     *
* ����    �� ElecM                                                           *
* ��Ȩ    �� ����BSD��ԴЭ��                                                 *
* ʱ��    �� 2020.10.05                                                      *
* ���������� ʹ�øô�������ɵ�һ�к������ʹ���߳е������߲����κη������Ρ�*
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
* ������MAX30102_GPIO_Init(void)
* ������MAX30102���ų�ʼ��
* ��������
* ���أ���
*---------------------------------------------------------------------------*/
static void MAX30102_GPIO_Init(void)
{
    RCC->APB2ENR |= (1<<2);              //����GPIOAʱ��
    GPIOA->CRL &= 0XFFFFF000;            //����GPIOA2Ϊ�������룬����GPIOA0,1Ϊ�������
    GPIOA->CRL |= 0X00000833;
    GPIOA->ODR |= ((1<<2)|(1<<1)|(1<<0));//����IIC����
}
 
 
/*---------------------------------------------------------------------------
* ������MAX30102_IIC_Start(void)
* ������IIC����������ʼ����
* ��������
* ���أ���
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_Start(void)
{
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_H       //IIC��ʼ����
    MAX30102_SDA_SET_H
    DelayUs(5);
    MAX30102_SDA_SET_L
    DelayUs(5);
    MAX30102_SCL_SET_L 
	
}
 
 
/*---------------------------------------------------------------------------
* ������MAX30102_IIC_SendByte(void)
* ������IIC�������ӻ���������
* ��������
* ���أ�0�����ͳɹ���   1������ʧ��
*---------------------------------------------------------------------------*/
static void MAX30102_IIC_SendByte(MAX30102_U8 byte)
{
    MAX30102_U8 i;
    MAX30102_SDA_SET_OUT
    MAX30102_SCL_SET_L
    for(i=0;i<8;i++)        //���䵥��Byte
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
* ������MAX30102_IIC_ReceiveByte(void)
* ������IIC�����Ӵӻ���������
* ��������
* ���أ�byte:���յ�������
*---------------------------------------------------------------------------*/
static MAX30102_U8 MAX30102_IIC_ReceiveByte(void)
{ 
    MAX30102_U8 i,byte=0;
    MAX30102_SDA_SET_IN
    MAX30102_SDA_SET_H
    for(i=0;i<8;i++)        //���䵥��Byte
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
* ������MAX30102_IIC_Stop(void)
* ������IIC��������ֹͣ����
* ��������
* ���أ���
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
* ������MAX30102_IIC_WaitACK(void)
* ������IIC�����ȴ��ӻ�Ӧ��
* ��������
* ���أ�1���ӻ���Ӧ��   0���ӻ�Ӧ��
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
* ������MAX30102_IIC_SendACK(void)
* ������IIC��������Ӧ��
* ��������
* ���أ���
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
* ������MAX30102_IIC_SendNACK(void)
* ������IIC����������Ӧ��
* ��������
* ���أ���
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
* ������MAX30102_WriteDatas(MAX30102_U8  Reg, MAX30102_U8 byte)
* ������������д�뵽��Ӧ�ļĴ�����
* ��������
* ���أ�0��д��ɹ��� 1��д��ʧ��
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
* ������MAX30102_ReadOneByte(MAX30102_U8  Reg, MAX30102_U8 *Byte)
* �����������ݴӼĴ����ж���һ���ֽ�
* ������Reg��Ҫ���ļĴ�����   *Byte����������
* ���أ�0�������ɹ��� 1������ʧ��
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
* ������MAX30102_ReadBytes(MAX30102_U8  Reg, MAX30102_U8 *Bytes,MAX30102_U8 Len)
* �����������ݴӼĴ����ж���
* ������Reg��Ҫ���ļĴ�����   *Bytes��������������  Len����ȡ�����ݳ���
* ���أ�0�������ɹ��� 1������ʧ��
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
* ������MAX30102_Init(void)
* ������MAX30102��ʼ��
* ��������
* ���أ���
*---------------------------------------------------------------------------*/
void MAX30102_Init(void)
{
    MAX30102_GPIO_Init();
    MAX30102_WriteByte(MAX30102_CONFIG_MODE,0X40);     //��λ
    MAX30102_WriteByte(MAX30102_CONFIG_MODE,0X03);     //����SPO2ģʽ
    MAX30102_WriteByte(MAX30102_INT_ENB_1,0X40);         //��MAX30102���¶Ⱥ�SPO2���ݲɼ�����ж�  
    // MAX30102_WriteByte(MAX30102_INT_ENB_2,0X50);         //��MAX30102���¶Ⱥ�SPO2���ݲɼ�����ж� 
	
	
    MAX30102_WriteByte(MAX30102_FIFO_WR_PTR,0X00);     //����FIFOд�Ĵ���
    MAX30102_WriteByte(MAX30102_FIFO_OVF_COUNTER,0X00);//����FIFO���������
    MAX30102_WriteByte(MAX30102_FIFO_RD_PTR,0X00);     //����FIFO���Ĵ���
	
	
    MAX30102_WriteByte(MAX30102_CONFIG_FIFO,0X00);     //ÿ��FIFO�����а���1����������
    MAX30102_WriteByte(MAX30102_CONFIG_SPO2,0X63);     //����18λADC������������Ϊ411uS
    MAX30102_WriteByte(MAX30102_CONFIG_LED1_PA,0XbF);  //����LED1Ϊ25.4mA
    MAX30102_WriteByte(MAX30102_CONFIG_LED2_PA,0XbF);  //����LED1Ϊ25.4mA
    // MAX30102_WriteByte(MAX30102_CONFIG_PILOT_PA,0X9F);  //����LED1Ϊ25.4mA
	
    //  MAX30102_WriteByte(MAX30102_PROX_INT_THRESH,0X01);      //����RED��IR��LED������Ϊ27.1mA
	  
}  
 
 
 
/*---------------------------------------------------------------------------
* ������MAX30102_ReadIrAndRedData(MAX30102_U16 *Ir,MAX30102_U16 *Red)
* ��������ȡMAX30102�е�RED��IR����
* ������*Ir�������ĺ�������ָ�룻 *Red�������ĺ������ָ��
* ���أ�1������ʧ�ܣ�  0����ȡ�ɹ�
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
    MAX30102_WriteByte(MAX30102_FIFO_WR_PTR,0X00);        //��дָ������
    MAX30102_WriteByte(MAX30102_FIFO_OVF_COUNTER,0X00);
    MAX30102_WriteByte(MAX30102_FIFO_RD_PTR,0X00);
	 
    return 0;
}
 
 
/*---------------------------------------------------------------------------
* ������MAX30102_CheckIrAndRedDataRdy(void)
* ���������RED��IR�����Ƿ��λ
* ��������
* ���أ�1��δ��λ��  0���Ѿ�λ
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


