#include "moto.h"
#include <stdio.h>
#include "delay.h"
#include "wdg.h"
#include "usart.h"

Machine machine = {0,0,0};
Moto motoDef = {Open_xMoto,Close_xMoto,Read_xMoto,0,0};
mError errorDef = {0,0};

extern uint32_t falg_z_time;
extern uint32_t falg_x_time;

uint16_t g_x_step = 0;

mPin Pin_Array[PINMAX] = 
	{
		MOTO(1), MOTO(2), MOTO(3), MOTO(4), MOTO(5), MOTO(6), MOTO(7), MOTO(8), MOTO(9), MOTO(10),
		MOTO(11),MOTO(12),MOTO(13),MOTO(14),MOTO(15),MOTO(16),MOTO(17),DEM(1),  DEM(2) , DEM(3) ,
		DEM(4),  DEM(5),  DEM(6),  DEM(7),  DEM(8),  DEM(9) , DEM(10) ,DEM(11), DEM(12), DEM(13), 
		DEM(14), DEM(15), DEM(16), DEM(17), MOTO(R1),MOTO(L1),MOTO(R2),MOTO(L2),MOTO(STEP1_PUL),MOTO(STEP2_PUL),
	  MOTO(STEP1_DIR),MOTO(STEP2_DIR),
	  //42-45 notch sensor input 0 when signal comes up   
	  // 46
	  DEM(19), DEM(20), DEM(21), DEM(22),DEM(18)
	};
	
void Moto_Init()
{
	uint8_t i = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);
 
	for(i = 0;i < PINMAX;i++)
	{
		GPIO_InitStructure.GPIO_Pin  = Pin_Array[i].pin;
		GPIO_InitStructure.GPIO_Mode = Pin_Array[i].mode;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_Init(Pin_Array[i].port, &GPIO_InitStructure);
		GPIO_ResetBits(Pin_Array[i].port,Pin_Array[i].pin);
	}
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO

//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4; //
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//
//   GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIO

//infrared sensor
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
}

uint8_t Open_xMoto(uint8_t num)
{
	if(num < 1 || num > 16)
		return 1;
  GPIO_SetBits(Pin_Array[num-1].port,Pin_Array[num-1].pin); 
  return 0;		
}

uint8_t Close_xMoto(uint8_t num)
{
	if(num < 1 || num > 16)
		return 1;
  GPIO_ResetBits(Pin_Array[num-1].port,Pin_Array[num-1].pin);
  return 0;		
}

uint8_t Read_xMoto(uint8_t num)
{
	if(num < 1 || num > 16)
		return 1;
	return GPIO_ReadInputDataBit(Pin_Array[17+num-1].port,Pin_Array[17+num-1].pin);
}

/*num: �������?1����2��dir������step������*/
void Start_Step(uint8_t num,uint8_t dir,uint8_t step)
{
	if(num == 1)
	{
		if(dir)
			MOTO_STEP1_LEFT_RUN;
		else
			MOTO_STEP1_RIGHT_RUN;	
		if(step!= 0)
			xStep_moto(num,step);
	}else if(num == 2)
	{
		if(dir)
			MOTO_STEP2_LEFT_RUN;
		else
			MOTO_STEP2_RIGHT_RUN;
		if(step!= 0)
			xStep_moto(num,step);
	}
}

/*תstep����*/
void xStep_moto(uint8_t num,uint8_t step)
{
	uint8_t i = 0;
  for(i = 0;i < step;i++)
	{
		SingStep(num);
	}
}

/*תһ����*/
void SingStep(uint8_t num)
{
	uint16_t i = 0;
	if(num == 1)		// Z
	{	
		for(i = 0;i < 50;i++)//תһ����		
		{
			 IWDG_Feed();
			 MOTO_STEP1_PUL_ON;
			 delay_us(Z_PULS_STEP_RESET);
			 MOTO_STEP1_PUL_OFF;
			 delay_us(Z_PULS_STEP_RESET);
		}
	}else if(num == 2)
	{
		for(i = 0;i < 20;i++)//תһ����
		{
			 IWDG_Feed();		
			 MOTO_STEP2_PUL_ON;
			 delay_us(PULS_STEP);
			 MOTO_STEP2_PUL_OFF;
			 delay_us(PULS_STEP);
		}
	
	}
}

void X_One_Step(void) {
	static uint8_t i;
	for(i = 0;i < 50;i++) { //תһ����
		IWDG_Feed();		
		MOTO_STEP2_PUL_ON;
		delay_us(PULS_STEP);
		MOTO_STEP2_PUL_OFF;
		delay_us(PULS_STEP);
	}
}

