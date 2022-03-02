#ifndef __ATGM332D5N31_H
#define __ATGM332D5N31_H
#include "sys.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收

#define false 0
#define true 1


//定义数组长度
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2 

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//是否获取到GPS数据
	char isParseData;	//是否解析完成
	char UTCTime[UTCTime_Length];		//UTC时间
	u8 latitude[latitude_Length];		//纬度
	char N_S[N_S_Length];		//N/S
	u8 longitude[longitude_Length];		//经度
	char E_W[E_W_Length];		//E/W
	char isUsefull;		//定位信息是否有效
} _SaveData;

extern char rxdatabufer;
extern u16 point1;
extern _SaveData Save_Data;


void GPS_Init(u32 bound);
void parseGpsBuffer(void);
void printGpsBuffer(void);
void errorLog(int num);

void CLR_Buf(void);
u8 Hand(char *a);
void clrStruct(void);


extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	


#endif


