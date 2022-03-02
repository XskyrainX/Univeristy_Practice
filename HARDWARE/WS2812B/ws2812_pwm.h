#ifndef __BSP_WS281X_H
#define __BSP_WS281X_H

#include "stm32f10x.h"

#define PIXEL_NUM  30
#define GRB  24   //3*8

#define WS_HIGH 45
#define WS_LOW  30

void WS281x_Init(void);
void WS281x_CloseAll(void);
uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelColor(uint16_t n, uint32_t GRBColor);
void WS281x_SetPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);
void WS281x_Show(void);

void WS281x_RainbowCycle(uint8_t wait);
void WS281x_TheaterChase(uint32_t c, uint8_t wait);
void WS281x_ColorWipe(uint32_t c, uint8_t wait);
void WS281x_Rainbow(uint8_t wait);
void WS281x_TheaterChaseRainbow(uint8_t wait);
static float min(float a, float b, float c);
static float max(float a, float b, float c);
void rgb2hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v);
void hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b);
void WS281x_Breathe(u32 color ,u8 wait);
#endif /* __BSP_WS281X_H */

