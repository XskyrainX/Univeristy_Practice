#include "adxl345.h"
#include "iic.h"
#include "delay.h"
#include "math.h"
#include "usart.h"

int Steps;

axis_info_t sample;
filter_avg_t axis_avg;
peak_value_t Peak_Value;
slid_reg_t Slid;


void ADXL345_WR_Reg(u8 reg,u8 date)
{
	IIC_Start();
	
	IIC_Send_Byte(ADXL_WRITE);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(date);
	IIC_Wait_Ack();
	
	IIC_Stop();
}

u8 ADXL345_RD_Reg(u8 reg)
{
	u8 date = 0;
	
	IIC_Start();
	
	IIC_Send_Byte(ADXL_WRITE);
	date = IIC_Wait_Ack();
	
	IIC_Send_Byte(reg);
	date = IIC_Wait_Ack();
	
	IIC_Start();
	
	IIC_Send_Byte(ADXL_READ);
	date = IIC_Wait_Ack();
	
	date = IIC_Read_Byte(0);
	
	IIC_Stop();

	return date;
}

u8 ADXL345_Init()
{
	IIC_Init();
	if(ADXL345_RD_Reg(DEVICE_ID) == 0XE5)
	{
		ADXL345_WR_Reg(DATA_FORMAT,0x2B);
		ADXL345_WR_Reg(BW_RATE,0x0A);
		ADXL345_WR_Reg(POWER_CTL,0x28);	 
		ADXL345_WR_Reg(INT_ENABLE,0x00);			 
	 	ADXL345_WR_Reg(OFSX,0x00);
		ADXL345_WR_Reg(OFSY,0x00);
		ADXL345_WR_Reg(OFSZ,0x00);	
		return 0;
	}
	return 1;
}

void ADXL345_RD_XYZ(short *x,short *y,short *z)
{
	u8 buf[6];
	u8 i;
	
	IIC_Start();  				 
	IIC_Send_Byte(ADXL_WRITE);	//发送写器件指令	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(0x32);   		//发送寄存器地址(数据缓存的起始地址为0X32)
	IIC_Wait_Ack(); 	 										  		   
 
 	IIC_Start();  	 	   		//重新启动
	IIC_Send_Byte(ADXL_READ);	//发送读器件指令
	IIC_Wait_Ack();

	for(i = 0;i < 6; i ++)
	{
		if(i == 5)
			buf[i] = IIC_Read_Byte(0);
		else
			buf[i] = IIC_Read_Byte(1);
	}
	
	*x = (short)((buf[1] <<8) | buf[0]);
	*y = (short)((buf[3] <<8) | buf[2]);
	*z = (short)((buf[5] <<8) | buf[4]);
}


void ADXL345_RD_Average(short *x,short *y,short *z,u8 times)	
{
	u8 i;
	short tx,ty,tz;
	*x=0;
	*y=0;
	*z=0;
	if(times)
	{
		for(i = 0;i < times; i ++)
		{
			ADXL345_RD_XYZ(&tx,&ty,&tz);
			*x+=tx;
			*y+=ty;
			*z+=tz;
			delay_ms(5);
		}
		*x/=times;
		*y/=times;
		*z/=times;
	}
}

short ADXL345_Get_Angle(float x,float y,float z,u8 dir)
{
	float temp;
 	float res=0;
	switch(dir)
	{
		case 0://与自然Z轴的角度
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1://与自然X轴的角度
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2://与自然Y轴的角度
 			temp=y/sqrt((x*x+z*z));
 			res=atan(temp);
 			break;
 	}
	return res*1800/3.14;
}

void ADXL345_RD_Avval(short *x,short *y,short *z)
{
	short tx=0,ty=0,tz=0;	   
	u8 i;  
	for(i=0;i<10;i++)
	{
		ADXL345_RD_XYZ(x,y,z);
		delay_ms(10);
		tx+=(short)*x;
		ty+=(short)*y;
		tz+=(short)*z;	   
	}
	*x=tx/10;
	*y=ty/10;
	*z=tz/10;
} 

void ADXL345_AUTO_Adjust(char *xval,char *yval,char *zval)
{
	short tx,ty,tz;
	u8 i;
	short offx=0,offy=0,offz=0;
	ADXL345_WR_Reg(POWER_CTL,0x00);	   	//先进入休眠模式.
	delay_ms(100);
	ADXL345_WR_Reg(DATA_FORMAT,0X2B);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程 
	ADXL345_WR_Reg(BW_RATE,0x0A);		//数据输出速度为100Hz
	ADXL345_WR_Reg(POWER_CTL,0x28);	   	//链接使能,测量模式
	ADXL345_WR_Reg(INT_ENABLE,0x00);	//不使用中断		 

	ADXL345_WR_Reg(OFSX,0x00);
	ADXL345_WR_Reg(OFSY,0x00);
	ADXL345_WR_Reg(OFSZ,0x00);
	delay_ms(12);
	for(i=0;i<10;i++)
	{
		ADXL345_RD_Avval(&tx,&ty,&tz);
		offx+=tx;
		offy+=ty;
		offz+=tz;
	}	 		
	offx/=10;
	offy/=10;
	offz/=10;
	*xval=-offx/4;
	*yval=-offy/4;
	*zval=-(offz-256)/4;	  
 	ADXL345_WR_Reg(OFSX,*xval);
	ADXL345_WR_Reg(OFSY,*yval);
	ADXL345_WR_Reg(OFSZ,*zval);	
} 

