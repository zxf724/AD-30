#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


#define	W5500_CS 		PBout(12)  		//W25QXX的片选信号
 				  	    													  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
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

