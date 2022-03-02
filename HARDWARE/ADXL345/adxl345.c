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
	IIC_Send_Byte(ADXL_WRITE);	//����д����ָ��	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(0x32);   		//���ͼĴ�����ַ(���ݻ������ʼ��ַΪ0X32)
	IIC_Wait_Ack(); 	 										  		   
 
 	IIC_Start();  	 	   		//��������
	IIC_Send_Byte(ADXL_READ);	//���Ͷ�����ָ��
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
		case 0://����ȻZ��ĽǶ�
 			temp=sqrt((x*x+y*y))/z;
 			res=atan(temp);
 			break;
		case 1://����ȻX��ĽǶ�
 			temp=x/sqrt((y*y+z*z));
 			res=atan(temp);
 			break;
 		case 2://����ȻY��ĽǶ�
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
	ADXL345_WR_Reg(POWER_CTL,0x00);	   	//�Ƚ�������ģʽ.
	delay_ms(100);
	ADXL345_WR_Reg(DATA_FORMAT,0X2B);	//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
	ADXL345_WR_Reg(BW_RATE,0x0A);		//��������ٶ�Ϊ100Hz
	ADXL345_WR_Reg(POWER_CTL,0x28);	   	//����ʹ��,����ģʽ
	ADXL345_WR_Reg(INT_ENABLE,0x00);	//��ʹ���ж�		 

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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	static u8 counter;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if(counter == FILTER_CNT)
		{
			counter = 0;
		}
		ADXL345_RD_XYZ(&axis_avg.info[counter].x,&axis_avg.info[counter].y,&axis_avg.info[counter].z);	//��ȡX,Y,Z��������ļ��ٶ�ֵ	
		counter ++;
		filter_calculate(&axis_avg,&sample);
		peak_update(&Peak_Value,&sample);
		slid_update(&Slid,&sample);
		is_most_active(&Peak_Value);
		
		Steps = detect_step(&Peak_Value,&Slid,&sample);

		printf("Z ANG:%4d   Num:%5d\r\n",sample.z,Steps);//�����ϴ�	
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

//�ڶ�̬��ֵ�ṹ���ʼ��ʱ��һ��Ҫ��max��ֵ����ֵΪ��Сֵ��min��ֵΪ���ֵ�������������ڶ�̬���¡�
 void peak_update(peak_value_t *peak, axis_info_t *cur_sample)
{
  	static unsigned int sample_size = 0;
	sample_size ++;
	if (sample_size > SAMPLE_SIZE) 
	{
		/*�����ﵽ50��������һ��*/
		sample_size = 1;
		peak->oldmax = peak->newmax;
		peak->oldmin = peak->newmin;
      	//��ʼ��
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


/*�жϵ�ǰ���Ծ��*/
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
 
/*�ж��Ƿ��߲�*/
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











