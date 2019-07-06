#include "MFRC522_SPI.h"


/*****************************************************
** @��������RC522SPI_Init
** @����  ��RC522 ģ��SPI�ӿڳ�ʼ������
** @����  ����
** @����ֵ����
*****************************************************/
void RC522SPI_Init (void)
{    
  GPIO_Init(SPI_MISO_GPIO_PORT, (GPIO_Pin_TypeDef)SPI_MISO_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
  /* MOSI ���� ������� */
  GPIO_Init(SPI_MOSI_GPIO_PORT, (GPIO_Pin_TypeDef)SPI_MOSI_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_SLOW);
  /* CLK ���� ������� */
  GPIO_Init(SPI_CLK_GPIO_PORT, (GPIO_Pin_TypeDef)SPI_CLK_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_SLOW);

  /* CS  ������� */
  GPIO_Init(SPI_CS_GPIO_PORT, (GPIO_Pin_TypeDef)SPI_CS_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_SLOW);

  /* RC523RST  ������� */
  GPIO_Init(RC523RST_GPIO_PORT, (GPIO_Pin_TypeDef)RC523RST_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_SLOW);
}

/*****************************************************
** @��������SPIReadByte
** @����  ��RC522 ģ��SPI�ӿ� SPI������
** @����  ����
** @����ֵ����
*****************************************************/
uint8_t SPIReadByte (void)
{
    uint8_t SPICount = 0;
    uint8_t SPIData  = 0;
    
    SPIData = 0;
    for (SPICount = 0; SPICount < 8; SPICount++)
    {
        SPIData <<= 1;
        CLR_SPI_SCK;
        __nop();
        __nop();
        if (STU_SPI_MISO)
        {
            SPIData |= 0x01;
        }
        SET_SPI_SCK;
        __nop();
        __nop();
    }
    
    return SPIData;
}

/*****************************************************
** @��������SPIWriteByte
** @����  ��RC522 ģ��SPI�ӿ� SPIд����
** @����  ����
** @����ֵ����
*****************************************************/
void SPIWriteByte (uint8_t SPIData)
{
  uint8_t SPICount;                                       // Counter used to clock out the data
  
  for (SPICount = 0; SPICount < 8; SPICount++)
  {
    if (SPIData & 0x80) {
      SET_SPI_MOSI;
    } else {
      CLR_SPI_MOSI;
    } 
    __nop();
    __nop();
    CLR_SPI_SCK;
    __nop();
    __nop();
    SET_SPI_SCK;
		__nop();
    __nop();
    SPIData <<= 1;
  }        	
}

/*****************************************************
** @��������delay_us
** @����  ����ʱ����, �Ǳ�׼us������ʱ
** @����  ��tus
** @����ֵ����
*****************************************************/
void delay_us (uint16_t tus)
{
  uint16_t ii;
    
  while(tus)
  {
    tus--;
    for (ii = 0; ii < 100; ii++) 
    {
      __nop();
      __nop();
      __nop();
      __nop();
      __nop();
      __nop();
      __nop();
      __nop();
    }
  }
}


