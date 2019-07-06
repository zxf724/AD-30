
#include "MFRC522_Dev.h"



//#include "Fun.h"

/*****************************************************
** @��������PcdRequest
** @����  ��Ѱ��
** @����  ��req_code[IN]:Ѱ����ʽ
**                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
**                0x26 = Ѱδ��������״̬�Ŀ�
**          pTagType[OUT]����Ƭ���ʹ���
**                0x4400 = Mifare_UltraLight
**                0x0400 = Mifare_One(S50)
**                0x0200 = Mifare_One(S70)
**                0x0800 = Mifare_Pro(X)
**                0x4403 = Mifare_DESFire
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdRequest(uint8_t req_code, uint8_t *pTagType)
{
	int8_t   status;
	uint16_t unLen;
	uint8_t  ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07); // �������LSB����bit0,���͵����һ���ֽ�λ��  7 bit
	SetBitMask(TxControlReg,0x03);  // ����TX1��TX2���;�13.56MHz�����ز�
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))	 // ���տ��������������ֽ���16Byte
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   
       status = MI_ERR;   
   }
   
	return status;
}

/*****************************************************
** @��������PcdAnticoll
** @����  ������ײ
** @����  ��pSnr[OUT]:��Ƭ���кţ�4�ֽ�
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdAnticoll (uint8_t *pSnr)
{
    int8_t   status;
    uint8_t  i,snr_check=0;
    uint16_t unLen;
    uint8_t  ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);	 
    WriteRawRC(BitFramingReg,0x00);    // ��ͣ���ݷ���
	                                   // ����LSB����bit0λ��
									   // ���һ���ֽ�����λ������
    ClearBitMask(CollReg,0x80);		   // ���Խ��յ�λ��λ��ͻ�󶼽�����
 
    ucComMF522Buf[0] = PICC_ANTICOLL1; // ����ײָ�����
    ucComMF522Buf[1] = 0x20;		   // ?

	// ���Ͳ���������ָ��
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];	 // 0��0�� 0--3 Byte ��Ƭ���к�
             snr_check ^= ucComMF522Buf[i];	 // ����Ƭ���кŰ���������У��
         }
         if (snr_check != ucComMF522Buf[i])	 // ��Ƭ���к�У����Byte4��ͬ������ȷ
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/*****************************************************
** @��������PcdSelect
** @����  ��ѡ����Ƭ
** @����  ��pSnr[IN]:��Ƭ���кţ�4�ֽ�
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdSelect(uint8_t *pSnr)
{
    int8_t   status;
    uint8_t  i;
    uint16_t unLen;
    uint8_t  ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;  // ����ײ  0x93
    ucComMF522Buf[1] = 0x70;			// ?
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);	// ѡ��Ƭ�����к�
    	ucComMF522Buf[6]  ^= *(pSnr+i);	// ѡ��Ƭ�����к�У����
    }
	// CRCУ�� 	ucComMF522Buf[7] ucComMF522Buf[8] �ֱ�洢У����LSB MSB��ֵ
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

	// ���Ͳ���������
	// ���ͷ���ײָ�� ��Ƭ���к�  ���к�У���� ���к�CRC������ֵ ��ֵ 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18)) // ���������Ƿ� 24 bit == 3 Byte
    {   
        status = MI_OK;  
    }
    else
    {   
        status = MI_ERR;    
    }

    return status;
}

/*****************************************************
** @��������PcdAuthState
** @����  ����֤��Ƭ����
** @����  ��auth_mode[IN]: ������֤ģʽ
**                 0x60 = ��֤A��Կ
**                 0x61 = ��֤B��Կ 
**          addr[IN]�����ַ
**          pKey[IN]������
**          pSnr[IN]����Ƭ���кţ�4�ֽ�
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdAuthState (uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
    int8_t   status;
    uint16_t unLen;
    uint8_t  i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    
		ucComMF522Buf[i+2] = *(pKey+i);
    }
    for (i=0; i<6; i++)	 // i < 4 ��Ϊ���к�ֻ��4���ֽ�
    {    
			ucComMF522Buf[i+8] = *(pSnr+i);
    }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 

	// ��֤��Կ
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   
			status = MI_ERR;   
		}
    return status;
}

/*****************************************************
** @��������PcdRead
** @����  ����ȡM1��һ������
** @����  ��addr[IN]�����ַ
**          pData[OUT]�����������ݣ�16�ֽ�
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdRead(uint8_t addr,uint8_t *pData)
{
    int8_t   status;
    uint16_t unLen;
    uint8_t  i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ; //   ����   0x30
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90)) // ��ȡ������ 144 bits == 18 Byte
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/*****************************************************
** @��������PcdWrite
** @����  ��д���ݵ�M1��һ��
** @����  ��addr[IN]�����ַ
**          pData[IN]��д������ݣ�16�ֽ�
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdWrite(uint8_t addr,uint8_t *pData)
{
    int8_t   status;
    uint16_t unLen;
    uint8_t  i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;  // 0xA0 д��
    ucComMF522Buf[1] = addr;		// ���ַ
	// CRCУ��ʵ����ֵ LSB MSB�ֱ�洢�� ucComMF522Buf[2] ucComMF522Buf[3]
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	
    
	// ���Ͳ���������
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(pData+i);   
        }
        
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/*****************************************************
** @��������PcdHalt
** @����  �����Ƭ��������״̬
** @����  ����
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdHalt(void)
{
    int8_t   status;
    uint16_t unLen;
    uint8_t  ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;  // 0x50 ����
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}

/*****************************************************
** @��������CalulateCRC
** @����  ����MF522����CRC16����
** @����  ����
** @����ֵ����
*****************************************************/
void CalulateCRC(uint8_t *pIndata,uint8_t len,uint8_t *pOutData)
{
    uint8_t  i,n; 
    
    ClearBitMask(DivIrqReg,0x04);   // ���� ��CRC����λ��Ч���������ݱ�����ʱ��λ
    WriteRawRC(CommandReg,PCD_IDLE);// ���Ϳ�������
    SetBitMask(FIFOLevelReg,0x80);	// FIFO��дָ��ָ����0�������������־λ��0
    
	for (i=0; i<len; i++)
    {   
		WriteRawRC(FIFODataReg, *(pIndata+i)); // ���������ݷ���FIFO��   
	}
    WriteRawRC(CommandReg, PCD_CALCCRC);       // ����CRC����

    i = 0xFF;  // 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04)); // �˳������� 
	                             // 1���Ա��� i �Լ���0
								 // 2��CRC��������λ��Ч���������ݱ�����ʱ��λ
    // ��ȡCRCУ���� CRC�����ʵ�� MSB LSB ��ֵ
    delay_us(10); // 2000  100 �ʺ϶�д
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/*****************************************************
** @��������PcdReset
** @����  ����λRC522
**          ��λRC522�����ö�ʱ��Ԥ��Ƶ3390��
**          ��ʱ��������ֵ30 ��ʱʱ��Ϊ 15ms
** @����  ����
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdReset(void)
{

	SET_SPI_RST;
  delay_us(10);

	CLR_SPI_RST;
  delay_us(10);

	SET_SPI_RST;
	delay_us(10);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	delay_us(10);
	
	WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
	WriteRawRC(TReloadRegL,30);           
	WriteRawRC(TReloadRegH,0);		     // Timer Reload Value 30
	WriteRawRC(TModeReg,0x8D);
	WriteRawRC(TPrescalerReg,0x3E);	     // Timer F Div is 3390
	
	WriteRawRC(TxAutoReg,0x40);//����Ҫ
    
  return MI_OK;
}

/*****************************************************
** @��������M500PcdConfigISOType
** @����  ������RC632�Ĺ�����ʽ 
** @����  ������������
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t M500PcdConfigISOType(uint8_t type)
{
	 if (type == 'A')                     //ISO14443_A
	 { 
		 ClearBitMask(Status2Reg,0x08);
		 WriteRawRC(ModeReg,0x3D);//3F
		 WriteRawRC(RxSelReg,0x86);//84
		 WriteRawRC(RFCfgReg,0x7F);   //4F
		 WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		 WriteRawRC(TReloadRegH,0);
		 WriteRawRC(TModeReg,0x8D);
		 WriteRawRC(TPrescalerReg,0x3E);
		 delay_us(1000);
		 PcdAntennaOn();
	 } 
	 else 
	 { 
		 return -1; 
	 }
   
   return MI_OK;
}

/*****************************************************
** @��������ReadRawRC
** @����  ����RC632�Ĵ���
** @����  ��Address[IN]:�Ĵ�����ַ
** @����ֵ��������ֵ
*****************************************************/
uint8_t ReadRawRC(uint8_t Address)
{
  uint8_t ucAddr;
  uint8_t ucResult=0;
    
	CLR_SPI_SDA;
  ucAddr = ((Address<<1)&0x7E)|0x80;
	
	SPIWriteByte(ucAddr);
	ucResult=SPIReadByte();
	SET_SPI_SDA;
  return ucResult;
}

/*****************************************************
** @��������WriteRawRC
** @����  ��дRC632�Ĵ���
** @����  ��Address[IN]:�Ĵ�����ַ
**          value[IN]:д���ֵ
** @����ֵ����
*****************************************************/
void WriteRawRC(uint8_t Address, uint8_t value)
{  
  uint8_t ucAddr;
	
	CLR_SPI_SDA;
  ucAddr = ((Address<<1)&0x7E);

	SPIWriteByte(ucAddr);
	SPIWriteByte(value);
	SET_SPI_SDA;
}

/*****************************************************
** @��������SetBitMask
** @����  ����RC522�Ĵ���λ
** @����  ��reg[IN]:�Ĵ�����ַ
**          mask[IN]:��λֵ
** @����ֵ����
*****************************************************/
void SetBitMask(uint8_t reg,uint8_t mask)  
{
	int8_t tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg,tmp | mask);  // set bit mask
}

