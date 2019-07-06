#ifndef _MOTO_H
#define _MOTO_H

#include "sys.h"

#define SUCCESS 1
#define FAIL    0

#define LEFT    0
#define RIGHT   1

#define PULS_STEP 100  //���������ʱ,��λus
#define PULS_STEP_SECOND	800

//  --- start middle end
#define X_PULS_STEP 400

#define Z_PULS_STEP 40
#define Z_PULS_STEP_DOWN	35
// z_start_puls - Z_START_STEP = Z_PULS_STEP

#define Z_START_STEP	200
#define X_START_STEP	200

#define Z_PULS_STEP_RESET 1000

// #define X_PULS_STEP_END 500
// #define Z_PULS_STEP_END 500
//---  start middle end

#define UP_DOWN_STEP_MOTO     1
#define LEFT_RIGHT_STEP_MOTO  2

#define PINMAX       47

#define MOTO(x) MOTO_##x
#define DEM(x)  DEM_##x

#define MOTO_1  {GPIOA,GPIO_Pin_0,GPIO_Mode_Out_PP}
#define MOTO_2  {GPIOA,GPIO_Pin_1,GPIO_Mode_Out_PP}
#define MOTO_3  {GPIOA,GPIO_Pin_2,GPIO_Mode_Out_PP}
#define MOTO_4  {GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP}
#define MOTO_5  {GPIOA,GPIO_Pin_4,GPIO_Mode_Out_PP}
#define MOTO_6  {GPIOA,GPIO_Pin_5,GPIO_Mode_Out_PP}
#define MOTO_7  {GPIOA,GPIO_Pin_6,GPIO_Mode_Out_PP}

#define MOTO_8  {GPIOC,GPIO_Pin_1,GPIO_Mode_Out_PP}
#define MOTO_9  {GPIOC,GPIO_Pin_2,GPIO_Mode_Out_PP}
#define MOTO_10 {GPIOC,GPIO_Pin_3,GPIO_Mode_Out_PP}
#define MOTO_11 {GPIOC,GPIO_Pin_4,GPIO_Mode_Out_PP}
#define MOTO_12 {GPIOC,GPIO_Pin_5,GPIO_Mode_Out_PP}

#define MOTO_13 {GPIOE,GPIO_Pin_7,GPIO_Mode_Out_PP}
#define MOTO_14 {GPIOE,GPIO_Pin_8,GPIO_Mode_Out_PP}
#define MOTO_15 {GPIOE,GPIO_Pin_9,GPIO_Mode_Out_PP}
#define MOTO_16 {GPIOE,GPIO_Pin_10,GPIO_Mode_Out_PP}
#define MOTO_17 {GPIOE,GPIO_Pin_11,GPIO_Mode_Out_PP}

#define MOTO_R1 {GPIOC,GPIO_Pin_10,GPIO_Mode_Out_PP}
#define MOTO_R2 {GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP}
#define MOTO_L1 {GPIOC,GPIO_Pin_11,GPIO_Mode_Out_PP}
#define MOTO_L2 {GPIOD,GPIO_Pin_1,GPIO_Mode_Out_PP}

#define MOTO_STEP1_PUL {GPIOD,GPIO_Pin_12,GPIO_Mode_Out_PP}
#define MOTO_STEP2_PUL {GPIOE,GPIO_Pin_1,GPIO_Mode_Out_PP}

#define MOTO_STEP1_DIR {GPIOD,GPIO_Pin_11,GPIO_Mode_Out_PP}
#define MOTO_STEP2_DIR {GPIOD,GPIO_Pin_14,GPIO_Mode_Out_PP}

#define DEM_1  {GPIOA,GPIO_Pin_7,GPIO_Mode_IN_FLOATING}
#define DEM_2  {GPIOA,GPIO_Pin_8,GPIO_Mode_IN_FLOATING}
#define DEM_3  {GPIOA,GPIO_Pin_9,GPIO_Mode_IN_FLOATING}
#define DEM_4  {GPIOA,GPIO_Pin_10,GPIO_Mode_IN_FLOATING}
#define DEM_5  {GPIOA,GPIO_Pin_11,GPIO_Mode_IN_FLOATING}
#define DEM_6  {GPIOA,GPIO_Pin_12,GPIO_Mode_IN_FLOATING}
#define DEM_7  {GPIOA,GPIO_Pin_15,GPIO_Mode_IN_FLOATING}

