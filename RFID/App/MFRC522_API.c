#include "MFRC522_API.h"

uint8_t PassWd[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};       //���ڷ��ʵ����루�û����룩
                    //{0x00,0x00,0x00,0x00,0x00,0x00};
                    //{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};	   // Ĭ������(��ԿA) - ���ܶ������ܸ�д

uint8_t MLastSelectedSnr[4];      // �洢 ��Ƭ���к�


uint8_t NewKey[16] = {0x01,0xAB,0x23,0xCD,0x45,0xEF,
                      0xff, 0x07, 0x80, 0x69, 
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00
					           };	// ������  Ӧ����ÿ�������һ��Ȩ��
				                // ��ԿA(6Byte) + Ȩ��(4Byte) + ��ԿB(6Byte)

//const uint8_t PassWord_Default[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};           //Ĭ�ϵ����루��ԭʼ���룩
//const uint8_t PassWord_user[6] = {0x01,0xAB,0x23,0xCD,0x45,0xEF};              //�޸ĵ����루�������û�ʹ�õ����룩

/*****************************************************
** @��������InitRc522
** @����  ����ʼ�� RC522
** @����  ����
** @����ֵ����
*****************************************************/
void MFRC522_Init(void)
{
	RC522SPI_Init();					//��ʼ���˿�
	PcdReset();								//��λRC522
	PcdAntennaOff(); 					//�ر�����
	PcdAntennaOn();						//��������
	M500PcdConfigISOType('A');//����ΪISO14443_Aģʽ
}

/*****************************************************
** @��������StartLinkRc522
** @����  ����ʼ�������� RC522
** @����  ����
** @����ֵ���ɹ�����1
*****************************************************/
u8 LinkMFRC522 (u8 *SN)
{
	u8  status;
  u8  ucRFIDCardSVN[6];
	
 	PcdReset(); // ��λRC522�����ö�ʱ��Ԥ��Ƶ3390����ʱ��������ֵ30 ��ʱʱ��Ϊ 15ms
	status = PcdRequest(PICC_REQIDL, &ucRFIDCardSVN[0]);
	if(status != MI_OK)
  {
		return 0;
	}
	status = PcdAnticoll(&ucRFIDCardSVN[2]);
	if(status != MI_OK)	
  {
		return 0;
	}
	memcpy(SN,&ucRFIDCardSVN[2],4);
	
	status = PcdSelect(SN); // ѡ��
	if(status != MI_OK) 
	{
		return 0;
	}
	return 1;
}