/*****************************************************
** @��������ClearBitMask
** @����  ����RC522�Ĵ���λ
** @����  ��reg[IN]:�Ĵ�����ַ
**          mask[IN]:��λֵ
** @����ֵ����
*****************************************************/
void ClearBitMask(uint8_t reg,uint8_t mask)  
{
	int8_t tmp = 0x00;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/*****************************************************
** @��������PcdComMF522
** @����  ��ͨ��RC522��ISO14443��ͨѶ
** @����  ��Command[IN]:RC522������
**          pInData[IN]:ͨ��RC522���͵���Ƭ������
**          InLenByte[IN]:�������ݵ��ֽڳ���
**          pOutData[OUT]:���յ��Ŀ�Ƭ��������
**         *pOutLenBit[OUT]:�������ݵ�λ����
** @����ֵ���ɹ�����MI_OK
*****************************************************/
int8_t PcdComMF522(uint8_t   Command, 
                   uint8_t  *pInData, 
                   uint8_t   InLenByte,
                   uint8_t  *pOutData, 
                   uint16_t *pOutLenBit
                  )
{
	int8_t   status = MI_ERR;
	uint8_t  irqEn   = 0x00;
	uint8_t  waitFor = 0x00;
	uint8_t  lastBits;
	uint8_t  n;
	uint16_t i;
    
	switch (Command)
	{
			case PCD_AUTHENT:      // ��֤��Կ ��֤��Կ��ֻ���������ݣ�δ������Կ�ɹ����
		irqEn   = 0x12;
		waitFor = 0x10;
		break;
	case PCD_TRANSCEIVE:   // ���Ͳ���������
		irqEn   = 0x77;
		waitFor = 0x30;
		break;
	default:
		break;
	}
   
	WriteRawRC(ComIEnReg,irqEn|0x80);	// �����ж������������
	ClearBitMask(ComIrqReg,0x80);
	WriteRawRC(CommandReg,PCD_IDLE);    // ȡ����ǰָ��	���Ϳ���ָ��
	SetBitMask(FIFOLevelReg,0x80);		// ��0�ڲ�FIFO��д�Ĵ���ָ��,�������������־λ
	
	for (i=0; i<InLenByte; i++)
	{   
		WriteRawRC(FIFODataReg, pInData[i]);   
	} // ����������ת����FIFO�У�data <= 64
	WriteRawRC(CommandReg, Command);	          // �������Ͳ��������ָ��

	if (Command == PCD_TRANSCEIVE)                // ����������ͺ��ܽ�������
	{    										  // ���Ϊ��֤��Կ���ý�������
		SetBitMask(BitFramingReg,0x80);           // ���Ϊ����ָ���������������
	}		   // ������ֻ�뷢��ָ��һ��ʹ�ò���Ч������ָ������һ�������
    
    //i = 600; //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	i = 2000; //2000;
	// SetBitMask(ControlReg,0x40);           // start timer now --- Tiger.Yang
	do 
	{
			n = ReadRawRC(ComIrqReg);
			i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitFor));  // �ȴ��������ݽ��� ��������
																								// 1��i �Լ���0
																								// 2����ʱ���Լ���0 ����ʱ��δ��
																								// 3�����յ���Ч����������һ����������

	ClearBitMask(BitFramingReg,0x80);             // ֹͣ����ָ��

	if (i!=0)                                     // �Ǽ������� i �Լ���0�˳�
	{    
		if(!(ReadRawRC(ErrorReg)&0x1B))	          // ����δ���ִ���
		{
			status = MI_OK;
			if (n & irqEn & 0x01)	              // ��ʱ�����Լ��� 0
			{   
				status = MI_NOTAGERR;   
			}
			if (Command == PCD_TRANSCEIVE)        // ���Ͳ���������
			{
				// д FIFODataReg ʱ���ֶ�ֵ���� ���ͽ׶μ�����ж��������ѷ���
				// �� FIFODataReg ʱ���ֶ�ֵ���� ���ս׶μ�⻹�ж�������δ����
				// ��ȡ�����ֽ��� Max = 64	FIFO = 64
				n = ReadRawRC(FIFOLevelReg);      
				// ��ʾ�����յ�������Чλ���������0������ֽڶ���Ч
				lastBits = ReadRawRC(ControlReg) & 0x07;

				// ����һ������bit����
				if (lastBits) 
				{   
					*pOutLenBit = (n-1)*8 + lastBits;   
				} 
				else 
				{   
					*pOutLenBit = n*8;
				}

				if (n == 0)	    // ������յ�0�����ݣ��� n ��1
				{
					n = 1;    
				}
				if(n > MAXRLEN)// ����������ݳ������ý������ֵ����ֻȡǰ�������ֵ������Ч 
				{   
					n = MAXRLEN;   
				}
				for (i=0; i<n; i++)	// �ӽ���FIFO�ж�ȡ���ݳ���
				{   
					pOutData[i] = ReadRawRC(FIFODataReg);   
				}
			}
		}
		else
		{   
			status = MI_ERR;  
		}      
	} 
	SetBitMask(ControlReg,0x80);           // stop timer now
	WriteRawRC(CommandReg,PCD_IDLE);       // ����һ����������
	return status;
}

/*****************************************************
** @��������PcdAntennaOn
** @����  ����������
**          ÿ��������ر����߷���֮��Ӧ������1ms�ļ��
** @����  ����
** @����ֵ����
*****************************************************/
void PcdAntennaOn(void)
{
	uint8_t i;
	
	i = ReadRawRC(TxControlReg);
	if (!(i & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}

/*****************************************************
** @��������PcdAntennaOff
** @����  ���ر�����
**          �������λΪ0 �ô�������� TxControlReg 
**          �Ĵ�����0x03λ ������λ	
** @����  ����
** @����ֵ����
*****************************************************/
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

