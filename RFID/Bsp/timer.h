/*
  **********************************  STM8S  ***********************************
  * @�ļ���     �� timer.h
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� TIMERͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _TIMER_BSP_H
#define _TIMER_BSP_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8s.h"


/* �궨�� --------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/
void TIMER_Initializes(void);

void TIM2_Timing_Nms(uint16_t Times);
void TIM2_Timing_Off(void);

void TIMDelay_N10us(uint16_t Times);
void TIMDelay_Nms(uint16_t Times);
void TIMDelay_Ns(uint16_t Times);


#endif /* _TIMER_BSP_H */

/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