#define DEM_8  {GPIOB,GPIO_Pin_6,GPIO_Mode_IN_FLOATING}
#define DEM_9  {GPIOB,GPIO_Pin_7,GPIO_Mode_IN_FLOATING}
#define DEM_10 {GPIOB,GPIO_Pin_8,GPIO_Mode_IN_FLOATING}
#define DEM_11 {GPIOB,GPIO_Pin_9,GPIO_Mode_IN_FLOATING}

#define DEM_12  {GPIOC,GPIO_Pin_0,GPIO_Mode_IN_FLOATING}

#define DEM_13  {GPIOE,GPIO_Pin_12,GPIO_Mode_IN_FLOATING}
#define DEM_14  {GPIOE,GPIO_Pin_13,GPIO_Mode_IN_FLOATING}
#define DEM_15  {GPIOE,GPIO_Pin_14,GPIO_Mode_IN_FLOATING}
#define DEM_16  {GPIOE,GPIO_Pin_15,GPIO_Mode_IN_FLOATING}

#define DEM_17  {GPIOE,GPIO_Pin_0,GPIO_Mode_IN_FLOATING}

#define DEM_18  {GPIOD,GPIO_Pin_3,GPIO_Mode_IN_FLOATING}   // new ȡ�����������

#define DEM_19  {GPIOD,GPIO_Pin_4,GPIO_Mode_IN_FLOATING}
#define DEM_20  {GPIOD,GPIO_Pin_7,GPIO_Mode_IN_FLOATING}
#define DEM_21  {GPIOE,GPIO_Pin_5,GPIO_Mode_IN_FLOATING}
#define DEM_22  {GPIOE,GPIO_Pin_6,GPIO_Mode_IN_FLOATING}

#define MOTO_STEP1  40  //Pin_Array����õ�40��Ԫ��
#define MOTO_STEP2  41  //Pin_Array����õ�41��Ԫ��
#define PUL_STEP1   38  
#define PUL_STEP2   39  

#define MOTO_STEP1_LEFT_RUN   GPIO_ResetBits(Pin_Array[MOTO_STEP1].port,Pin_Array[MOTO_STEP1].pin)
#define MOTO_STEP1_RIGHT_RUN  GPIO_SetBits(Pin_Array[MOTO_STEP1].port,Pin_Array[MOTO_STEP1].pin)
#define MOTO_STEP1_PUL_OFF     GPIO_ResetBits(Pin_Array[PUL_STEP1].port,Pin_Array[PUL_STEP1].pin)
#define MOTO_STEP1_PUL_ON  GPIO_SetBits(Pin_Array[PUL_STEP1].port,Pin_Array[PUL_STEP1].pin)

#define MOTO_STEP2_LEFT_RUN   GPIO_ResetBits(Pin_Array[MOTO_STEP2].port,Pin_Array[MOTO_STEP2].pin)
#define MOTO_STEP2_RIGHT_RUN  GPIO_SetBits(Pin_Array[MOTO_STEP2].port,Pin_Array[MOTO_STEP2].pin)
#define MOTO_STEP2_PUL_OFF     GPIO_ResetBits(Pin_Array[PUL_STEP2].port,Pin_Array[PUL_STEP2].pin)
#define MOTO_STEP2_PUL_ON  GPIO_SetBits(Pin_Array[PUL_STEP2].port,Pin_Array[PUL_STEP2].pin)


#define MOTO_CLAIM_GOODS_LEFT   34 //Pin_Array����õ�34��Ԫ�� ����ȡ�����
#define MOTO_CLAIM_GOODS_RIGHT  35

#define MOTO_CLAIM_GOODS_LEFT_RUN   {GPIO_SetBits(Pin_Array[MOTO_CLAIM_GOODS_LEFT].port,Pin_Array[MOTO_CLAIM_GOODS_LEFT].pin);\
		GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_RIGHT].port,Pin_Array[MOTO_CLAIM_GOODS_RIGHT].pin);}

#define MOTO_CLAIM_GOODS_RIGHT_RUN  {GPIO_SetBits(Pin_Array[MOTO_CLAIM_GOODS_RIGHT].port,Pin_Array[MOTO_CLAIM_GOODS_RIGHT].pin);\
		GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_LEFT].port,Pin_Array[MOTO_CLAIM_GOODS_LEFT].pin);}

#define MOTO_CLAIM_GOODS_CLOSE   {GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_LEFT].port,Pin_Array[MOTO_CLAIM_GOODS_LEFT].pin);\
		GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_RIGHT].port,Pin_Array[MOTO_CLAIM_GOODS_RIGHT].pin);}