void Z_One_Step(void) {
	uint8_t i;
	for(i = 0;i < 50;i++) { //תһ����
		IWDG_Feed();
		MOTO_STEP1_PUL_ON;
		delay_us(PULS_STEP);
		MOTO_STEP1_PUL_OFF;
		delay_us(PULS_STEP);
	}
}

void Z_Step(uint16_t z) {
	uint16_t i;
	for (i=0;i<=100;i++) {
		IWDG_Feed();
		if((z>=0)&&(z<=3000)) {
			MOTO_STEP1_PUL_ON;
		}
		delay_us(PULS_STEP);		// puls_step
		if((z>=0)&&(z<=3000)) {
			MOTO_STEP1_PUL_OFF;
		}
		delay_us(PULS_STEP);		// puls_step
	}
}

// new   dir: 0 is run. 1 is back.

//   void x_z_sametime(uint32_t z_step,uint32_t x_step,uint8_t dir) {
// 	uint8_t z_step_tmp;
// 	uint8_t z_step_in = z_step; 
// 	g_x_step = x_step;
// 	TIM_Cmd(TIM3, ENABLE);
// 	for (z_step_tmp=0;z_step_tmp<=z_step_in;z_step_tmp++) {
// 			z_step--;
// 			if(dir == 0) {
// 				MOTO_STEP1_LEFT_RUN;
// 			} else if(dir == 1) {
// 				MOTO_STEP1_RIGHT_RUN;
// 			}
// 			Z_Step(z_step);
// 	}
// }

typedef enum 
{
	motor_start,
	motor_start_fast,
	motor_fast,
	motor_slowdown,
	motor_slow,
	motor_stop
} MotorStatusEnum;

// new sametime
void x_z_sametime(uint32_t zin, uint32_t xin, uint8_t dir) {
	uint32_t iz = 0,dz = 0;
	uint8_t oz = 0;
	uint32_t z_step_in = 0;
	uint16_t z_start_puls = 100;
	uint16_t z_step = 0, x_step = 0;
	uint16_t z_plus_fast = 80, z_plus_slow = 150, z_plus_slow_down = 180;
	uint32_t slow_count = 50, fast_count = 9000;
	uint8_t z_slowdown_count = 12;

	MotorStatusEnum zstatus = motor_start;

	z_step_in = (zin*2);
	DBG_LOG("z_step_in = %d",z_step_in);
 
	if(dir == 0) {
		MOTO_STEP1_LEFT_RUN;
	} else if(dir == 1) {
		MOTO_STEP1_RIGHT_RUN;
	}
		// ix = 3000;//
	while (iz <= z_step_in) 
	{
	 	delay_us(1);
		dz++;
		// z plus
		if (dz >= z_start_puls) {
			dz = 0;
			oz = (oz) ? 0 : 1;
			if (oz == 0){
				z_step++;
				iz++;
				switch (zstatus)
				{
					case motor_start:
					z_start_puls = z_plus_slow;		// Hz
					if (z_step >= slow_count) {
						zstatus = motor_start_fast;
						z_step = 0;
					}
					break;
					case motor_start_fast:			// slow down
					if (z_step % z_slowdown_count == 0){		// speed 
						z_start_puls--;
					}
					if (z_start_puls <= z_plus_fast) {
						zstatus = motor_fast;
						z_step = 0;
					}
					break;
					case motor_fast:
					z_start_puls = z_plus_fast;
					if (z_step >= (zin - slow_count*2)) {
						zstatus = motor_slowdown;
						z_step = 0;
					}
					break;		
					case motor_slowdown:
						//z_start_puls = z_plus_fast;
					if (z_step % z_slowdown_count == 0){
						z_start_puls++;
					}
					if (z_start_puls >= z_plus_slow_down) {
						zstatus = motor_slow;
						z_step = 0;
					}
					break;		
					case motor_slow:
					z_start_puls = z_plus_slow;
					if (z_step >= slow_count) {
						zstatus = motor_stop;
						z_step = 0;
					}
					break;		
					case motor_stop:
					break;
					default:
					break;	
				}
			}
		}

		if (oz) {
			MOTO_STEP1_PUL_ON;
		} else {
			MOTO_STEP1_PUL_OFF;
		}
		IWDG_Feed();
	}
}

