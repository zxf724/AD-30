/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� app.h
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� Ӧ�ó���ͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _APP_H
#define _APP_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8s.h"


/* �궨�� --------------------------------------------------------------------*/

#define UART_FIFO_BUF_SIZE      128


/*Э������*/
#define CMD_DEVICE_RST          0xFF
#define CMD_DEVICE_CHK          0x01
#define CMD_DEVICE_CHK_RSP      0x11
#define CMD_READ_IC             0x02
#define CMD_READ_IC_RSP         0x12
#define CMD_READ_IC_FRIM        0x03
#define CMD_READ_IC_FRIM_RSP    0x13
#define CMD_WRITE_ID            0x04
#define CMD_WRITE_FRIM          0x14

#define CARD_TYPE_USER          0x01                     //������ - ��ͨ��

#define CARD_BLOCK              1                        //����������һ������
#define START_BLOCK             1
#define CARD_BLOCK_COUN         4                        //�����������ڿ����ڿ�-��ʼ��(��ȡ��ͨ��ʣ�����)



/* �������� ------------------------------------------------------------------*/
void App_Init(void);

void RFID_Polling(void);
void SendCmd(uint8_t cmd, uint8_t *data, uint8_t datalen);
void ReadCmdDeal(void);
void UART_NewData(uint8_t data);
void GET_DeviceAddress(void);
void SendID_Data(void);


#endif /* _APP_H */

/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
