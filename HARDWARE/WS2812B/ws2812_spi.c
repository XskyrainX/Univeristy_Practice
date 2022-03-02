#include "ws2812_spi.h"
#include "delay.h"

/* ���������߱� */
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
* ������min(float a, float b, float c)
* �������������������С��
* ������a,b,c ������
* ���أ���Сֵ
*---------------------------------------------------------------------------*/
static float min(float a, float b, float c)
{
  float m;
  
  m = a < b ? a : b;
  return (m < c ? m : c); 
}
/*---------------------------------------------------------------------------
* ������max(float a, float b, float c)
* ����������������������
* ������a,b,c ������
* ���أ����ֵ
*---------------------------------------------------------------------------*/
static float max(float a, float b, float c)
{
  float m;
  
  m = a > b ? a : b;
  return (m > c ? m : c); 
}
/*---------------------------------------------------------------------------
* ������WS2812_Init(void)
* ������SPI DMA��ʼ��
* ��������
* ���أ���
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
* ������DMA2_Start_SPI_TX(void)
* ������DMA SPI��������
* ��������
* ���أ���
*---------------------------------------------------------------------------*/
void DMA2_Start_SPI_TX()
{
	DMA2_Channel2->CNDTR = (PixelNumber*24+310);
	DMA2_Channel2->CMAR = (uint32_t)PixelBuffer;

	DMA_Cmd(DMA2_Channel2,ENABLE);
	while(!DMA_GetFlagStatus(DMA2_FLAG_TC2)); //ѭ���ȴ�������ɣ���ʱ������жϴ�ϣ�����Ӱ�췢��
	DMA_Cmd(DMA2_Channel2,DISABLE); 
	DMA_ClearFlag(DMA2_FLAG_TC2); 	  
	return;
}
/*---------------------------------------------------------------------------
* ������Set_All_Pixel_Color(uint8_t r, uint8_t g, uint8_t b)
* ����������ÿ����
* ������r g b ָRGB��ֵ
* ���أ���
*---------------------------------------------------------------------------*/
void Set_All_Pixel_Color(uint8_t r, uint8_t g, uint8_t b)
{
	u8 i=0;
	for (i = 0; i < PixelNumber; i++)//�ƴ�����30������
	{
        Ws281x_Set_Pixel(Color_Show(r,g,b),i);
	}
}
/*---------------------------------------------------------------------------
* ������Ws281x_Set_Pixel(uint32_t color,uint32_t position)
* ���������õ�������ɫ
* ������color����ɫ position:λ��
* ���أ���
*---------------------------------------------------------------------------*/
void Ws281x_Set_Pixel(uint32_t color,uint32_t position)
{
	unsigned int positionin=position*3;//һ������3����ɫ
	uint8_t Red, Green, Blue; 
	
	Red = color>>16;
	Green = color>>8;
	Blue = color;
	
	Ws281x_Set_Bits(Green,positionin);
	Ws281x_Set_Bits(Red,positionin+1);
	Ws281x_Set_Bits(Blue,positionin+2);
}
/*---------------------------------------------------------------------------
* ������Ws281x_Set_Bits(uint8_t bits,uint32_t position)
* ���������õ����Ƶ�һ���ֽ�
* ������bits:��ɫ position:λ��
* ���أ���
*---------------------------------------------------------------------------*/
void Ws281x_Set_Bits(uint8_t bits,uint32_t position)
{
	unsigned int positionin=0;	
	int zero = 0xe0; //11100000
	int one = 0xfC;  //11111100
	int i = 0x00;
	int j = 0x00;	
	positionin=position*8;//һ�������ϵ�һ����ɫ,��Ҫ8λ����ʾ����
	for (i = 0x80; i >= 0x01; i >>= 1)
	{			
		PixelBuffer[positionin+j]=((bits & i) ? one : zero);
		j++;
	}
}
/*---------------------------------------------------------------------------
* ������Color_Show(uint8_t r, uint8_t g, uint8_t b)
* ���������RGB3�ֽڱ���
* ������r g b
* ���أ�R G B
*---------------------------------------------------------------------------*/
uint32_t Color_Show(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
/*---------------------------------------------------------------------------
* ������Colourful_Wheel(u8 WheelPos)
* �������ṩ�ʺ�ѭ����RGBֵ
* ������WheelPos:RGBֵ����
* ���أ�Color:�ʺ�ѭ��RGBֵ
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
* ������rgb2hsv(uint32_t color, float *h, float *s, float *v)
* ������RGBת����HSVɫ�ʿռ�
* ������color:RGBɫ��ֵ    h,s,v:H,S,V��ŵ�ַ
* ���أ�H,S,Vת��ֵ
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
* ������hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
* ������HSVת����RGBɫ�ʿռ�
* ������h,s,v:��ת��hsvֵ    *r,*g,*b:ת����RGBֵ�洢��ַ
* ���أ�R,G,Bת��ֵ
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
* ������RainbowCycle(u8 PixelLength)
* �������ʺ�ѭ������
* ������PixelLength:���ظ���
* ���أ�
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