void x_sametime(uint32_t xin,uint8_t dir){ 
	uint32_t ix = 0,dx = 0;
	uint8_t ox = 0;
	uint32_t x_step_in = 0;
	uint16_t x_start_puls = 200;
	uint16_t x_step = 0;
	uint16_t x_plus_fast = 600, x_plus_slow = 150;
	uint32_t slow_count = 100;
	uint8_t  x_slowdown_count = 46;

	MotorStatusEnum xstatus = motor_start;

	x_step_in = (xin*2);
	DBG_LOG("x_step_in = %d",x_step_in);

	if(dir == 0) {
		MOTO_STEP2_LEFT_RUN;
	} else if(dir == 1) {
		MOTO_STEP2_RIGHT_RUN;
	}
	while (ix <= x_step_in)
	{
	 	delay_us(1);
	 	dx++;
		// x plus
		if (dx >= x_start_puls) {
			dx = 0;
			ox = (ox) ? 0 : 1;
			if (ox == 0){
				x_step++;
				ix++;
				switch (xstatus)
				{
					case motor_start:
					x_start_puls = x_plus_fast;		// Hz
					if (x_step >= slow_count) {	
						xstatus = motor_start_fast;
						x_step = 0;
					}
					break;
					case motor_start_fast:			// slow down
					if (x_step % x_slowdown_count == 0) {		// speed 
						x_start_puls--;
					}
					if (x_start_puls <= x_plus_slow) {
						xstatus = motor_fast;
						x_step = 0;
					}
					break;
					case motor_fast:
					x_start_puls = x_plus_fast;
					if (x_step >= xin - (slow_count * 2)) {
						xstatus = motor_slowdown;
						x_step = 0;
					}
					break;		
					case motor_slowdown:
					if (x_step % x_slowdown_count == 0){
						x_start_puls++;
					}
					if (x_start_puls >= x_plus_slow) {
						xstatus = motor_slow;
						x_step = 0;
					}
					break;
					case motor_slow:
					x_start_puls = x_plus_slow;
					if (x_step >= slow_count) {
						xstatus = motor_stop;
						x_step = 0;
					}
					break;		
					case motor_stop:
					break;
					default:
					break;	
				}
			}
		}

		if (ox) {
			MOTO_STEP2_PUL_ON;
		} else {
			MOTO_STEP2_PUL_OFF;
		}
		IWDG_Feed();
	}
}

void x_z_sametime_begin(uint32_t zin, uint32_t xin, uint8_t dir) {
	uint32_t iz = 0,dz = 0;
	uint8_t oz = 0;
	uint32_t z_step_in = 0;
	uint16_t z_start_puls = 100;
	uint16_t z_step = 0, x_step = 0;
	uint16_t z_plus_fast = 40, z_plus_slow = 150, z_plus_slow_down = 60;
	uint32_t slow_count = 50;
	uint8_t z_slowdown_count = 75;

	MotorStatusEnum zstatus = motor_start;

	z_step_in = (zin*2);
	DBG_LOG("z_step_in = %d",z_step_in);
 
	if(dir == 0) {
		MOTO_STEP1_LEFT_RUN;
	} else if(dir == 1) {
		MOTO_STEP1_RIGHT_RUN;
	}
		// ix = 3000;//
	while (iz <= z_step_in) 
	{
	 	delay_us(1);
		dz++;
		// z plus
		if (dz >= z_start_puls) {
			dz = 0;
			oz = (oz) ? 0 : 1;
			if (oz == 0){
				z_step++;
				iz++;
				switch (zstatus)
				{
					case motor_start:
					z_start_puls = z_plus_slow;		// Hz
					if (z_step >= slow_count) {
						zstatus = motor_start_fast;
						z_step = 0;
					}
					break;
					case motor_start_fast:			// slow down
					if (z_step % z_slowdown_count == 0){		// speed 
						z_start_puls--;
					}
					if (z_start_puls <= z_plus_fast) {
						zstatus = motor_fast;
						z_step = 0;
					}
					break;
					case motor_fast:
					z_start_puls = z_plus_fast;
					if (z_step >= slow_count) {
						zstatus = motor_slowdown;
						z_step = 0;
					}
					break;		
					case motor_slowdown:
						//z_start_puls = z_plus_fast;
					if (z_step % z_slowdown_count == 0){
						z_start_puls++;
					}
					if (z_start_puls >= z_plus_slow_down) {
						zstatus = motor_slow;
						z_step = 0;
					}
					break;		
					case motor_slow:
					z_start_puls = z_plus_slow;
					if ((z_step >= z_plus_slow_down) && (z_step <= zin)) {
						zstatus = motor_stop;
						z_step = 0;
					}
					break;		
					case motor_stop:
					break;
					default:
					break;	
				}
			}
		}

		if (oz) {
			MOTO_STEP1_PUL_ON;
		} else {
			MOTO_STEP1_PUL_OFF;
		}
		IWDG_Feed();
	}
}