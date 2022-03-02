/*    MAX30102.h   */
/*============================================================================
* 文件编码： Encoding in UTF-8 without signature                             *
* 文件描述： 该文件是MAX30100的头文件，主要提供用户API                       *
*                                                                            *
* 基本功能： 提供MAX30100读取FIFO的API                                       *
* 作者    ： ElecM                                                           *
* 版权    ： 遵守BSD开源协议                                                 *
* 时间    ： 2020.10.05                                                      *
* 责任声明： 使用该代码所造成的一切后果均由使用者承担，作者不负任何法律责任。*
============================================================================*/
 
#ifndef  __MAX30102_H__
#define  __MAX30102_H__
 
#include "stm32f10x.h"
 
 
 
 
/*---------------------------------------------------------------------------
*  MAX30100模块引脚定义
*  参考MAX30100模块手册
---------------------------------------------------------------------------*/
#define   MAX30102_INT_GET        (GPIOA->IDR&(1<<2))
 
#define   MAX30102_SDA_SET_IN     GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=0X00000008;GPIOA->ODR|=(1<<0);//设置为上拉输入
#define   MAX30102_SDA_SET_OUT    GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=0X00000003;//设置为推挽输出
 
#define   MAX30102_SDA_SET_L      GPIOA->ODR&=(~(1<<0));
#define   MAX30102_SDA_SET_H      GPIOA->ODR|=((1<<0));
 
#define   MAX30102_SDA_GET        (GPIOA->IDR&(1<<0))
 
#define   MAX30102_SCL_SET_L      GPIOA->ODR&=(~(1<<1));
#define   MAX30102_SCL_SET_H      GPIOA->ODR|=((1<<1));
 
 
/*---------------------------------------------------------------------------
*  MAX30100使用变量类型定义
*  参考编译器中使用的类型
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
*  MAX30102寄存器声明
*  参考MAX30102数据手册
---------------------------------------------------------------------------*/
//状态寄存器
#define  MAX30102_INT_STATUS_1       0X00
#define  MAX30102_INT_STATUS_2       0X01
 
#define  MAX30102_INT_ENB_1          0X02
#define  MAX30102_INT_ENB_2          0X03
 
 
//FIFO寄存器
#define  MAX30102_FIFO_WR_PTR        0X04
#define  MAX30102_FIFO_OVF_COUNTER   0X05
#define  MAX30102_FIFO_RD_PTR        0X06
#define  MAX30102_FIFO_DATA          0X07
 
//配置寄存器
#define  MAX30102_CONFIG_FIFO        0X08
#define  MAX30102_CONFIG_MODE        0X09
#define  MAX30102_CONFIG_SPO2        0X0A
#define  MAX30102_CONFIG_LED1_PA     0X0C
#define  MAX30102_CONFIG_LED2_PA     0X0D
#define  MAX30102_CONFIG_PILOT_PA    0X10
 
 
//温度相关寄存器
#define  MAX30102_TEMP_INT           0X1F
#define  MAX30102_TEMP_FRAC          0X20
#define  MAX30102_TEMP_EN            0X21
 
//靠近功能寄存器
#define  MAX30102_PROX_INT_THRESH    0X30
 
//芯片ID寄存器
#define  MAX30102_REV_ID             0XFE
#define  MAX30102_PART_ID            0XFF
 
 
 
/*###########################################################################
#                           MAX30100用户API                                 #
#----------------------------------------------------------------------------
#  使用方法（查询）：首先初始化MAX30100模块，再检测模块是否准备就绪，       #
#                    随后就可以读取MAX30100内部FIFO数据。                   #
#----------------------------------------------------------------------------
#  使用方法（中断）：在中断服务函数中设置标志位，根据标志位响应读取数据     #
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




 