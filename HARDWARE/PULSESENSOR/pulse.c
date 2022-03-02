#include "pulse.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

#define true 1
#define false 0
int BPM;            //心率值 每分钟跳动多少次       // used to hold the pulse rate
int Signal;         //采样的值，即adc值       // holds the incoming raw data
int IBI = 600;      //两次心跳间隔时间 单位 毫秒       // holds the time between beats, must be seeded!
unsigned char Pulse = false; //脉冲     // true when pulse wave is high, false when it's low
unsigned char QS = false;   //QS为真时候表示一次真正的心跳     // becomes true when Arduoino finds a beat.
int rate[10];       //该数组用于保存最后10次IBI的值             // array to hold last ten IBI values
unsigned long sampleCounter = 0; //脉搏跳动启动时间         // used to determine pulse timing
unsigned long lastBeatTime = 0;  //结束时间，即IBI的时间         // used to find IBI
int Peak =512;        //用于统计峰值              // used to find peak in pulse wave, seeded
int Trough = 512;       //用于统计低谷值              // used to find trough in pulse wave, seeded
int thresh = 512;  //峰值和低谷分界时间              // used to find instant moment of heart beat, seeded
int amp = 100;     //震幅长度              // used to hold amplitude of pulse waveform, seeded
int Num;             //时间
unsigned char firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
unsigned char secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
//用于确定心跳时间  IBI


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
	// 保持一个运行总计近10次的 IBI值
	unsigned int runningTotal = 0;                  // 清除运行的总变量    
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		Get_Adc_Pulse(ADC_Channel_4);
		sampleCounter += 2;                         // 以毫秒为单位跟踪此变量
		N = sampleCounter - lastBeatTime;       // 监视时间，以避免最后一拍的噪声


	    //  发现脉搏波的波峰和波谷
		if(Signal < thresh && N > (IBI/5)*3)
		{       // 通过等待最后3/5IBI来避免重博噪声
			if (Signal < Trough)
			{                      //T是信号最低点 
				Trough = Signal;                         // 跟踪脉搏波的最低点 
			}
		}

		if(Signal > thresh && Signal > Peak)
		{          // 避免噪声的阈值条件
			Peak = Signal;                             // P峰值信号
		}                                        // 跟踪脉搏波的最高点

	  //  接下来计算心率
	  // 信号波每一次的上升都有一次脉搏 
		if (N > 250)
		{                                   // 避免高频噪声
			if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )
			{        
				Pulse = true;                               // 当我们认为有一个脉冲时，设置脉冲标志
				IBI = sampleCounter - lastBeatTime;         // 以毫秒为单位内测量节拍
				lastBeatTime = sampleCounter;               // 跟踪下一个脉冲的时间

				if(secondBeat)
				{                        // 如果这是第二次拍，如果secondBeat为真
					secondBeat = false;                  // 清secondBeat标志位
					for(i=0; i<=9; i++)
					{             // 在启动时得到一个真实的BPM
						rate[i] = IBI;                      
				  }
				}
			
				if(firstBeat)
				{                         // 如果这是第一次我们发现了一个节拍，如果firstbeat为真
					firstBeat = false;                   // 清firstBeat标志位
				  secondBeat = true;                   // 置高second beat标志位
				  return;                              //该值是不可靠的，丢弃它
				}   
			  


				for(i=0; i<=8; i++)
				{                // 移位速率数组中的数据
					rate[i] = rate[i+1];                  // 放入IBI的值
				  runningTotal += rate[i];              // 九个数值相加
				}

				rate[9] = IBI;                          // 将最近的一次 IBI 加到 速率数组中
				runningTotal += rate[9];                // 将最近一次 IBI 加到 runningTotal
				runningTotal /= 10;                     // 求最近10次的 IBI 的平均值 
				BPM = 60000/runningTotal;               //一分钟可以跳多少次即为心率
					//	if(BPM>200)BPM=200;			//限制BPM最高显示值
				 QS = true;                              // 置高量化的标志
				// 本次的ISR QS标志不清0
			}                       
		}

		if (Signal < thresh && Pulse == true)
		{   // 当值下降时，跳动结束
			Pulse = false;                         // 为了能够重复进程，复位 Pulse 标志位 
		    amp = Peak - Trough;                           // 得到的脉搏波振幅
		    thresh = amp/2 + Trough;                    // 设置阈值在幅度的50%
		    Peak = thresh;                            // 下一次	重置标志位
		    Trough = thresh;
		  }		 
	 
		if (N > 2500)
		{                           // 如果2.5秒没有跳动
		    thresh = 512;                          // 设置默认阈值
		    Peak = 512;                               // 设置 P 的默认值
		    Trough = 512;                               // 设置 T 的默认值
		    lastBeatTime = sampleCounter;                
		    firstBeat = true;                      // 避免噪音的设置
		    secondBeat = false;                    // 当我们得到心跳的时候
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
