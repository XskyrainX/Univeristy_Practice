#ifndef __ADXL345_H
#define __ADXL345_H

#include "sys.h"

#define DEVICE_ID			0X00 		//器件ID,0XE5
#define THRESH_TAP		0X1D   	//敲击阀值
#define OFSX			0X1E
#define OFSY			0X1F
#define OFSZ			0X20
#define DUR				0X21
#define Latent			0X22
#define Window  			0X23 
#define THRESH_ACK		0X24
#define THRESH_INACT		0X25 
#define TIME_INACT		0X26
#define ACT_INACT_CTL		0X27	 
#define THRESH_FF			0X28	
#define TIME_FF			0X29 
#define TAP_AXES			0X2A  
#define ACT_TAP_STATUS  	0X2B 
#define BW_RATE			0X2C 
#define POWER_CTL			0X2D 

#define INT_ENABLE		0X2E
#define INT_MAP			0X2F
#define INT_SOURCE  		0X30
#define DATA_FORMAT	    	0X31
#define DATA_X0			0X32
#define DATA_X1			0X33
#define DATA_Y0			0X34
#define DATA_Y1			0X35
#define DATA_Z0			0X36
#define DATA_Z1			0X37
#define FIFO_CTL			0X38
#define FIFO_STATUS		0X39


//0X0B TO OX1F Factory Reserved	 
//如果ALT ADDRESS脚(12脚)接地,IIC地址为0X53(不包含最低位).
//如果接V3.3,则IIC地址为0X1D(不包含最低位).
//因为开发板接V3.3,所以转为读写地址后,为0X3B和0X3A(如果接GND,则为0XA7和0XA6)  
#define ADXL_READ    0XA7
#define ADXL_WRITE   0XA6

//I2C设备的写地址 = I2C设备地址 << 1
//I2C设备的读地址 = (I2C设备地址 << 2) + 1

#define MAX(a,b) ((a) > (b) ? (a) : (b)) 
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)



#define DYNAMIC_PRECISION     30     	 /*动态精度*/
#define FILTER_CNT		4	
#define SAMPLE_SIZE   		50

#define MOST_ACTIVE_NULL      			0      	 /*未找到最活跃轴*/
#define MOST_ACTIVE_X					1		 /*最活跃轴X*/	
#define MOST_ACTIVE_Y					2        /*最活跃轴Y*/	
#define MOST_ACTIVE_Z					3        /*最活跃轴Z*/	
 
#define ACTIVE_PRECISION      			60       /*活跃轴最小变化值*/

typedef struct axis_info
{
  short x;
  short y;
  short z;
}axis_info_t;

typedef struct filter_avg
{
	axis_info_t info[FILTER_CNT];
	unsigned char count;
}filter_avg_t;

//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample,如何读取存满就不多说了。
typedef struct 
{
	axis_info_t newmax;
	axis_info_t newmin;
	axis_info_t oldmax;
	axis_info_t oldmin;
}peak_value_t;

/*一个线性移位寄存器，用于过滤高频噪声*/
typedef struct slid_reg
{
	axis_info_t new_sample;
	axis_info_t old_sample;
}slid_reg_t;



extern short x,y,z;  // 三轴加速度量值	    
extern int Steps;//步数

void ADXL345_WR_Reg(u8 reg,u8 date);
u8 ADXL345_RD_Reg(u8 reg);
u8 ADXL345_Init(void);
void ADXL345_RD_XYZ(short *x,short *y,short *z);
void ADXL345_RD_Average(short *x,short *y,short *z,u8 times);
short ADXL345_Get_Angle(float x,float y,float z,u8 dir);
void ADXL345_RD_Avval(short *x,short *y,short *z);
void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval);
void TIM3_Int_Init(u16 arr,u16 psc);

 void filter_calculate(filter_avg_t *filter, axis_info_t *sample);
 char slid_update(slid_reg_t *slid, axis_info_t *cur_sample);
 void peak_update(peak_value_t *peak, axis_info_t *cur_sample);
 void peak_value_init(peak_value_t *peak);
 char is_most_active(peak_value_t *peak);
 u16 detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample);


#endif

