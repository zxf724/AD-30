/*
  **********************************  STM8S  ***********************************
  * @�ļ���     �� timer.c
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� TIMERԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-09-02 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "timer.h"
#include "global_variable.h"


/************************************************
�������� �� TIMER_Initializes
��    �� �� ��ʱ����ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIMER_Initializes(void)
{
#if 0
                                                  //(��ʱ100����)
  TIM1_TimeBaseInit(16000, TIM1_COUNTERMODE_UP, 100, 0);
  TIM1->IER |= (uint8_t)TIM1_IT_UPDATE;          //�ж�����

  TIM2_TimeBaseInit(TIM2_PRESCALER_16, 1000);    //16M/16 / 1000 = 1000(��ʱ1����)
  TIM2->IER |= (uint8_t)TIM2_IT_UPDATE;          //�ж�����

                                                 //TIM4��ʱ10us (2��Ƶ=8M  ����80������)
  TIM4_TimeBaseInit(TIM4_PRESCALER_2, 79);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);

#else
  TIM1->ARRH  = 0x00;
  TIM1->ARRL  = 0x64;
  TIM1->PSCRH = 0x3E;
  TIM1->PSCRL = 0x80;
  TIM1->IER   = 0x01;

  TIM2->PSCR  = 0x04;
  TIM2->ARRH  = 0x03;
  TIM2->ARRL  = 0xE8;
  TIM2->IER   = 0x01;

  TIM4->PSCR  = 0x01;
  TIM4->ARR   = 0x4F;
#endif

  TIM1->CR1 |= (uint8_t)TIM1_CR1_CEN;            //������ʱ��1�����1���ж�һ�Σ�
}

/************************************************
�������� �� TIM2_Timing_Nms
��    �� �� ��ʱNS
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIM2_Timing_Nms(uint16_t Times)
{
  gTIMTiming2_Num = Times;
  gTIMTiming2_FlagOver = FLAG_N_VALID;
  TIM2->CNTRH  = 0x00;
  TIM2->CNTRL  = 0x00;
  TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN;            //������ʱ��
}

/************************************************
�������� �� TIM2_Timing_Off
��    �� �� �رն�ʱ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIM2_Timing_Off(void)
{
  gTIMTiming2_Num = 0;
  gTIMTiming2_FlagOver = FLAG_N_VALID;
  TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN);         //�رն�ʱ��
}

/************************************************
�������� �� TIMDelay_N10us
��    �� �� ��ʱ����ʱN��10us
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIMDelay_N10us(uint16_t Times)
{
  TIM4->CR1 |= (uint8_t)TIM4_CR1_CEN;            //������ʱ��
  while(Times--)
  {
    while((TIM4->SR1 & TIM4_FLAG_UPDATE) == RESET);
    TIM4->SR1 &= ~TIM4_FLAG_UPDATE;
  }
  TIM4->CR1 &= (uint8_t)(~TIM4_CR1_CEN);         //�رն�ʱ��
}

/************************************************
�������� �� TIMDelay_Nms
��    �� �� ��ʱ����ʱNms
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIMDelay_Nms(uint16_t Times)
{
  while(Times--)
  {
    TIMDelay_N10us(100);
  }
}

/************************************************
�������� �� TIMDelay_Ns
��    �� �� ��ʱ����ʱNs
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void TIMDelay_Ns(uint16_t Times)
{
  while(Times--)
  {
    TIMDelay_Nms(1000);
  }
}


/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
