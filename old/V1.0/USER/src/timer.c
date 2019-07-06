#include "timer.h"
#include "led.h"
#include "usart.h"	
#include "rtc.h" 	
#include "prjlib.h"
#include "moto.h"
#include "dns.h"
#include "cJSON.h"
#include "analysis_protocol.h"
#include "moto.h"

extern _calendar_obj calendar;
extern void timer0_backcall_func(void);
extern Moto motoDef;
extern uint8_t heart_time;
extern mPin Pin_Array[];


timer_t  timerlist[TIMER_LIST_MAX];
uint32_t timer3_tick =0;
volatile uint32_t msTicks;

// z moto time 
uint32_t falg_z_time = 0;
uint32_t falg_x_time = 0;
uint8_t flag_test1 = 0;

void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ���?���Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ��������?
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ���?���Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ��������?
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	// TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx	
	TIM_Cmd(TIM3,DISABLE);
}

//��ʱ��3�жϷ������?
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	static uint32_t time_plus = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������?
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־
		if(flag_test1 == 1) {
			time_plus++;
			if((GPIO_ReadInputDataBit(Pin_Array[(17-motoDef.num)+16].port,Pin_Array[(17-motoDef.num)+16].pin)) == 0) {
				motoDef.close_moto((17-motoDef.num));
				flag_test1 = 0;
				time_plus = 0;
				TIM_Cmd(TIM3,DISABLE);
			}
			if(time_plus >= 15000) {
				flag_test1 = 0;
				time_plus = 0;
				motoDef.close_moto((17-motoDef.num));
				TIM_Cmd(TIM3,DISABLE);
			}
		}
	}
}

void TIM2_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������?
	{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  
      heart_time = 1;
		//   printf("systen run \r\n");
	}
}

// TIM4
void TIM4_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ���?���Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ��������?
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  // TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  // ��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx
}

//��ʱ��4�жϷ������?
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������?
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		// Tim2_flag = 1;
	}
} 

void timer_task()
{
	uint8_t i;
	for(i=0;i<TIMER_LIST_MAX ;i++)
	{
			if(timerlist[i].state==0)
				continue;
			else{
							timerlist[i].count++;
			}
			if(timerlist[i].count==timerlist[i].expire){
							timerlist[i].count = 0;
							timerlist[i].callback();
			}
	}
        
}

void timer_enable(uint8_t num,uint16_t  val,void (* timer_backcall_func) (void))
{
		timerlist[num].state  = 1;
		timerlist[num].expire =  val+timer3_tick;
		timerlist[num].callback = timer_backcall_func;
}


void timer_disable(uint8_t num)
{
    timerlist[num].state = 0;
	  timerlist[num].expire = 0;
}

void Timer_Start(void)
{
	TIM3->CNT=0;//����������Ĵ�����ֵ�����Լ�С��֡�����
  /* Enable the TIM Counter */
	TIM_Cmd(TIM3, ENABLE); 
}

void Timer_Stop(void)
{ 
  /* Disable the TIM Counter */
	TIM_Cmd(TIM3, DISABLE);
}

void Timer2_Start(void)
{
	TIM2->CNT=0;//����������Ĵ�����ֵ�����Լ�С��֡�����
  /* Enable the TIM Counter */
	TIM_Cmd(TIM2, ENABLE); 
}

void Timer2_Stop(void)
{
  /* Disable the TIM Counter */
	TIM_Cmd(TIM2, DISABLE);
}

//pwm init
void TIM1_PWM_Init(uint16_t arr,uint16_t psc) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //
 
  //?��??????????????????????,????TIM1 CH1??PWM?????��??
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM1_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //????????????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //??????GPIO
 
	TIM_TimeBaseStructure.TIM_Period = arr; //?��??��?????����????????
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //?��???�衤????? ??��???
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //?��???��??��???:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM?��??????????
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //?��?????��????????????TIMx
	
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //CH1 PWM2????	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //��???????????
	TIM_OCInitStructure.TIM_Pulse = 0; //?��????����??????��????????��????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //OC1 ?????????�� 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //?��?????��???????????????��TIMx

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1 ?�����???????
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //????TIMx??ARR?????�����????????��
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE ?��????????,?????��?��?�¡�??????????? 
	
	TIM_Cmd(TIM1, ENABLE);  //????TIMx
}


