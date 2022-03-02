/*    MAX30102.h   */
/*============================================================================
* �ļ����룺 Encoding in UTF-8 without signature                             *
* �ļ������� ���ļ���MAX30100��ͷ�ļ�����Ҫ�ṩ�û�API                       *
*                                                                            *
* �������ܣ� �ṩMAX30100��ȡFIFO��API                                       *
* ����    �� ElecM                                                           *
* ��Ȩ    �� ����BSD��ԴЭ��                                                 *
* ʱ��    �� 2020.10.05                                                      *
* ���������� ʹ�øô�������ɵ�һ�к������ʹ���߳е������߲����κη������Ρ�*
============================================================================*/
 
#ifndef  __MAX30102_H__
#define  __MAX30102_H__
 
#include "stm32f10x.h"
 
 
 
 
/*---------------------------------------------------------------------------
*  MAX30100ģ�����Ŷ���
*  �ο�MAX30100ģ���ֲ�
---------------------------------------------------------------------------*/
#define   MAX30102_INT_GET        (GPIOA->IDR&(1<<2))
 
#define   MAX30102_SDA_SET_IN     GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=0X00000008;GPIOA->ODR|=(1<<0);//����Ϊ��������
#define   MAX30102_SDA_SET_OUT    GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=0X00000003;//����Ϊ�������
 
#define   MAX30102_SDA_SET_L      GPIOA->ODR&=(~(1<<0));
#define   MAX30102_SDA_SET_H      GPIOA->ODR|=((1<<0));
 
#define   MAX30102_SDA_GET        (GPIOA->IDR&(1<<0))
 
#define   MAX30102_SCL_SET_L      GPIOA->ODR&=(~(1<<1));
#define   MAX30102_SCL_SET_H      GPIOA->ODR|=((1<<1));
 
 
/*---------------------------------------------------------------------------
*  MAX30100ʹ�ñ������Ͷ���
*  �ο���������ʹ�õ�����
---------------------------------------------------------------------------*/
typedef   vu8        MAX30102_U8;   
typedef   vu16       MAX30102_U16;
typedef   vu32       MAX30102_U32;
typedef   uint64_t   MAX30102_U64;
  
typedef   vs8        MAX30102_S8;   
typedef   vs16       MAX30102_S16;
typedef   vs32       MAX30102_S32;
typedef   int64_t    MAX30102_S64;
 
/*---------------------------------------------------------------------------
*  MAX30102�Ĵ�������
*  �ο�MAX30102�����ֲ�
---------------------------------------------------------------------------*/
//״̬�Ĵ���
#define  MAX30102_INT_STATUS_1       0X00
#define  MAX30102_INT_STATUS_2       0X01
 
#define  MAX30102_INT_ENB_1          0X02
#define  MAX30102_INT_ENB_2          0X03
 
 
//FIFO�Ĵ���
#define  MAX30102_FIFO_WR_PTR        0X04
#define  MAX30102_FIFO_OVF_COUNTER   0X05
#define  MAX30102_FIFO_RD_PTR        0X06
#define  MAX30102_FIFO_DATA          0X07
 
//���üĴ���
#define  MAX30102_CONFIG_FIFO        0X08
#define  MAX30102_CONFIG_MODE        0X09
#define  MAX30102_CONFIG_SPO2        0X0A
#define  MAX30102_CONFIG_LED1_PA     0X0C
#define  MAX30102_CONFIG_LED2_PA     0X0D
#define  MAX30102_CONFIG_PILOT_PA    0X10
 
 
//�¶���ؼĴ���
#define  MAX30102_TEMP_INT           0X1F
#define  MAX30102_TEMP_FRAC          0X20
#define  MAX30102_TEMP_EN            0X21
 
//�������ܼĴ���
#define  MAX30102_PROX_INT_THRESH    0X30
 
//оƬID�Ĵ���
#define  MAX30102_REV_ID             0XFE
#define  MAX30102_PART_ID            0XFF
 
 
 
/*###########################################################################
#                           MAX30100�û�API                                 #
#----------------------------------------------------------------------------
#  ʹ�÷�������ѯ�������ȳ�ʼ��MAX30100ģ�飬�ټ��ģ���Ƿ�׼��������       #
#                    ���Ϳ��Զ�ȡMAX30100�ڲ�FIFO���ݡ�                   #
#----------------------------------------------------------------------------
#  ʹ�÷������жϣ������жϷ����������ñ�־λ�����ݱ�־λ��Ӧ��ȡ����     #
#                                                                           #
#  by ElecM  2020.10.05                                                     #
###########################################################################*/
 
void MAX30102_Init(void);       
MAX30102_U8 MAX30102_CheckIrAndRedDataRdy(void);
MAX30102_U8 MAX30102_ReadIrAndRedData(MAX30102_U32 *Ir,MAX30102_U32 *Red);
MAX30102_U8 MAX30102_ReadOneByte(MAX30102_U8  Reg, MAX30102_U8 *Byte);
MAX30102_U8 MAX30102_ReadBytes(MAX30102_U8  Reg, MAX30102_U8 *Bytes,MAX30102_U8 Len);
 
#endif
 
/*  2020.10.05  by ElecM  */




 