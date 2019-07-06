/*
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp.c
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� �弶֧�ְ�Դ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-09-02 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp.h"

uint32_t TS_Index = 0;

/************************************************
�������� �� CLK_Configuration
��    �� �� ʱ������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void CLK_Configuration(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //HSI = 16M (1��Ƶ)
}

/************************************************
�������� �� GPIO_Basic_Configuration
��    �� �� ����IO��������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void GPIO_Basic_Configuration(void)
{
    GPIO_Init(LED_RED_GPIO_Port, (GPIO_Pin_TypeDef)LED_RED_Pin, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(LED_GREEN_GPIO_Port, (GPIO_Pin_TypeDef)LED_GREEN_Pin, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(PORT_485EN, (GPIO_Pin_TypeDef)PIN_485EN, GPIO_MODE_OUT_PP_LOW_FAST);

    GPIO_Init(PORT_KEY1, (GPIO_Pin_TypeDef)PIN_KEY1, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(PORT_KEY2, (GPIO_Pin_TypeDef)PIN_KEY2, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(PORT_KEY3, (GPIO_Pin_TypeDef)PIN_KEY3, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(PORT_KEY4, (GPIO_Pin_TypeDef)PIN_KEY4, GPIO_MODE_IN_PU_NO_IT);

    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_6), GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    /* ϵͳʱ��������� */
#if 0
    GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
    CLK_CCOCmd(ENABLE);
#endif
}

/************************************************
�������� �� BSP_Initializes
��    �� �� �弶֧�ְ���ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void BSP_Initializes(void)
{
    CLK_Configuration();
    GPIO_Basic_Configuration();
}


/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
