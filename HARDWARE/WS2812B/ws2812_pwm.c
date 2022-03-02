#include "ws2812_pwm.h"
#include "delay.h"

uint16_t pixelBuffer[PIXEL_NUM][GRB];

const uint16_t index_wave[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 
                               4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 
                               13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 30, 31, 32, 33, 
                               34, 36, 37, 38, 40, 41, 43, 45, 46, 48, 50, 52, 54, 56, 58, 60, 62, 65, 67, 70, 72, 75, 78, 81, 84, 87, 90, 
                               94, 97, 101, 105, 109, 113, 117, 122, 126, 131, 136, 141, 146, 152, 158, 164, 170, 176, 183, 190, 197, 205, 
                               213, 221, 229, 238, 247, 256, 256, 247, 238, 229, 221, 213, 205, 197, 190, 183, 176, 170, 164, 158, 152, 146, 
                               141, 136, 131, 126, 122, 117, 113, 109, 105, 101, 97, 94, 90, 87, 84, 81, 78, 75, 72, 70, 67, 65, 62, 60, 58, 
                               56, 54, 52, 50, 48, 46, 45, 43, 41, 40, 38, 37, 36, 34, 33, 32, 31, 30, 28, 27, 26, 25, 25, 24, 23, 22, 21, 20, 
                               20, 19, 18, 18, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 
                               6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                               2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

void WS281x_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	DMA_InitTypeDef DMA_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
  
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 90 - 1;  //72M / 90 = 800K
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM2, &TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM2, ENABLE);	//PWM输出使能	

	TIM_Cmd(TIM2, DISABLE);
	TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
	
	
  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM2->CCR1);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pixelBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = PIXEL_NUM * GRB;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    
	DMA_Cmd(DMA1_Channel5, DISABLE);
	
	WS281x_CloseAll();
	delay_ms(100);
}

void WS281x_CloseAll(void)
{
	uint16_t i;
	uint8_t j;
  
	for(i = 0; i < PIXEL_NUM; ++i)
	{
		for(j = 0; j < 24; ++j)
		{
			pixelBuffer[i][j] = WS_LOW;
		}
	}
	WS281x_Show(); 
}


uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue)
{
	return green << 16 | red << 8 | blue;
}

void WS281x_SetPixelColor(u16 n, u32 GRBColor)
{
	uint8_t i;
	if(n < PIXEL_NUM)
	{
		for(i = 0; i < GRB; i++)
		{
			pixelBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
		}			
	}
}

void WS281x_SetPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; ++i)
    {
      pixelBuffer[n][i] = (((WS281x_Color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
  }
}

//下面函数中语句执行的顺序相当关键，不能错。
//从示波器看，非当前的执行顺序，最后一个灯的波形序列会多。
void WS281x_Show(void)
{
	DMA_SetCurrDataCounter(DMA1_Channel5, (uint16_t)(PIXEL_NUM * GRB));  //设定DMA要传输数据的大小 
	DMA_Cmd(DMA1_Channel5, ENABLE);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);   //CH1预装载器使能
	TIM_Cmd(TIM2, ENABLE);
	while(DMA_GetFlagStatus(DMA1_FLAG_TC5) != SET);    //等待传输完成
	DMA_Cmd(DMA1_Channel5, DISABLE);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable); //关闭CH1预装载器
	TIM_Cmd(TIM2, DISABLE);
	DMA_ClearFlag(DMA1_FLAG_TC5);
}


