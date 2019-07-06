#include "sys.h"
#include "usart.h"	 
#include <stdarg.h>
#include "fifo.h"
#include "usart.h"	 

extern app_fifo_t  rx_fifo_DBG_Def; 
extern app_fifo_t  rx_fifo_Rfid_Def;

static uint8_t gs_Rfid_rx_buff[512] = {'\0'};
static uint8_t gs_DBG_rx_buff[512] = {'\0'};

void uart2_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
  USART_Init(USART2, &USART_InitStructure); 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2, ENABLE);  

	app_fifo_init(&rx_fifo_DBG_Def,gs_DBG_rx_buff,sizeof(gs_DBG_rx_buff));
  app_fifo_init(&rx_fifo_Rfid_Def,gs_Rfid_rx_buff,sizeof(gs_Rfid_rx_buff));
}

void uart3_init(u32 baud)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);  
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
	
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 
  USART_Init(USART3, &USART_InitStructure);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART3, ENABLE); 
      
} 


void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
	  app_uart_put(Res,RFID); 
  } 
} 
	
void USART3_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		app_uart_put(Res,DBG);		 
  } 
} 

void RFID_printf (char *fmt, ...)
{
		char buffer[200] = {'\0'}; 
		u8 i = 0;
		
		va_list arg_ptr;
		va_start(arg_ptr, fmt);  
		vsnprintf(buffer, 200, fmt, arg_ptr);
		while ((i < 200) && buffer)
		{
				USART_SendData(USART2, (u8) buffer[i++]);
				while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
		}
		va_end(arg_ptr);
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART3, (uint8_t) ch);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}	
  return ch;
}


void Uart_Send_Data(uint8_t usart,char* data,int len)
{
	  int i = 0;
	  USART_TypeDef* USARTx = NULL;
	  if(usart == DBG)
		{
			USARTx = 	USART3;
		}else if(usart == RFID)
		{
			USARTx = 	USART2;
		}
		while ((i < len))
		{
				USART_SendData(USARTx, (u8) data[i++]);
				while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); 
		}
}
