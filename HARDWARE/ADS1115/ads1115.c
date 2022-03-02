#include "ads1115.h"
#include "delay.h"
#include "usart.h"

#ifdef ADS1115

static ADS1115_InitTypeDefine ADS1115_InitType;

/**
 * 可供外部调用的全局变量，记录了ADS1115采样的原始16位数据。调用void ADS1115_RefreshAllChannel( )函数可以刷新这个变量。
 * 通过定义宏ADS1115_USE_FILTER，可以将ADS1115的轮询采样数据经过滑动滤波后，保存到ADS1115_RawData[]中。
 * 通过float ADS1115_RawDataToVoltage(int16_t rawData)函数可以将ADS1115_RawData[]换算成对应的电压值。
 */
int16_t ADS1115_RawData[4] = {0};


/**
 * @brief 完成芯片控制端口初始化，并设置初始状态
 */
void ADS1115_Init()
{
//	I2C_Virtual_ConfigPort(ADS1115_SDA_PORT, ADS1115_SDA_PIN, ADS1115_SCL_PORT, ADS1115_SCL_PIN);
}


/**
 * @brief Configuration of ADS1115, single-shot
 */
void ADS1115_UserConfig1()
{
	ADS1115_InitType.COMP_LAT = ADS1115_COMP_LAT_0;
	ADS1115_InitType.COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitType.COMP_POL = ADS1115_COMP_POL_0;
	ADS1115_InitType.DataRate = ADS1115_DataRate_475;
	ADS1115_InitType.MODE = ADS1115_MODE_SingleConver;
	ADS1115_InitType.MUX = ADS1115_MUX_Channel_0;
	ADS1115_InitType.OS = ADS1115_OS_SingleConverStart;
	ADS1115_InitType.PGA = ADS1115_PGA_4096;

	ADS1115_Config(&ADS1115_InitType);
}


/**
 * @brief Configuration of ADS1115, continuous conversion
 */
void ADS1115_UserConfig2()
{
	ADS1115_InitType.COMP_LAT = ADS1115_COMP_LAT_0;
	ADS1115_InitType.COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitType.COMP_POL = ADS1115_COMP_POL_0;
	ADS1115_InitType.DataRate = ADS1115_DataRate_475;
	ADS1115_InitType.MODE = ADS1115_MODE_ContinuConver;
	ADS1115_InitType.MUX = ADS1115_MUX_Channel_0;
	ADS1115_InitType.OS = ADS1115_OS_OperationalStatus;
	ADS1115_InitType.PGA = ADS1115_PGA_4096;

	while(!ADS1115_Config(&ADS1115_InitType))
	{
		printf("ADS1115 Initialize Success !");
	}
}


/**
 * @brief 配置ADS1115
 * @param ADS1115_InitStruct: 用来配置ADS1115的结构体变量指针
 * @return 配置结果
 * @arg: fail
 * @arg: success
 */