/*****************************************************
** @��������StartLinkRc522
** @����  ����ʼ�������� RC522
** @����  ����
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t StartLinkRc522 (void)
{
  uint8_t ucNum;
	int8_t  status;
  uint8_t ucRFIDCardSVN[6];
	
	PcdReset(); // ��λRC522�����ö�ʱ��Ԥ��Ƶ3390����ʱ��������ֵ30 ��ʱʱ��Ϊ 15ms
	
	// Ѱ��������δ��������״̬�Ŀ�
	// ���ؿ�Ƭ���� 2�ֽ� ��Ƭ����λ�� 0����0�� Byte6 -- Byte7
	// �洢��	ucRFIDCardSVN[0] ucRFIDCardSVN[1] ��
	status = PcdRequest(PICC_REQIDL, &ucRFIDCardSVN[0]);
	if(status != MI_OK) {
		return 0;
	}
	
	// ����ײ�����ؿ������к� 4�ֽ� ���к�Ϊ��0����0��� Byte0 -- Byte3
	// ���سɹ���Ƭ���кŴ洢�� RevBuffer[2] RevBuffer[3] RevBuffer[4] RevBuffer[5] ��
	status = PcdAnticoll(&ucRFIDCardSVN[2]);
	if(status != MI_OK)	{
		return 0;
	}
	// ����Ƭ���к�ת���� MLastSelectedSnr ȫ������
	for (ucNum = 0; ucNum < 4; ucNum++)
	{
    	MLastSelectedSnr[ucNum] = ucRFIDCardSVN[2+ucNum];
	}
	
	status = PcdSelect(MLastSelectedSnr); // ѡ��
	if(status != MI_OK) {
		return 0;
	}

	return 1;
}

/*****************************************************
** @��������RC522SendSnr
** @����  ����RFID���ж�ȡ���к�
** @����  ��None
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t RC522SendSnr (void)
{
	uint8_t ucRecDat = 0;
	
 	ucRecDat = StartLinkRc522();
	if (1 != ucRecDat) 
  {
//	    OperateFailure(LINKCARD);		
		return 0;
	}

//	OperateSuccess(MCUSENDSNR, 4);
// 	for(ucRecDat = 0; ucRecDat < 4; ucRecDat++)
//  {
//  	UART1_TxChar(MLastSelectedSnr[ucRecDat]);
//  }
	PcdHalt();					 // ϵͳ����ģʽ

	return 1;
}

/*****************************************************
** @��������RC522SnrPassward
** @����  ����RFID���ж�ȡ���кţ������к�����6λ����
** @����  ��NewKey:������
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t RC522SnrPassward (void)
{
  uint8_t ucNum    = 0;
  uint8_t DealData = 0;
  
 	if (StartLinkRc522())
 	{
   	for (ucNum = 0; ucNum < 4; ucNum++)
   	{
   	  NewKey[ucNum]    = MLastSelectedSnr[ucNum];
   	  NewKey[10+ucNum] = 0xFF - MLastSelectedSnr[ucNum];
   	}
   	
   	DealData = MLastSelectedSnr[0];
   	for (ucNum = 1; ucNum < 4; ucNum++)
   	{
     	DealData ^= MLastSelectedSnr[ucNum];
   	}
   	NewKey[4]  = DealData;
   	NewKey[14] = 0xFF - DealData;
   	
   	DealData = MLastSelectedSnr[0];
   	for (ucNum = 1; ucNum < 4; ucNum++)
   	{
     	DealData += MLastSelectedSnr[ucNum];
   	}
   	NewKey[5]  = 0xFF - DealData;
   	NewKey[15] = 0xFF - DealData;
   	
   	PcdHalt(); // ϵͳ����ģʽ
   	return 1;
 	}
 	PcdHalt();   // ϵͳ����ģʽ
 	return 0;
}


/*****************************************************
** @��������RC522ModifyPsw
** @����  ���޸�RFID��ָ�����������
** @����  ��KuaiN  : �޸Ŀ�
**          NewKey : �޸ĺ��������
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t RC522ModifyPsw (uint8_t KuaiN)
{
 	uint8_t ucRecDat = 0;

 	ucRecDat = StartLinkRc522();
	if (1 != ucRecDat) 
  {
//		OperateFailure(LINKCARD);
		return 0;
	}

	ucRecDat = PcdAuthState(PICC_AUTHENT1A,KuaiN,PassWd,MLastSelectedSnr);
  if (ucRecDat != MI_OK) 
  {
//        OperateFailure(CHECKPSW);
	  return 0;
  }

  ucRecDat = PcdWrite(KuaiN,&NewKey[0]); // д���޸�֮����������
  if (ucRecDat != MI_OK) 
  {
//    OperateFailure(MCUMODIFYPSW);
	  return 0;
  }
  PcdHalt();                             // ����
//	OperateSuccess(MCUMODIFYPSW, 0); 

	return 1;
}

/*****************************************************
** @��������RfidReadData
** @����  ����RFID���ж�ȡ����
** @����  ��pucKuaiDat   ��ȡ������
**          ucKuaiNum    RFID ��ȡ�鿪ʼ��ַ
**          ucKuaiLen    RFID ��ȡ��ĳ���
**          ucContiFlg   ��ȡ�������Ƿ����� 
**                        0 ֻ��ȡ���ݿ�
**                        1 ���ݿ�����ƿ�һ���
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t RfidReadData (uint8_t *pucKuaiDat,
                      uint8_t  ucKuaiNum, 
                      uint8_t  ucKuaiLen, 
                      uint8_t  ucContiFlg
                     )
{
  uint8_t ucStatus;
  uint8_t ucStartKuai;
  uint8_t ucKuaiLength;
  uint8_t ucNum;
  uint8_t ucDatNum;
  uint8_t ucPswFlg;     // ��֤�����־
  uint8_t ucReadDat[16];
  
  ucPswFlg     = 0;
  ucStartKuai  = ucKuaiNum;
  ucKuaiLength = ucKuaiLen;
  
  ucStatus = StartLinkRc522(); // ����RDIF
	if (1 != ucStatus) 
  {
		PcdHalt();
		return 0;
	}
	
	for (ucNum = 0; ucNum < ucKuaiLength; ucNum++, ucStartKuai++) {
    	
    	// �ж������ݿ黹�ǿ��ƿ�
    	if (ucStartKuai < MAXAREASTA) 
      { // С������
        if (MINCTRLKUAI == ucStartKuai%MINAREALEN) 
        {
          ucPswFlg = 1;
        }
      } 
      else
      {                  // ��������
        if (MAXCTRLKUAI == ucStartKuai%MAXAREALEN) 
        {
          ucPswFlg = 1;
        }
      }
	    
	    // �ж��Ƿ�������ȡ      
      if ((!ucContiFlg) && (ucPswFlg)) 
      { // ֻ��ȡ���ݿ�
        ucStartKuai++;     	    
	    }
	    
	    //KuaiN = ucStartKuai;
	    if ((!ucNum) || (ucPswFlg)) 
  	  {
    	    ucPswFlg = 0;
	        ucStatus = PcdAuthState(PICC_AUTHENT1A,ucStartKuai,PassWd,MLastSelectedSnr);// ��֤��Ƭ����
            if (ucStatus != MI_OK) {
        		PcdHalt();
        		return 0;
            }
	    }
	    
	    ucStatus = PcdRead(ucStartKuai, ucReadDat);
        if (ucStatus != MI_OK) {
    		PcdHalt();
    		return 0;
        } else {
    	    for (ucDatNum = 0; ucDatNum < 16; ucDatNum++) {    	        
    	        pucKuaiDat[ucNum*16+ucDatNum] = ucReadDat[ucDatNum];
    	    }	        
	    } 
	}
	PcdHalt();	        
	return 1;
}

/*****************************************************
** @��������RfidWriteData
** @����  ����RFID������������д������
** @����  ��ucKuaiNum    RFID д���ݿ鿪ʼ��ַ
**          ucKuaiLen    RFID д���ݿ�ĳ���
**         *pucDat       д�������
** @����ֵ���ɹ�����1
*****************************************************/
uint8_t RfidWriteData (uint8_t ucKuaiNum, 
                       uint8_t ucKuaiLen, 
                       uint8_t *pucDat
                      )
{
    uint8_t ucStatus;
    uint8_t ucStartKuai;
    uint8_t ucKuaiLength;
    uint8_t ucNum;
    uint8_t ucDatNum;
    uint8_t ucPswFlg;     // ��֤�����־
    uint8_t ucWriteDat[16];
    
    ucPswFlg     = 0;
    ucStartKuai  = ucKuaiNum;
    ucKuaiLength = ucKuaiLen;
    
    ucStatus = StartLinkRc522(); // ����RDIF
	if (1 != ucStatus) {
		PcdHalt();
		return 0;
	}
	
	for (ucNum = 0; ucNum < ucKuaiLength; ucNum++, ucStartKuai++) {
	    // �жϿ��ƿ�
    	if (ucStartKuai < MAXAREASTA) { // С������
            if (MINCTRLKUAI == ucStartKuai%MINAREALEN) {
                ucPswFlg = 1;                
            }
        } else {                  // ��������
            if (MAXCTRLKUAI == ucStartKuai%MAXAREALEN) {
	            ucPswFlg = 1;
	        }
        }
        
        // �ж��Ƿ�������ȡ      
        if (ucPswFlg) { // ֻ��ȡ���ݿ�
	       ucStartKuai++;     	    
	    }
	    
	    //KuaiN = ucStartKuai;
	    if ((!ucNum) || (ucPswFlg)) {
    	    ucPswFlg = 0;
	        ucStatus = PcdAuthState(PICC_AUTHENT1A,ucStartKuai,PassWd,MLastSelectedSnr);// ��֤��Ƭ����
            if (ucStatus != MI_OK) {
        		PcdHalt();
        		return 0;
            }
	    }
	    
	    for (ucDatNum = 0; ucDatNum < 16; ucDatNum++) {
		 	 ucWriteDat[ucDatNum] = *pucDat++; // д��16λ����
		}

		ucStatus = PcdWrite(ucStartKuai,&ucWriteDat[0]);
		if (ucStatus != MI_OK) {
    		PcdHalt();
		    return 0;
        }     
	}
	PcdHalt();	        
	return 1;
}