uint32_t WS281x_Wheel(uint8_t wheelPos) 
{
	wheelPos = 255 - wheelPos;
	if(wheelPos < 85) 
	{
		return WS281x_Color(255 - wheelPos * 3, 0, wheelPos * 3);
	}
	if(wheelPos < 170) 
	{
		wheelPos -= 85;
		return WS281x_Color(0, wheelPos * 3, 255 - wheelPos * 3);
	}
	wheelPos -= 170;
	return WS281x_Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

void WS281x_ColorWipe(u32 color, u8 wait)
{
	u16 i;
	for(i = 0; i < PIXEL_NUM; i ++)
	{
		WS281x_SetPixelColor(i,color);
		WS281x_Show();
		delay_ms(wait);
	}
}

void WS281x_Rainbow(u8 wait)
{
	u16 i,j;
	for(j = 0; j < 256; j ++)
	{
		for(i = 0; i < PIXEL_NUM; i++)
		{
			WS281x_SetPixelColor(i,WS281x_Wheel((i+j)&255));
		}
		WS281x_Show();
		delay_ms(wait);
	}
}

void WS281x_RainbowCycle(uint8_t wait) 
{
	uint16_t i, j;

	for(j=0; j<256*5; j++) 
	{ 
		for(i=0; i< PIXEL_NUM; i++) 
		{
			WS281x_SetPixelColor(i,WS281x_Wheel(((i * 256 / PIXEL_NUM) + j) & 255));
		}
		WS281x_Show();
		delay_ms(wait);
	}
}

void WS281x_TheaterChase(uint32_t c, uint8_t wait) 
{
	int j,q;
	u16 i;
	for (j=0; j<10; j++) 
	{  
		for (q=0; q < 3; q++) 
		{
			for (i=0; i < PIXEL_NUM; i=i+3) 
			{
				WS281x_SetPixelColor(i+q, c);    //turn every third pixel on
			}
			WS281x_Show();

			delay_ms(wait);

			for (i=0; i < PIXEL_NUM; i=i+3) 
			{
				WS281x_SetPixelColor(i+q, 0);        //turn every third pixel off
			}
		}
	}
}

void WS281x_TheaterChaseRainbow(uint8_t wait) 
{
	int j,q;
	u16 i;

	for (j=0; j < 256; j++) 
	{     
		for (q=0; q < 3; q++) 
		{
			for (i=0; i < PIXEL_NUM; i=i+3) 
			{
				WS281x_SetPixelColor(i+q, WS281x_Wheel( (i+j) % 255));    //turn every third pixel on
			}
			WS281x_Show();

			delay_ms(wait);

			for (i=0; i < PIXEL_NUM; i=i+3) 
			{
				WS281x_SetPixelColor(i+q, 0);        //turn every third pixel off
			}
		}
	}
}

static float min(float a, float b, float c)
{
	float m;
  
	m = a < b ? a : b;
	return (m < c ? m : c); 
}

static float max(float a, float b, float c)
{
	float m;
  
	m = a > b ? a : b;
	return (m > c ? m : c); 
}
  
void rgb2hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v)
{
	float red, green ,blue;
	float cmax, cmin, delta;
  
	red = (float)r / 255;
	green = (float)g / 255;
	blue = (float)b / 255;
  
	cmax = max(red, green, blue);
	cmin = min(red, green, blue);
	delta = cmax - cmin;

	/* H */
	if(delta == 0)
	{
		*h = 0;
	}
	else
	{
		if(cmax == red)
		{
			if(green >= blue)
			{
				*h = 60 * ((green - blue) / delta);
			}
			else
			{
				*h = 60 * ((green - blue) / delta) + 360;
			}
		}
		else if(cmax == green)
		{
			*h = 60 * ((blue - red) / delta + 2);
		}
		else if(cmax == blue) 
		{
			*h = 60 * ((red - green) / delta + 4);
		}
	}
  
	/* S */
	if(cmax == 0)
	{
		*s = 0;
	}
	else
	{
		*s = delta / cmax;
	}
  
  /* V */
	*v = cmax;
}
  
void hsv2rgb(float h, float s, float v, uint8_t *g, uint8_t *r, uint8_t *b)
{
	if(s == 0)
      {
		*r=*g=*b=v;
      }
      else
      {
		float H = h / 60;
		int hi = (int)H;
		float f = H - hi;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1- (1 - f) * s);
		switch (hi)
		{
			case 0:
			*r = (int)(v * 255.0 + 0.5);
			*g = (int)(t * 255.0 + 0.5);
			*b = (int)(p * 255.0 + 0.5);
			break;
			case 1:
			*r = (int)(q * 255.0 + 0.5);
			*g = (int)(v * 255.0 + 0.5);
			*b = (int)(p * 255.0 + 0.5);
			break;
			case 2:
			*r = (int)(p * 255.0 + 0.5);
			*g = (int)(v * 255.0 + 0.5);
			*b = (int)(t * 255.0 + 0.5);
			break;
			case 3:
			*r = (int)(p * 255.0 + 0.5);
			*g = (int)(q * 255.0 + 0.5);
			*b = (int)(v * 255.0 + 0.5);
			break;
			case 4:
			*r = (int)(t * 255.0 + 0.5);
			*g = (int)(p * 255.0 + 0.5);
			*b = (int)(v * 255.0 + 0.5);
			case 5:
			*r = (int)(v * 255.0 + 0.5);
			*g = (int)(p * 255.0 + 0.5);
			*b = (int)(q * 255.0 + 0.5);
			break;
		}
	}
}

void WS281x_Breathe(u32 color ,u8 wait)
{
	static uint16_t count = 0;
	uint8_t r,g,b;
	float h, s, v;
	u16 j;
	rgb2hsv((color>>8)&0xFF, (color>>16)&0xFF, (color)&0xFF, &h, &s, &v);
	v = (float)index_wave[count] / 256;
	hsv2rgb(h, s, v, &r, &g, &b);
            
	for(j = 0; j < PIXEL_NUM; ++j)
	{
		WS281x_SetPixelRGB(j, r, g, b);              
      }
      WS281x_Show();
	delay_ms(wait);
	count++;
      if(count >= 300)
	{
             count = 0;
	}   
}


