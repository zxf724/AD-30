/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� global_variable.h
  * @����       �� Huang Fugui
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��09��02��
  * @ժҪ       �� ȫ�ֱ���ͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _GLOBAL_VERIABLE_H
#define _GLOBAL_VERIABLE_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "global_define.h"


/* ȫ�ֱ��� ------------------------------------------------------------------*/
/* ���� */
extern uint8_t  gRx_Buf[];                                 //���ڽ�������BUF
extern uint8_t  gRx_Ok;                                    //���ݰ��������(��Ч)

/* ����ˢ�� */
extern uint8_t  gEeprom_Buf[];                             //����BUF
extern uint16_t gCount;                                    //ˢ���ܴ���
extern uint16_t gRecord_Offset;                            //��¼ƫ��

extern uint8_t  gDeviceID;                                //ˢ����(�豸)���

/* TIM��ʱ�� */
extern uint8_t  gTIMTiming2_FlagOver;                      //��ʱ2ʱ�������־
extern uint16_t gTIMTiming2_Num;                           //��ʱ2����

/* �����ݿ� */
extern uint8_t  gCardBlock_Buf[];                          //��һ������BUF

/* ��ID�� */
extern uint8_t  gCardID[];                                 //���һ��ˢ��ID��

/* ��ʱ����ʱ */
extern uint16_t gTiming_Card;                              //ˢ����ʱ����ֹ������ͬ���ظ�������
extern uint16_t gTiming_Out;                               //�����ʱ���̵��������

/* �豸��ַ */
extern  uint8_t  gDeviceAddress;



#endif /* _GLOBAL_VERIABLE_H */

/***** Copyright (C)2016 HuangFugui. All Rights Reserved ***** END OF FILE *****/
