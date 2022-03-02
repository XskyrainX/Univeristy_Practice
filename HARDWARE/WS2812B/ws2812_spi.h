#ifndef __WS2812_H
#define __WS2812_H
#include "sys.h"

#define PixelNumber 30

typedef struct
{
	u8 R;
	u8 G;
	u8 B;
}RGBColor_TypeDef;

static float min(float a, float b, float c);
static float max(float a, float b, float c);

void WS2812_Init(void);
void DMA2_Start_SPI_TX(void);
void Set_All_Pixel_Color(uint8_t r, uint8_t g, uint8_t b);
void Ws281x_Set_Pixel(uint32_t color,uint32_t position);
void Ws281x_Set_Bits(uint8_t bits,uint32_t position);
uint32_t Color_Show(uint8_t r, uint8_t g, uint8_t b);
u32 Colourful_Wheel(u8 WheelPos);
void rgb2hsv(uint32_t color, float *h, float *s, float *v);					 
void HSVtoRGB(unsigned char *r, unsigned char *g, unsigned char *b, float h, float s, float v);
void RainbowCycle(u8 PixelLength);

#endif


