#ifndef __ATGM332D5N31_H
#define __ATGM332D5N31_H
#include "sys.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define false 0
#define true 1


//�������鳤��
#define GPS_Buffer_Length 80
#define UTCTime_Length 11
#define latitude_Length 11
#define N_S_Length 2
#define longitude_Length 12
#define E_W_Length 2 

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//�Ƿ��ȡ��GPS����
	char isParseData;	//�Ƿ�������
	char UTCTime[UTCTime_Length];		//UTCʱ��
	u8 latitude[latitude_Length];		//γ��
	char N_S[N_S_Length];		//N/S
	u8 longitude[longitude_Length];		//����
	char E_W[E_W_Length];		//E/W
	char isUsefull;		//��λ��Ϣ�Ƿ���Ч
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


extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	


#endif


