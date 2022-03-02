#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "lcd.h"
#include "led.h"
#include "key.h"

#include "iic.h"
#include "ads1115.h"
u16 T;
float temp1;
float temp;
float a=-1.809628;
float b=-3.325395;
float c=-1.814103;
float d=2.055894;

int main(void)
{
	int16_t rawData = 0;
	float voltage = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	IIC_Init();
	ADS1115_UserConfig2();
	
	ADS1115_ScanChannel(0);

	while(1)
	{
		
		//获取ADS1115采集的原始16位数据
		ADS1115_ReadRawData(&rawData);
		//将ADS1115采集的原始16位数据换算为实际电压值
		voltage = ADS1115_RawDataToVoltage(rawData);

		//直接获取ADS1115采集的电压数据
		voltage = ADS1115_GetVoltage();
		
		//直接获取ADS1115采集的电压数据（经过多次采样计算平均值）
		voltage = ADS1115_GetAverageVoltage(10);
		
		temp1=voltage;
		temp = a*temp1*temp1*temp1/1000000000+b*temp1*temp1/1000000+c*temp1/10+d*100;
		
		printf("%f\r\n%d\r\n",temp,rawData);
		
		delay_ms(10);
	}

	
}
