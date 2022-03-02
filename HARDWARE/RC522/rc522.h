#ifndef __RC522_H
#define __RC522_H
#include "stm32f10x.h"

#define MI_EXE_ERR			98

#define MI_OK           0

#define MI_NOTAGERR     1



#define DEF_FIFO_LENGTH 64   //MF522 FIFO���ȶ���

#define Key1 0x5c
#define Key2 0xb6
#define MAXRLEN	16

//M1��������
#define  MIFARE_1K_4BYTE_UID       0x01
#define  MIFARE_1K_7BYTE_UID       0x02
#define  MIFARE_ULTRALIGHT          0x03
#define  MIFARE_4K_4BYTE_UID      0x04
#define  MIFARE_4K_7BYTE_UID     0x05
#define  MIFARE_PRO       0x07
#define  MIFARE_DESFIRE_7BYTE_UID  0x06
#define  OTHER      0x0a

/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������ (M1��)
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

//MF522�Ĵ�������
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01//������ֹͣ����ִ��  
#define     ComIEnReg             0x02//���úͽ����ж��������λ
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04//�ж�����λ
#define     DivIrqReg             0x05//�����ж������־
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08//���ջ��ͷ����״̬λ 
#define     FIFODataReg           0x09//64�ֽ�FIFO���������������
#define     FIFOLevelReg          0x0A//�洢��FIFO�������е��ֽ���
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D//����λ��֡����
#define     CollReg               0x0E//������RF�ӿ��ϼ�⵽�ĵ�һλ��ͻ��
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11//���巢�ͺͽ��յĳ���ģʽ
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14//������������������TX1��TX2���߼���Ϊ
#define     TxAskReg              0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17//ѡ���ڲ��Ľ���������
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21//��ʾCRC�����ʵ��MSB��LSBֵ��
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26//���ý���������
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A//�����ڲ���ʱ��������
#define     TPrescalerReg         0x2B//
#define     TReloadRegH           0x2C//���� 16 λ���Ķ�ʱ����װֵ
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E//��ʾ 16 λ����ʵ�ʶ�ʱ��ֵ
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  0x3F

void spi2_init(void);//SPI��ʼ������
u8 spi2_readwritebyte(unsigned char txdata);//SPI���ղ�����һ������
void spi_setspeed(unsigned char  SPI_BaudRatePrescaler);//���ò�����
void trans(unsigned char* table,unsigned char* table1,unsigned char num);//�����������ݸ�������

void RFIDGPIO_Init(void);//RFID_RC522 GPIO SPI��ʼ��
char PcdReset(void);//��λRC522
void PcdAntennaOn(void);//��������  
void PcdAntennaOff(void);//�ر�����
char M500PcdConfigISOType(unsigned char type);//����RC632�Ĺ�����ʽ
char PcdHalt(void);//���Ƭ��������״̬

void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);//��MF522����CRC16����
unsigned char ReadRawRC(unsigned char Address);//��RC523�Ĵ���
void WriteRawRC(unsigned char Address, unsigned char value);//дRC523�Ĵ���
void SetBitMask(unsigned char reg,unsigned char mask);//��RC522�Ĵ���λ						 
void ClearBitMask(unsigned char reg,unsigned char mask);//��RC522�Ĵ���λ
char PcdComMF522(unsigned char Command,//ͨ��RC522��ISO14443��ͨѶ
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit);
								 						 
char PcdRequest(unsigned char req_code,unsigned char *pTagType);//Ѱ��
char PcdAnticoll(unsigned char *pSnr);//����ײ
char PcdSelect(unsigned char *pSnr);//ѡ����Ƭ
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//��֤��Ƭ����   
char PcdRead(unsigned char addr,unsigned char *pData);//��ȡM1��һ������   
char PcdWrite(unsigned char addr,unsigned char *pData);//дM1��һ������ 
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue);//M1�ۿ�ͳ�ֵ
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);//M1������Ǯ�� 

void MoveBits(unsigned char  *Source,unsigned char  Len,unsigned char  BitCnt);//�ƶ�
void DataDecrypt(unsigned char  *Source,unsigned char  Len);//����
	
void Printing(unsigned char* Buffer,unsigned short len);//�������鵽����		     
u8 Request_Anticoll_Select(u8 request_mode,u16 card_type,u8* card_buffer,u8* card_id);//Ѱ��������ײ��ѡ��
void User_Regist(void);
void User_Verify(void);
void User_Logout(void);

#endif