void TIM3_Int_Init(u16 arr,u16 psc)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	static u8 counter;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if(counter == FILTER_CNT)
		{
			counter = 0;
		}
		ADXL345_RD_XYZ(&axis_avg.info[counter].x,&axis_avg.info[counter].y,&axis_avg.info[counter].z);	//读取X,Y,Z三个方向的加速度值	
		counter ++;
		filter_calculate(&axis_avg,&sample);
		peak_update(&Peak_Value,&sample);
		slid_update(&Slid,&sample);
		is_most_active(&Peak_Value);
		
		Steps = detect_step(&Peak_Value,&Slid,&sample);

		printf("Z ANG:%4d   Num:%5d\r\n",sample.z,Steps);//串口上传	
	}
}

 void filter_calculate(filter_avg_t *filter, axis_info_t *sample)
{
	unsigned int i;
	short x_sum = 0, y_sum = 0, z_sum = 0;
	for (i = 0; i < FILTER_CNT; i++) 
	{
		x_sum += filter->info[i].x;
		y_sum += filter->info[i].y;
		z_sum += filter->info[i].z;
	}
	sample->x = x_sum / FILTER_CNT;
	sample->y = y_sum / FILTER_CNT;
	sample->z = z_sum / FILTER_CNT;
}

//在动态阈值结构体初始化时，一定要将max的值都赋值为最小值，min赋值为最大值，这样才有利于动态更新。
 void peak_update(peak_value_t *peak, axis_info_t *cur_sample)
{
  	static unsigned int sample_size = 0;
	sample_size ++;
	if (sample_size > SAMPLE_SIZE) 
	{
		/*采样达到50个，更新一次*/
		sample_size = 1;
		peak->oldmax = peak->newmax;
		peak->oldmin = peak->newmin;
      	//初始化
      	peak_value_init(peak);
	}
	peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
	peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
	peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);
	
	peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
	peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
	peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
	
}

 char slid_update(slid_reg_t *slid, axis_info_t *cur_sample)
{
  	char res = 0;
  	if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION) 
	{
		slid->old_sample.x = slid->new_sample.x;
		slid->new_sample.x = cur_sample->x;
		res = 1;
	} 
	else 
	{
		slid->old_sample.x = slid->new_sample.x;
	}
	if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION) 
	{
		slid->old_sample.y = slid->new_sample.y;
		slid->new_sample.y = cur_sample->y;
		res = 1;
	} 
	else 
	{
		slid->old_sample.y = slid->new_sample.y;
	}
	
	if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION) 
	{
		slid->old_sample.z = slid->new_sample.z;
		slid->new_sample.z = cur_sample->z;
		res = 1;
	} 
	else 
	{
		slid->old_sample.z = slid->new_sample.z;
	}
	return res;
}

 void peak_value_init(peak_value_t *peak)
{
	peak->newmax.x = 0;
	peak->newmax.y = 0;
	peak->newmax.z = 0;
	
	peak->newmin.x = 0;
	peak->newmin.y = 0;
	peak->newmin.z = 0;
	
	peak->oldmax.x = 0;
	peak->oldmax.y = 0;
	peak->oldmax.z = 0;
	
	peak->oldmin.x = 0;
	peak->oldmin.y = 0;
	peak->oldmin.z = 0;

}


/*判断当前最活跃轴*/
 char is_most_active(peak_value_t *peak)
{
	char res = MOST_ACTIVE_NULL;
	short x_change = ABS((peak->newmax.x - peak->newmin.x));
	short y_change = ABS((peak->newmax.y - peak->newmin.y));
	short z_change = ABS((peak->newmax.z - peak->newmin.z));
	
	if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION) {
		res = MOST_ACTIVE_X;
	} else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION) {
		res = MOST_ACTIVE_Y;
	} else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION) {
		res = MOST_ACTIVE_Z;
	}
	return res;
}
 
/*判断是否走步*/
 u16 detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample)
{
	static int step_cnt = 0;
	char res = is_most_active(peak);
	switch (res) 
	{
		case MOST_ACTIVE_NULL: 
		{
			//fix
			break;
		}
		case MOST_ACTIVE_X: 
		{
			short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
			if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x) 
			{
				step_cnt ++;
			}
			return step_cnt;
		}
		case MOST_ACTIVE_Y: 
		{
			short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
			if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y) 
			{
				step_cnt ++;
			}
			return step_cnt;
		}
		case MOST_ACTIVE_Z: 
		{
			short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
			if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z) 
			{
				step_cnt ++;
			}
			return step_cnt;
		}
		default: 
			return step_cnt;
	}
	return step_cnt;
}











