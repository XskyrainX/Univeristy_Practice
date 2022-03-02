#include "ws2812_spi.h"
#include "delay.h"

/* 呼吸灯曲线表 */
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

unsigned char PixelBuffer[PixelNumber*24+310] = {0};
RGBColor_TypeDef Color;
/*---------------------------------------------------------------------------
* 函数：min(float a, float b, float c)
* 描述：求出三变量中最小项
* 参数：a,b,c 三变量
* 返回：最小值
*---------------------------------------------------------------------------*/
static float min(float a, float b, float c)
{
  float m;
  
  m = a < b ? a : b;
  return (m < c ? m : c); 
}
/*---------------------------------------------------------------------------
* 函数：max(float a, float b, float c)
* 描述：求出三变量中最大项
* 参数：a,b,c 三变量
* 返回：最大值
*---------------------------------------------------------------------------*/
static float max(float a, float b, float c)
{
  float m;
  
  m = a > b ? a : b;
  return (m > c ? m : c); 
}
/*---------------------------------------------------------------------------
* 函数：WS2812_Init(void)
* 描述：SPI DMA初始化
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void WS2812_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	SPI_I2S_DeInit(SPI3);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CRCPolynomial = 10;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI3,&SPI_InitStruct);
	
	SPI3->CR1 &=~(1<<13);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
	DMA_InitStruct.DMA_BufferSize = 0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)PixelBuffer;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA2_Channel2,&DMA_InitStruct);
	
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPI3,ENABLE);
}
/*---------------------------------------------------------------------------
* 函数：DMA2_Start_SPI_TX(void)
* 描述：DMA SPI传送数据
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
void DMA2_Start_SPI_TX()
{
	DMA2_Channel2->CNDTR = (PixelNumber*24+310);
	DMA2_Channel2->CMAR = (uint32_t)PixelBuffer;

	DMA_Cmd(DMA2_Channel2,ENABLE);
	while(!DMA_GetFlagStatus(DMA2_FLAG_TC2)); //循环等待发送完成，此时如果被中断打断，并不影响发送
	DMA_Cmd(DMA2_Channel2,DISABLE); 
	DMA_ClearFlag(DMA2_FLAG_TC2); 	  
	return;
}
/*---------------------------------------------------------------------------
* 函数：Set_All_Pixel_Color(uint8_t r, uint8_t g, uint8_t b)
* 描述：设置每个灯
* 参数：r g b 指RGB数值
* 返回：无
*---------------------------------------------------------------------------*/
void Set_All_Pixel_Color(uint8_t r, uint8_t g, uint8_t b)
{
	u8 i=0;
	for (i = 0; i < PixelNumber; i++)//灯带上有30个灯珠
	{
        Ws281x_Set_Pixel(Color_Show(r,g,b),i);
	}
}
/*---------------------------------------------------------------------------
* 函数：Ws281x_Set_Pixel(uint32_t color,uint32_t position)
* 描述：设置单个灯颜色
* 参数：color：颜色 position:位置
* 返回：无
*---------------------------------------------------------------------------*/
void Ws281x_Set_Pixel(uint32_t color,uint32_t position)
{
	unsigned int positionin=position*3;//一个灯珠3种颜色
	uint8_t Red, Green, Blue; 
	
	Red = color>>16;
	Green = color>>8;
	Blue = color;
	
	Ws281x_Set_Bits(Green,positionin);
	Ws281x_Set_Bits(Red,positionin+1);
	Ws281x_Set_Bits(Blue,positionin+2);
}
/*---------------------------------------------------------------------------
* 函数：Ws281x_Set_Bits(uint8_t bits,uint32_t position)
* 描述：设置单个灯的一个字节
* 参数：bits:颜色 position:位置
* 返回：无
*---------------------------------------------------------------------------*/
void Ws281x_Set_Bits(uint8_t bits,uint32_t position)
{
	unsigned int positionin=0;	
	int zero = 0xe0; //11100000
	int one = 0xfC;  //11111100
	int i = 0x00;
	int j = 0x00;	
	positionin=position*8;//一个灯珠上的一种颜色,需要8位数表示亮度
	for (i = 0x80; i >= 0x01; i >>= 1)
	{			
		PixelBuffer[positionin+j]=((bits & i) ? one : zero);
		j++;
	}
}
/*---------------------------------------------------------------------------
* 函数：Color_Show(uint8_t r, uint8_t g, uint8_t b)
* 描述：组成RGB3字节变量
* 参数：r g b
* 返回：R G B
*---------------------------------------------------------------------------*/
uint32_t Color_Show(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
/*---------------------------------------------------------------------------
* 函数：Colourful_Wheel(u8 WheelPos)
* 描述：提供彩虹循环的RGB值
* 参数：WheelPos:RGB值算子
* 返回：Color:彩虹循环RGB值
*---------------------------------------------------------------------------*/
u32 Colourful_Wheel(u8 WheelPos)
{
	u32 Color;
	uint8_t Red, Green,Blue; 
	WheelPos = 255 - WheelPos;
	if(WheelPos<85)
	{
		Red = 255 - WheelPos * 3;
		Green = 0;
		Blue = WheelPos * 3;
	}
	else if(WheelPos<170)
	{
		WheelPos -= 85;
		Red = WheelPos * 3;
		Green = 255 - WheelPos;
		Blue = 0;
	}
	else
	{
		WheelPos -= 170;
		Red = 0;
		Green = WheelPos * 3;
		Blue = 255 - WheelPos;
	}
	Color = Color_Show(Red,Green,Blue);
	return Color;
}
/*---------------------------------------------------------------------------
* 函数：rgb2hsv(uint32_t color, float *h, float *s, float *v)
* 描述：RGB转换成HSV色彩空间
* 参数：color:RGB色彩值    h,s,v:H,S,V存放地址
* 返回：H,S,V转换值
*---------------------------------------------------------------------------*/
void rgb2hsv(uint32_t color, float *h, float *s, float *v)
{
	uint8_t r,g,b;
	float red, green ,blue;
	float cmax, cmin, delta;
	
	r = color>>16;
	g = color>>8;
	b = color;
	
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
/*---------------------------------------------------------------------------
* 函数：hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
* 描述：HSV转换成RGB色彩空间
* 参数：h,s,v:待转换hsv值    *r,*g,*b:转换后RGB值存储地址
* 返回：R,G,B转换值
*---------------------------------------------------------------------------*/  
void hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
	int hi = ((int)h / 60) % 6;
	float f = h * 1.0 / 60 - hi;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1- (1 - f) * s);
	switch (hi){
		case 0:
			*r = 255 * v;
			*g = 255 * t;
			*b = 255 * p;
		break;
		case 1:
			*r = 255 * q;
			*g = 255 * v;
			*b = 255 * p;
		break;
		case 2:
			*r = 255 * p;
			*g = 255 * v;
			*b = 255 * t;
		break;
		case 3:
			*r = 255 * p;
			*g = 255 * q;
			*b = 255 * v;
		break;
		case 4:
			*r = 255 * t;
			*g = 255 * p;
			*b = 255 * v;
		break;
		case 5:
			*r = 255 * v;
			*g = 255 * p;
			*b = 255 * q;
            break;
    }
}
/*---------------------------------------------------------------------------
* 函数：RainbowCycle(u8 PixelLength)
* 描述：彩虹循环函数
* 参数：PixelLength:像素个数
* 返回：
*---------------------------------------------------------------------------*/  
void RainbowCycle(u8 PixelLength)
{
	u16 i,j;
//	u8 R,G,B;
//	float H,S,V;
//	uint8_t *r,*g,*b;
//	float *h,*s,*v;
//	r=&R;g=&G;b=&B;h=&H;s=&S;v=&V;

	for(j = 0; j < 1024; j++)
	{
		for(i=0; i< PixelLength; i++)
			Ws281x_Set_Pixel(Colourful_Wheel(((i * 256 / PixelLength) + j)&255),i);
//		{
//			rgb2hsv(Colourful_Wheel(((i * 256 / PixelLength) + j)&255),h,s,v);
//			V=index_wave[10*i];
//			hsv2rgb(H,S,V,r,g,b);
//			Ws281x_Set_Pixel(Color_Show(R,G,B),i);
//		}
		DMA2_Start_SPI_TX();
		delay_ms(5);
	}
	
}