#define MOTO_CLAIM_GOODS_STOP_RUN {GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_LEFT].port,Pin_Array[MOTO_CLAIM_GOODS_LEFT].pin);delay_ms(5);\
		GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_RIGHT].port,Pin_Array[MOTO_CLAIM_GOODS_RIGHT].pin);}
																		
#define MOTO_CLAIM_GOODS_STOP       GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_RIGHT].port,Pin_Array[MOTO_CLAIM_GOODS_RIGHT].pin);\
																		GPIO_ResetBits(Pin_Array[MOTO_CLAIM_GOODS_LEFT].port,Pin_Array[MOTO_CLAIM_GOODS_LEFT].pin)

#define MOTO_UNCAP_LEFT   36 //Pin_Array����õ�36��Ԫ��
#define MOTO_UNCAP_RIGHT  37

#define MOTO_UNCAP_LEFT_RUN   GPIO_SetBits(Pin_Array[MOTO_UNCAP_LEFT].port,Pin_Array[MOTO_UNCAP_LEFT].pin);\
									GPIO_ResetBits(Pin_Array[MOTO_UNCAP_RIGHT].port,Pin_Array[MOTO_UNCAP_RIGHT].pin);
#define MOTO_UNCAP_RIGHT_RUN  GPIO_SetBits(Pin_Array[MOTO_UNCAP_RIGHT].port,Pin_Array[MOTO_UNCAP_RIGHT].pin);\
									GPIO_ResetBits(Pin_Array[MOTO_UNCAP_LEFT].port,Pin_Array[MOTO_UNCAP_LEFT].pin);

#define MOTO_UNCAP_STOP  GPIO_ResetBits(Pin_Array[MOTO_UNCAP_RIGHT].port,Pin_Array[MOTO_UNCAP_RIGHT].pin);\
							GPIO_ResetBits(Pin_Array[MOTO_UNCAP_LEFT].port,Pin_Array[MOTO_UNCAP_LEFT].pin);


#define MOTO_CLAIM_GOODS_RF 33 //ȡ������ú�����
#define MOTO_OUT_GOODS      17 //�������

#define OPEN_17_MOTOR  GPIO_SetBits(Pin_Array[16].port,Pin_Array[16].pin);
#define CLOSE_17_MOTOR  GPIO_ResetBits(Pin_Array[16].port,Pin_Array[16].pin);

enum
{
  state_stop = 0,
  state_repay,
	state_borrow,
	state_run_first,
	state_error,
	state_run_second,
	state_run_third,
	state_report,
	state_DBG_open,
	state_door_open
};

/*����ṹ��
 * open_moto�� �����������
 * close_moto�� �رյ������
 * read_moto��  ��ȡ״̬
 * num�� ������
 * state��ִ��״̬
*/
typedef struct
{
	uint8_t(*open_moto)(uint8_t);
	uint8_t(*close_moto)(uint8_t);
	uint8_t(*read_moto)(uint8_t);
	uint8_t num;
	uint8_t state;
}Moto;

/*��������ṹ��
 *moto_state:  �������
 *lock_state:  �黹���
 *DBG_state:
 *state: ����״̬ 
*/
typedef struct
{
	uint8_t lock_state;
	uint8_t moto_state;
	uint8_t DBG_state;
	uint8_t state;
}Machine;

/*����ṹ��
 *android_state: ��׿�ظ�״̬
 *error_count: �������
 *bar_code_state: �����յ�
*/
typedef struct
{
	uint8_t android_state;
	uint8_t error_count;
	uint8_t bar_code_state;
}mError;

/*12�ֽ�*/
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
	GPIOMode_TypeDef mode;
}mPin;

void Moto_Init(void);
uint8_t Open_xMoto(uint8_t num);
uint8_t Close_xMoto(uint8_t num);
uint8_t Read_xMoto(uint8_t num);
void Start_Step(uint8_t num,uint8_t dir,uint8_t step);
void xStep_moto(uint8_t num,uint8_t step);
void SingStep(uint8_t num);
void X_One_Step(void);
void Z_One_Step(void);
void ungap_up(void);
void ungap_down(void);
void x_down(void);
void z_down(void);
void x_down_origin(void);
void x_z_sametime(uint32_t z_step,uint32_t x_step,uint8_t dir);
void x_sametime(uint32_t x_step,uint8_t dir);
void x_z_sametime_begin(uint32_t zin, uint32_t xin, uint8_t dir);
#endif

