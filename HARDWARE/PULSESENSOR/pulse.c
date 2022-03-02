#include "pulse.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

#define true 1
#define false 0
int BPM;            //����ֵ ÿ�����������ٴ�       // used to hold the pulse rate
int Signal;         //������ֵ����adcֵ       // holds the incoming raw data
int IBI = 600;      //�����������ʱ�� ��λ ����       // holds the time between beats, must be seeded!
unsigned char Pulse = false; //����     // true when pulse wave is high, false when it's low
unsigned char QS = false;   //QSΪ��ʱ���ʾһ������������     // becomes true when Arduoino finds a beat.
int rate[10];       //���������ڱ������10��IBI��ֵ             // array to hold last ten IBI values
unsigned long sampleCounter = 0; //������������ʱ��         // used to determine pulse timing
unsigned long lastBeatTime = 0;  //����ʱ�䣬��IBI��ʱ��         // used to find IBI
int Peak =512;        //����ͳ�Ʒ�ֵ              // used to find peak in pulse wave, seeded
int Trough = 512;       //����ͳ�Ƶ͹�ֵ              // used to find trough in pulse wave, seeded
int thresh = 512;  //��ֵ�͵͹ȷֽ�ʱ��              // used to find instant moment of heart beat, seeded
int amp = 100;     //�������              // used to hold amplitude of pulse waveform, seeded
int Num;             //ʱ��
unsigned char firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
unsigned char secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
//����ȷ������ʱ��  IBI


void  Pulse_Sensor_Init()
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE );
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_DeInit(ADC1);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=2;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=2000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=72-1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM3,ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
}				  

void TIM3_IRQHandler()
{
	int N;
	unsigned char i;
	// ����һ�������ܼƽ�10�ε� IBIֵ
	unsigned int runningTotal = 0;                  // ������е��ܱ���    
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		Get_Adc_Pulse(ADC_Channel_4);
		sampleCounter += 2;                         // �Ժ���Ϊ��λ���ٴ˱���
		N = sampleCounter - lastBeatTime;       // ����ʱ�䣬�Ա������һ�ĵ�����


	    //  �����������Ĳ���Ͳ���
		if(Signal < thresh && N > (IBI/5)*3)
		{       // ͨ���ȴ����3/5IBI�������ز�����
			if (Signal < Trough)
			{                      //T���ź���͵� 
				Trough = Signal;                         // ��������������͵� 
			}
		}

		if(Signal > thresh && Signal > Peak)
		{          // ������������ֵ����
			Peak = Signal;                             // P��ֵ�ź�
		}                                        // ��������������ߵ�

	  //  ��������������
	  // �źŲ�ÿһ�ε���������һ������ 
		if (N > 250)
		{                                   // �����Ƶ����
			if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )
			{        
				Pulse = true;                               // ��������Ϊ��һ������ʱ�����������־
				IBI = sampleCounter - lastBeatTime;         // �Ժ���Ϊ��λ�ڲ�������
				lastBeatTime = sampleCounter;               // ������һ�������ʱ��

				if(secondBeat)
				{                        // ������ǵڶ����ģ����secondBeatΪ��
					secondBeat = false;                  // ��secondBeat��־λ
					for(i=0; i<=9; i++)
					{             // ������ʱ�õ�һ����ʵ��BPM
						rate[i] = IBI;                      
				  }
				}
			
				if(firstBeat)
				{                         // ������ǵ�һ�����Ƿ�����һ�����ģ����firstbeatΪ��
					firstBeat = false;                   // ��firstBeat��־λ
				  secondBeat = true;                   // �ø�second beat��־λ
				  return;                              //��ֵ�ǲ��ɿ��ģ�������
				}   
			  


				for(i=0; i<=8; i++)
				{                // ��λ���������е�����
					rate[i] = rate[i+1];                  // ����IBI��ֵ
				  runningTotal += rate[i];              // �Ÿ���ֵ���
				}

				rate[9] = IBI;                          // �������һ�� IBI �ӵ� ����������
				runningTotal += rate[9];                // �����һ�� IBI �ӵ� runningTotal
				runningTotal /= 10;                     // �����10�ε� IBI ��ƽ��ֵ 
				BPM = 60000/runningTotal;               //һ���ӿ��������ٴμ�Ϊ����
					//	if(BPM>200)BPM=200;			//����BPM�����ʾֵ
				 QS = true;                              // �ø������ı�־
				// ���ε�ISR QS��־����0
			}                       
		}

		if (Signal < thresh && Pulse == true)
		{   // ��ֵ�½�ʱ����������
			Pulse = false;                         // Ϊ���ܹ��ظ����̣���λ Pulse ��־λ 
		    amp = Peak - Trough;                           // �õ������������
		    thresh = amp/2 + Trough;                    // ������ֵ�ڷ��ȵ�50%
		    Peak = thresh;                            // ��һ��	���ñ�־λ
		    Trough = thresh;
		  }		 
	 
		if (N > 2500)
		{                           // ���2.5��û������
		    thresh = 512;                          // ����Ĭ����ֵ
		    Peak = 512;                               // ���� P ��Ĭ��ֵ
		    Trough = 512;                               // ���� T ��Ĭ��ֵ
		    lastBeatTime = sampleCounter;                
		    firstBeat = true;                      // ��������������
		    secondBeat = false;                    // �����ǵõ�������ʱ��
		}
		if(QS == 1)
		{	
			printf("%d\r\n",BPM);
			QS = 0;
		}
	}
}

u16 Get_Adc_Pulse(u8 ch)   
{
	ADC_RegularChannelConfig(ADC1, ch, 2, ADC_SampleTime_239Cycles5 );
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	return ADC_GetConversionValue(ADC1);
}
