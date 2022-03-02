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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	IIC_Init();
	ADS1115_UserConfig2();
	
	ADS1115_ScanChannel(0);

	while(1)
	{
		
		//��ȡADS1115�ɼ���ԭʼ16λ����
		ADS1115_ReadRawData(&rawData);
		//��ADS1115�ɼ���ԭʼ16λ���ݻ���Ϊʵ�ʵ�ѹֵ
		voltage = ADS1115_RawDataToVoltage(rawData);

		//ֱ�ӻ�ȡADS1115�ɼ��ĵ�ѹ����
		voltage = ADS1115_GetVoltage();
		
		//ֱ�ӻ�ȡADS1115�ɼ��ĵ�ѹ���ݣ�������β�������ƽ��ֵ��
		voltage = ADS1115_GetAverageVoltage(10);
		
		temp1=voltage;
		temp = a*temp1*temp1*temp1/1000000000+b*temp1*temp1/1000000+c*temp1/10+d*100;
		
		printf("%f\r\n%d\r\n",temp,rawData);
		
		delay_ms(10);
	}

	
}
