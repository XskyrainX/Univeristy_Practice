#include "srf05.h"
#include "delay.h"
#include "usart.h"
u32 S;
float Distance;

void StartModule()
{
	Trig=0;
	Trig=1;
	delay_us(15);
	Trig=0;
}
void SRF_05_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=0XFFFF;
	TIM_TimeBaseInitStruct.TIM_Prescaler=72-1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter=0x00;
	TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM5,&TIM_ICInitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM5,TIM_IT_CC1|TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM5,ENABLE);
}

u8 TIM5CH1_CAPTURE_STA=0; 
u16 TIM5CH1_CAPTURE_VAL;

void TIM5_IRQHandler()
{
	if((TIM5CH1_CAPTURE_STA&0x80)==0)
	{
		if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
		{	
			if(TIM5CH1_CAPTURE_STA&0x40)
			{
					if((TIM5CH1_CAPTURE_STA&0x3F)==0X3F)
					{
						TIM5CH1_CAPTURE_STA|=0x80;
						TIM5CH1_CAPTURE_VAL=0xFFFF;
					}
					else
						TIM5CH1_CAPTURE_STA++;
			}
		}
	}
	
	if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
	{
		if(TIM5CH1_CAPTURE_STA&0x40)
		{
			TIM5CH1_CAPTURE_STA|=0x80;
			TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);
		}
		else
		{
			TIM5CH1_CAPTURE_STA=0;
			TIM5CH1_CAPTURE_VAL=0;
			TIM_SetCounter(TIM5,0);
			TIM5CH1_CAPTURE_STA|=0x40;
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_CC1|TIM_IT_Update);
}

float SRF_05_Count()
{
	if(TIM5CH1_CAPTURE_STA&0x80)
	{
		S=TIM5CH1_CAPTURE_STA&0X3F;
		S*=65536;
		S+=TIM5CH1_CAPTURE_VAL;
		TIM5CH1_CAPTURE_STA=0;
		Distance=((float)S/58);
	}
	return Distance;
}
