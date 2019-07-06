/*
  **********************************  STM8S  ***********************************
  * @�ļ���     �� app.c
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� Ӧ�ó���Դ�ļ�
  ******************************************************************************/
#include "user_comm.h"


uint8_t DevAddress = 0;

static uint32_t tsUART = 0;
static FIFO_t   UART_RecFIFO;
static uint8_t  UART_RecBuffer[UART_FIFO_BUF_SIZE], IC_ReadBuf[20], IC_Read = 0;

/**
 * APP��ʼ��
 */
void App_Init(void)
{
    FIFO_Init(&UART_RecFIFO, UART_RecBuffer, sizeof(UART_RecBuffer));
}

/**
 * ˢ����ѭ
 */
void RFID_Polling(void)
{
    if (RfidReadData(&IC_ReadBuf[4], CARD_BLOCK, 1, 0)) {
        LED_ON(RED);
        memcpy(IC_ReadBuf, MLastSelectedSnr, 4);
        IC_Read = 1;
    } else {
        LED_OFF(RED);
        IC_Read = 0;
    }
}

/**
 * ��������
 *
 * @param cmd
 * @param arg    ���Ͳ���
 */
void SendCmd(uint8_t cmd, uint8_t *data, uint8_t datalen)
{
    uint8_t buf[32], *p = buf, i = 0, len = 5;

    *p++ = 0x7E;
    *p++ = 0x1B;
    *p++ = DevAddress;
    *p++ = cmd;
    *p++ = datalen;
    if (data != NULL && datalen > 0) {
        memcpy(p, data, datalen);
        p += datalen;
        len = len + datalen;
    }
    *p = AddCheck(buf, len);
    len += 1;
    p = buf;

    EN485_Recevie_OFF;

    for (i = 0; i < len; i++) {
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
        UART1_SendData8(*p++);
    }
    while (UART1_GetFlagStatus(UART1_FLAG_TC) == RESET);
    EN485_Recevie_ON;
}

/**
 * ���մ�������
 */
void ReadCmdDeal(void)
{
    uint8_t buf[32], len = 0;

    if (TS_IS_OVER(tsUART, 50)) {
        while (FIFO_Length(&UART_RecFIFO) >= 6 && FIFO_Get(&UART_RecFIFO) != 0x7E);
        if (FIFO_Length(&UART_RecFIFO) >= 5 && FIFO_Get(&UART_RecFIFO) == 0x1B) {
            buf[0] = 0x7E;
            buf[1] = 0x1B;
            FIFO_Read(&UART_RecFIFO, &buf[2], 3);
            if (buf[4] > 0) {
                FIFO_Read(&UART_RecFIFO, &buf[5], buf[4]);
            }
            len = buf[4] + 5;
            buf[len] = FIFO_Get(&UART_RecFIFO);

            if ((DevAddress == buf[2] || buf[2] == 0) && AddCheck(buf, len) == buf[len]) {
                if (buf[3] == CMD_DEVICE_RST) {
                    LED_ON(GREEN);
                    WWDG_SWReset();
                    while (1);
                } else if (buf[3] == CMD_DEVICE_CHK) {
                    LED_ON(GREEN);
                    SendCmd(CMD_DEVICE_CHK_RSP, NULL, 0);
                } else if (buf[3] == CMD_READ_IC) {
                    if (IC_Read) {
                        LED_ON(GREEN);
                        SendCmd(CMD_READ_IC_RSP, IC_ReadBuf, 4);
                    } else {
                        LED_ON(RED);
                        TS_DELAY(100);
                        LED_OFF(RED);
                    }
                } else if (buf[3] == CMD_READ_IC_FRIM) {
                    if (IC_Read) {
                        LED_ON(GREEN);
                        SendCmd(CMD_READ_IC_FRIM_RSP, IC_ReadBuf, 20);
                    } else {
                        LED_ON(RED);
                        TS_DELAY(100);
                        LED_OFF(RED);
                    }
                } else if (buf[3] == CMD_WRITE_FRIM && buf[4] == 16) {
                    LED_ON(GREEN);
                    /*��IC����д�볧����Ϣ*/
                    if (RfidWriteData(CARD_BLOCK, 1, &buf[5])) {
                        LED_ON(RED);
                        TS_DELAY(200);
                        LED_OFF(RED);
                        TS_DELAY(200);
                        IWDG_ReloadCounter();
                        LED_ON(RED);
                        TS_DELAY(200);
                        LED_OFF(RED);
                        TS_DELAY(200);
                        IWDG_ReloadCounter();
                    }
                }
            }
        }
    }

    if (TS_IS_OVER(tsUART, 1000)) {
        LED_OFF(GREEN);
        if (FIFO_Length(&UART_RecFIFO) > 0 && TS_IS_OVER(tsUART, 3000)) {
            FIFO_Flush(&UART_RecFIFO);
        }
    }
}

/**
 * ���ڽ��յ��µ�����
 *
 * @param data   �½��յ�������
 */
void UART_NewData(uint8_t data)
{
    FIFO_Put(&UART_RecFIFO, data);
    TS_INIT(tsUART);
}

/************************************************
�������� �� GET_DeviceAddress
��    �� �� �õ����õ�ַ
��    �� �� ��

�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void GET_DeviceAddress(void)
{
    DevAddress |= KEY1;
    DevAddress |= KEY2 << 1;
    DevAddress |= KEY3 << 2;
    DevAddress |= KEY4 << 3;
    DevAddress += 0xA0;
}





/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
