#include "nrf24l01.h"
#include "spi.h"
#include "delay.h"

void NRF24L01_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
}

u8 NRF24L01_WR_REG(u8 REG,u8 Value)
{
	u8 status;
	
	NRF24L01_CS = 0;
	status = SPI2_ReadWriteByte(REG);
	SPI2_ReadWriteByte(Value);
	NRF24L01_CS = 1;
	
	return status;
}

u8 NRF24L01_Read_REG(u8 REG)
{
	
	u8 REG_Value;
	
	NRF24L01_CS = 0;
	SPI2_ReadWriteByte(REG);
	REG_Value = SPI2_ReadWriteByte(0);
	NRF24L01_CS = 1;
	
	return REG_Value;
}

u8 NRF24L01_Read_BUF(u8 REG, uint8_t *rx_BUF, u8 bytes)
{
	u8 status,byte_ctr;
	
	NRF24L01_CS = 0;
	status = SPI2_ReadWriteByte(REG);
	
	for(byte_ctr = 0 ; byte_ctr < bytes ; byte_ctr++)
	{
		rx_BUF[byte_ctr] = SPI2_ReadWriteByte(0);
	}
	NRF24L01_CS = 1;
	
	return status;
}

u8 NRF24L01_WR_BUF(u8 REG, uint8_t *tx_BUF, u8 bytes)
{
	u8 status,byte_ctr;
	
	NRF24L01_CS = 0;
	status = SPI2_ReadWriteByte(REG);
	
	for(byte_ctr = 0 ; byte_ctr < bytes ; byte_ctr++)
	{
		SPI2_ReadWriteByte(*tx_BUF++);
	}
	
	NRF24L01_CS = 1;
	
	return status;
}

void TX_Mode()
{
	NRF24L01_CE = 0;
	NRF24L01_CS = 1;
	
	NRF24L01_WR_BUF(WRITE_NRF24L01_REG + TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);
	NRF24L01_WR_BUF(WRITE_NRF24L01_REG + RX_ADDR_P1,RX_ADDRESS,TX_ADR_WIDTH);
	
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + EN_AA, 0x03);	
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + EN_RXADDR, 0x03);  //  ������յ�ַֻ��Ƶ��1�������Ҫ��Ƶ�����Բο�Page21  
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + RF_CH, 0);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + RX_PW_P1, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
}

void NRF24L01_TxPacket(u8 *tx_buf)
{
	NRF24L01_CE=0;											//StandBy Iģʽ	
	NRF24L01_WR_BUF(WRITE_NRF24L01_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24L01_WR_BUF(WRITE_NRF24L01_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// ����˵�ַ
	NRF24L01_WR_BUF(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 	// װ������	
	NRF24L01_WR_REG(WRITE_NRF24L01_REG + CONFIG, 0x1e);   		 		// IRQ�շ�����ж���Ӧ��16λCRC��������
	NRF24L01_CE=1;									 		//�ø�CE���������ݷ���
	delay_us(20);	
}
