#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


#define	W5500_CS 		PBout(12)  		//W25QXX��Ƭѡ�ź�
 				  	    													  
void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
void SPI2_WriteByte(u8 TxData);
u8 SPI2_ReadByte(void);
void Spi_Writeburst(u8* pBuffer,u16 NumByteToWrite);
void Spi_Readburst(u8* pBuffer,u16 NumByteToWrite);
void CS_Low(void);
void CS_High(void);
void W5500_Init(void);
void Reset_W5500(void);
void SPI_CrisEnter(void);
void SPI_CrisExit(void);
#endif

