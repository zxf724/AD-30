#ifndef __MFRC522_API_H
#define __MFRC522_API_H

#include "MFRC522_Dev.h"

typedef enum
{
    FPORDERDATA = 1,
    FPFINGEDATA = 2,
    FPNONEDATA  = 0
} FPDATATYPE;

typedef struct
{
    uint8_t ucBlockNum;    // RFID CARD �鿪ʼ��ַ��
    uint8_t ucBlockLen;    // RFID CARD �鳤��
    uint8_t ucBlockPass[6]; // RFID CARD ������
}RFIDDATA_TYPE;

// �洢��ɾ�������Чָ��ID��
#define MAXFINID     121

// ����FP�����������ֵ
#define MAXFINDATA   (2048)

// ����ָ��
// ��֤����ָ��
#define  LINKCARD      0xBE
#define  CHECKPSW      0xBF

/*
** ָ�ƴ洢����ʼ��ַ
*/
#define FINGERDATA1ADR     40
#define FINGERDATA2ADR     128

/*
**  ��Ӧ�洢ָ�ƿ���
*/
#define DATA1STEPLEN       4
#define DATA2STEPLEN       16



/*
** ������С����Ŀ
*/
#define MAXAREANUM         (8)
#define MINAREANUM         (32)

/*
** ������ʼ��
*/
#define MAXAREASTA         (128)

/*
** ������С����������
*/
#define MAXERRORPAR        (15)
#define MINERRORPAR        (3)

/*
** ������С����������
*/
#define MAXCTRLKUAI        (15)
#define MINCTRLKUAI        (3)

/*
** ������С���еĵظ����п���
*/
#define MAXAREALEN         (16)
#define MINAREALEN         (4)

// һ�����������
#define KUANDATALEN        (16)

/*
** ʱ�䡢����š���־λ�洢��
*/
#define TMRROOMADR         2


#define  MCUSENDSNR    0xA0
#define  MCUREADCARD   0xA1
#define  MCUWRITECARD  0xA2
#define  MCUMODIFYPSW  0xA3
#define  MCUSETTIME    0xA4
#define  MCULOGINFIG   0xA5
#define  MCUDELETEFIG  0xA6
#define  MCUMATCHFIG   0xA7
#define  MCUREADEXTCD  0xAA
#define  MCUWRITEXTCD  0xAB


// ���������Զ�
// ����������д
// ������δ����
#define  MCUTESTREAD   0xA8
#define  MCUTESTWRITE  0xA9
#define  MCUNONEORDER  0x00





extern uint8_t NewKey[16];     // ������  Ӧ����ÿ�������һ��Ȩ��
                                                             // ��ԿA(6Byte) + Ȩ��(4Byte) + ��ԿB(6Byte)
extern uint8_t PassWd[6];          // ���� ��ԿA ��ԿA���ܶ������ܸ�д
extern uint8_t MLastSelectedSnr[4];// �洢 ��Ƭ���к�

//extern const uint8_t PassWord_Default[6];           //Ĭ�ϵ����루��ԭʼ���룩
//extern const uint8_t PassWord_user[6];              //�޸ĵ����루�������û�ʹ�õ����룩





void    MFRC522_Init(void);
u8      LinkMFRC522 (u8 *SN);

uint8_t StartLinkRc522 (void);
uint8_t RC522SendSnr (void);
uint8_t RC522SnrPassward (void);
uint8_t RC522ModifyPsw (uint8_t KuaiN);
uint8_t RfidReadData (uint8_t *pucKuaiDat,
                      uint8_t  ucKuaiNum,
                      uint8_t  ucKuaiLen,
                      uint8_t  ucContiFlg
                     );
uint8_t RfidWriteData (uint8_t ucKuaiNum,
                       uint8_t ucKuaiLen,
                       uint8_t *pucDat
                      );

#endif
