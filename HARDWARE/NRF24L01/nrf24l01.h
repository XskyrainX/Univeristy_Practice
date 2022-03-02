#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "sys.h"
/****************************************NRF24L01�˿ڶ���***************************************/
#define NRF24L01_IRQ PGin(6)
#define NRF24L01_CS PGout(7)
#define NRF24L01_CE PGout(8)

/*********************************************NRF24L01*************************************/

#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x35,0x43,0x10,0x10,0x03}; 	//���ص�ַ
uint8_t RX_ADDRESS[RX_ADR_WIDTH]= {0x35,0x43,0x10,0x10,0x03};	//���յ�ַ

/***************************************NRF24L01�Ĵ���ָ��*******************************************************/
#define READ_NRF24L01_REG        	0x00  	// ���Ĵ���ָ��
#define WRITE_NRF24L01_REG       	0x20 		// д�Ĵ���ָ��
#define RD_RX_PLOAD     		0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     		0xA0  	// д��������ָ��
#define FLUSH_TX        		0xE1 		// ��ϴ���� FIFOָ��
#define FLUSH_RX        		0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     		0xE3  	// �����ظ�װ������ָ��
#define NOP             		0xFF  	// ����

/*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************/

#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������

void NRF24L01_GPIO_Init(void);
u8 NRF24L01_WR_REG(u8 REG,u8 Value);
u8 NRF24L01_Read_REG(u8 REG);
u8 NRF24L01_Read_BUF(u8 REG, u8 *pBUF, u8 bytes);
u8 NRF24L01_WR_BUF(u8 REG, u8 *pBUF, u8 bytes);
void TX_Mode(void);
void NRF24L01_TxPacket(u8 *tx_buf);
void RX_Mode(void);

#endif