u8 ADS1115_Config(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{
	u16 Config;
	u8 Writebuff[2];

	Config = ADS1115_InitStruct->OS + ADS1115_InitStruct->MUX + ADS1115_InitStruct->PGA + ADS1115_InitStruct->MODE
			+ ADS1115_InitStruct->DataRate + ADS1115_InitStruct->COMP_MODE + ADS1115_InitStruct->COMP_POL
			+ ADS1115_InitStruct->COMP_LAT + ADS1115_InitStruct->COMP_QUE;

	Writebuff[0] = (unsigned char) ((Config >> 8) & 0xFF);
	Writebuff[1] = (unsigned char) (Config & 0xFF);

	IIC_Start();					//启动总线
	IIC_Send_Byte(ADS1115_ADDRESS_W);		//发送器件地址（写）

	IIC_Wait_Ack();

	IIC_Send_Byte(ADS1115_Pointer_ConfigReg);		//发送寄存器地址

	IIC_Wait_Ack();

	IIC_Send_Byte(Writebuff[0]);		//发送数据

	IIC_Wait_Ack();

	IIC_Send_Byte(Writebuff[1]);		//发送数据

	IIC_Wait_Ack();

	IIC_Stop();

	return (1);
}



/**
 * @brief 读取ADS1115当前通道下的原始数据
 * @param rawData: 传入一个int16_t整型变量的指针，ADS1115的原始数据将保存在这个变量中
 * @return 读取结果
 * 		@arg 0: fail
 * 		@arg 1: success
 */
uint8_t ADS1115_ReadRawData(int16_t *rawData)
{
	unsigned char Result[2];

	IIC_Start();															//启动总线
	IIC_Send_Byte(ADS1115_ADDRESS_W);					//发送器件地址（写）

	IIC_Wait_Ack();
	
	IIC_Send_Byte(ADS1115_Pointer_ConverReg); 	//发送寄存器地址

	IIC_Wait_Ack();

	IIC_Stop();
	delay_us(10);
	IIC_Start();											//写寄存器之后需要重新启动总线

	IIC_Send_Byte(ADS1115_ADDRESS_R);						//发送器件地址（读）

	IIC_Wait_Ack();

	Result[0] = IIC_Read_Byte(1);		//接收数据
	Result[1] = IIC_Read_Byte(0);
	IIC_Stop(); 			//结束总线

	*rawData = (int16_t) (((Result[0] << 8) & 0xFF00) | (Result[1] & 0xFF));

	return 1;
}


/**
 * @brief Switch the channel of ADS1115
 * @param channel
 */
void ADS1115_ScanChannel(uint8_t channel)
{
	switch (channel)
	{
	case 0:
		ADS1115_InitType.MUX = ADS1115_MUX_Channel_0;
		break;
	case 1:
		ADS1115_InitType.MUX = ADS1115_MUX_Channel_1;
		break;
	case 2:
		ADS1115_InitType.MUX = ADS1115_MUX_Channel_2;
		break;
	case 3:
		ADS1115_InitType.MUX = ADS1115_MUX_Channel_3;
		break;
	default:
		break;
	}

	ADS1115_Config(&ADS1115_InitType);
}


/**
 * @brief 将传感器的原始采样数据转化为电压数据，
 * 			根据ADS1115_InitType结构体中包含的增益信息计算
 * @param rawData: 待转换的原始数据
 * @retval 返回经过计算的电压值
 */
float ADS1115_RawDataToVoltage(int16_t rawData)
{
	float voltage;

	switch (ADS1115_InitType.PGA)
	{
	case ADS1115_PGA_0256:
		voltage = rawData * 0.0078125;
		break;

	case ADS1115_PGA_0512:
		voltage = rawData * 0.015625;
		break;

	case ADS1115_PGA_1024:
		voltage = rawData * 0.03125;
		break;

	case ADS1115_PGA_2048:
		voltage = rawData * 0.0625;
		break;

	case ADS1115_PGA_4096:
		voltage = rawData * 0.125;
		break;

	case ADS1115_PGA_6144:
		voltage = rawData * 0.1875;
		break;

	default:
		voltage = 0;
		break;
	}

	return voltage;
}


/**
 * @brief 直接获取ADS1115当前通道的电压采样值
 * @return 电压采样值
 */
float ADS1115_GetVoltage()
{
	int16_t rawData;

	ADS1115_ReadRawData(&rawData);

	return ADS1115_RawDataToVoltage(rawData);
}


/**
 * @brief 获取并计算ADC采样的平均电压值
 * @param num: 计算平均值的数量
 * @retval 电压采样的平均值
 */
float ADS1115_GetAverageVoltage(uint16_t num)
{
	int32_t sum = 0;
	int16_t rawData;
	uint16_t i;
	if(num == 0)
	{
		return ADS1115_GetVoltage();
	}

	for(i =0; i< num;i++)
	{
		ADS1115_ReadRawData(&rawData);
		sum += rawData;
	}

	return ADS1115_RawDataToVoltage(sum/num);
}

/**
 * @brief 刷新ADS1115全部通道的采样数据
 * 		由于ADS1115通道切换后需要等待较长时间数据才能够稳定，
 * 		在进行多路数据采集的时候，切换通道后延时阻塞等待切换完成会占用过多的系统时间，
 * 		因此需要在一个定时器中轮询采集ADS1115数据，每次采集完成后，切换到下一个通道
 * 		大幅度提高了系统工作的效率。
 *
 * 		调用此函数可以刷新全局变量ADS1115_RawData[4]的值。
 *
 * 		应当在一个定时器更新中断服务函数中周期性的调用此函数，更新周期最好小于200Hz
 */
void ADS1115_RefreshAllChannel()
{
	static uint8_t channel = 0;
	int16_t adcDataTemp = 0;

	//通道切换时可能有不确定的数据读出，因此需要将前1~2次读出的数据舍弃
	ADS1115_ReadRawData(&adcDataTemp);
	ADS1115_ReadRawData(&adcDataTemp);

	//读取数据返回正确，则将读到的数据写入ADS1115_RawData数组中
	if( ADS1115_ReadRawData(&adcDataTemp) !=0 )
	{
		ADS1115_RawData[channel] = adcDataTemp;
	}
	
	//ADS1115总共4个通道
	channel++;
	
	if(channel>ADS1115_MAX_CHANNEL-1)
		channel = 0;

	//结束采样后切换至下一通道
	ADS1115_ScanChannel(channel);
}
#endif
