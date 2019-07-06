/*
  **********************************  STM8S  ***********************************
  * @�ļ���     �� uart.c
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� UARTԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-09-02 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "uart.h"


/************************************************
�������� �� UART_Initializes
��    �� �� UART��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void UART_Initializes(void)
{
#if 0
  UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  UART1_Cmd(ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);

#else
  UART1->BRR2 = 0x02;
  UART1->BRR1 = 0x68;
//UART1->CR1  = 0x00;
  UART1->CR2  = 0x2C;
//UART1->SR   = 0xC0;
#endif
}

/************************************************
�������� �� UART1_SendByte
��    �� �� UART1����һ���ַ�
��    �� �� Data --- ����
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void UART1_SendByte(uint8_t Data)
{
  while((UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET));
  UART1_SendData8(Data);
}


/************************************************
�������� �� UART1_SendNByte
��    �� �� ����1����N���ַ�
��    �� �� pData ----- �ַ���
            Length --- ����
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void UART1_SendNByte(uint8_t *pData, uint16_t Length)
{
  while(Length--)
  {
    UART1_SendByte(*pData);
    pData++;
  }
}

/************************************************
�������� �� UART1_Printf
��    �� �� ����1��ӡ���
��    �� �� string --- �ַ���
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void UART1_Printf(uint8_t *String)
{
  while((*String) != '\0')
  {
    UART1_SendByte(*String);
    String++;
  }
}

#if 1
/******************************************* �����ض��� *******************************************/
/************************************************
�������� �� fputc
��    �� �� �ض���putc����
��    �� �� ch --- �ַ�
            *f --- �ļ�ָ��
�� �� ֵ �� �ַ�
��    �� �� Huang Fugui
*************************************************/
int fputc(int ch, FILE *f)
{
  UART1_SendByte((uint8_t)ch);

  return ch;
}

/************************************************
�������� �� fgetc
��    �� �� �ض���getc����
��    �� �� *f --- �ļ�ָ��
�� �� ֵ �� �ַ�
��    �� �� Huang Fugui
*************************************************/
int fgetc(FILE *f)
{
  while(UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);

  return (int)UART1_ReceiveData8();
}
#endif


/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
