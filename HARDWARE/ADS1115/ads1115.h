#ifndef __ADS1115_H__
#define __ADS1115_H__

#include "sys.h"

#define ADS1115

#ifdef ADS1115
#include "iic.h"

#define ADS1115_MAX_CHANNEL 4

#define Accuracy 							32768	//ADC单端输入15位精度
#define ADS1115_ADDRESS_0					0x90	//ADDR PIN ->GND
#define ADS1115_ADDRESS_1					0x92	//ADDR PIN ->VDD
#define ADS1115_ADDRESS_2					0x94	//ADDR PIN ->SDA
#define ADS1115_ADDRESS_3					0x96	//ADDR PIN ->SCL

//*************ADDR Initial********************/
#define ADS1115_ADDRESS		ADS1115_ADDRESS_0		//ADDR PIN ->GND
#define ADS1115_ADDRESS_W	ADS1115_ADDRESS|0x00	//write address
#define ADS1115_ADDRESS_R	ADS1115_ADDRESS|0x01	//read address

/************POINTER REGISTER*****************/
#define ADS1115_Pointer_ConverReg			0x00	//Convertion register
#define ADS1115_Pointer_ConfigReg			0x01	//Config register
#define ADS1115_Pointer_LoThreshReg			0x02	//Lo_thresh register
#define ADS1115_Pointer_HiThreshReg			0x03	//Hi_thresh register

/************CONFIG REGISTER*****************/

//Bit[15]
#define ADS1115_OS_OperationalStatus		0x0000		//No Effect
#define ADS1115_OS_SingleConverStart		0x8000		//Begin a single conversion
//Bits[14:12]
#define ADS1115_MUX_Differ_01				0x0000		//AINp=AIN0, AINn=AIN1(default)
#define ADS1115_MUX_Differ_03				0x1000		//AINp=AIN0, AINn=AIN3
#define ADS1115_MUX_Differ_13				0x2000		//AINp=AIN1, AINn=AIN3
#define ADS1115_MUX_Differ_23				0x3000		//AINp=AIN2, AINn=AIN3
#define ADS1115_MUX_Channel_0				0x4000		//AINp=AIN0, AINn=GND
#define ADS1115_MUX_Channel_1				0x5000		//AINp=AIN1, AINn=GND
#define ADS1115_MUX_Channel_2				0x6000		//AINp=AIN2, AINn=GND
#define ADS1115_MUX_Channel_3				0x7000		//AINp=AIN3, AINn=GND
//Bits[11:9]
#define ADS1115_PGA_6144					0x0000		//FS=6.144V
#define ADS1115_PGA_4096					0x0200		//FS=4.096V
#define ADS1115_PGA_2048					0x0400		//FS=2.048V(default)
#define ADS1115_PGA_1024					0x0600		//FS=1.024V
#define ADS1115_PGA_0512					0x0800		//FS=0.512V
#define ADS1115_PGA_0256					0x0A00		//FS=0.256V
//Bit[8]
#define ADS1115_MODE_ContinuConver			0x0000		//Continuous conversion mode
#define ADS1115_MODE_SingleConver			0x0100		//Power-down single-shot mode(default)
//Bits[7:5]
#define ADS1115_DataRate_8					0x0000		//Data Rate = 8
#define ADS1115_DataRate_16					0x0020		//Data Rate = 16
#define ADS1115_DataRate_32					0x0040		//Data Rate = 32
#define ADS1115_DataRate_64					0x0060		//Data Rate = 64
#define ADS1115_DataRate_128				0x0080		//Data Rate = 128(default)
#define ADS1115_DataRate_250				0x00A0		//Data Rate = 250
#define ADS1115_DataRate_475				0x00C0		//Data Rate = 475
#define ADS1115_DataRate_860				0x00E0		//Data Rate = 860
//Bit[4]
#define ADS1115_COMP_MODE_0					0x0000		//Traditional comparator with hysteresis
#define ADS1115_COMP_MODE_1					0x0010		//Window comparator
//Bit[3]
#define ADS1115_COMP_POL_0					0x0000		//Active low
#define ADS1115_COMP_POL_1					0x0008		//Active high
//Bit[2]
#define ADS1115_COMP_LAT_0					0x0000		//Non-latching comparator
#define ADS1115_COMP_LAT_1					0x0004		//Latching comparator
//Bits[1:0]
#define ADS1115_COMP_QUE_0					0x0000		//Assert after one conversion
#define ADS1115_COMP_QUE_1					0x0001		//Assert after two conversion
#define ADS1115_COMP_QUE_2					0x0002		//Assert after four conversion
#define ADS1115_COMP_QUE_3					0x0003		//Disable Comparator

typedef struct
{
    u16 OS;
    u16 MUX;
    u16 PGA;
    u16 MODE;
    u16 DataRate;
    u16 COMP_MODE;
    u16 COMP_POL;
    u16 COMP_LAT;
    u16 COMP_QUE;
} ADS1115_InitTypeDefine;

extern int16_t ADS1115_RawData[4];

void ADS1115_Init(void);
void ADS1115_UserConfig1(void);
void ADS1115_UserConfig2(void);
u8 ADS1115_Config(ADS1115_InitTypeDefine* ADS1115_InitStruct);

u8 ADS1115_ReadRawData(int16_t* rawData);
void ADS1115_ScanChannel(uint8_t channel);
float ADS1115_RawDataToVoltage(int16_t rawData);

float ADS1115_GetVoltage(void);
float ADS1115_GetAverageVoltage(uint16_t num);

void ADS1115_RefreshAllChannel(void);

#endif
#endif
