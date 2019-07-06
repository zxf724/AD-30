/*
 * @Author: ��ʩ��
 * @Description: 
 * @Version: V1.0
 * @Date: 2019-06-06 10:14:10
 * @LastEditors: OBKoro1
 * @LastEditTime: 2019-06-20 17:56:01
 * @locale: zh-CN
 */
#include "moto.h"
#include "analysis_protocol.h"
#include "schedule.h"
#include "string.h"
#include "usart.h"
#include "ananlysis_data.h"
#include "delay.h"
#include "m_w5500_conf.h"
#include "rtc.h"
#include "wdg.h"
#include "stm32f10x_tim.h"
#include "timer.h"

#define X_Z_BEGIN	7200
#define Z_DISTANCE	7900
#define X_DISTANCE  335
#define ORIGIN		35
#define DEM18	46

extern uint8_t rfid_flag;
extern uint8_t flag_test1;

mMachine machineDef={NULL,NULL,NULL,{5,5,5},0,{0,0,0},NULL,0,0,0,0,0};

extern uint8_t g_start_cmd[7];
extern Moto motoDef;
extern Machine machine;
extern mError errorDef;
extern mPin Pin_Array[];
extern uint8_t flag_ungap_up;
extern uint8_t Tim2_flag;

uint32_t z_step_origin = 0;
uint32_t x_step_origin = 0;
uint8_t flag_back = 0;
uint8_t flag_x_back = 0;

void Start_Schedule()	
{
	static uint8_t flag_rfid = 0;
	uint8_t buff[512] = {0};
	switch(machineDef.state)
	{
		IWDG_Feed();
		case stop: //����״̬
 				IWDG_Feed();
			  if(motoDef.num)
			  {		
					// ���㲽�� Start
					uint32_t z_step = (X_Z_BEGIN+(Z_DISTANCE*(3-((motoDef.num-1)/4))));
					uint32_t x_step = X_DISTANCE*(3-((motoDef.num-1)%4)) + ORIGIN;
					DBG_LOG("z_step = %d",z_step);
					DBG_LOG("x_step = %d",x_step);
					z_step_origin = z_step;
					x_step_origin = x_step;
					// // ���㲽�� end
					rfid_flag = 0;
					// x_z_sametime(z_step,x_step,0);
					x_z_sametime(z_step_origin,0,0);
					x_sametime(x_step_origin,1);
					// Json_Pack((char*)buff,"CMD-99",GENEAL);
					MOTO_CLAIM_GOODS_LEFT_RUN;
					// ��ʱ������Ƿ��л�
 					flag_rfid = 1;
					 if((motoDef.open_moto((17-motoDef.num))) == 0) {
						machineDef.state = track;
					}
			  }
			break;
		case track: //	�Ĵ���
				if((GPIO_ReadInputDataBit(Pin_Array[(17-motoDef.num)].port,Pin_Array[(17-motoDef.num)].pin)) == 0) {
					motoDef.close_moto((17-motoDef.num));
				}
			if((rfid_flag == 1) && (flag_rfid == 1)) {
				MOTO_CLAIM_GOODS_STOP_RUN;
				flag_back = 1;
				machineDef.state = back;
			}
		break;
		case back: //��ȥ
			IWDG_Feed();
			if (flag_back == 1) {
				flag_back = 0;
				x_z_sametime(z_step_origin,0,1);
				x_sametime((x_step_origin-ORIGIN),0);
			}
			// show 
			if((rfid_flag == 1) && (flag_rfid == 1)) {
				motoDef.open_moto((17-motoDef.num));
				flag_test1 = 1;
				TIM_Cmd(TIM3, ENABLE);
				ungap_down();
				delay_ms_whx(1000);
				ungap_up();
				machineDef.state = showoff;
			}
		break;
		case showoff:
			MOTO_CLAIM_GOODS_RIGHT_RUN;
			OPEN_17_MOTOR;
			delay_ms_whx(1000);
			IWDG_Feed();
			if((GPIO_ReadInputDataBit(Pin_Array[16+17].port,Pin_Array[16+17].pin)) == 0) {
				MOTO_CLAIM_GOODS_STOP_RUN;
				CLOSE_17_MOTOR;
				flag_x_back = 1;
				machineDef.state = still_run;	
			}
		break;
		case still_run: //�ϱ�
			if(flag_x_back == 1){
				flag_x_back = 0;
				x_sametime(ORIGIN,0);
			}
			machineDef.state = report;
		break;
		case report:
			// Json_Pack((char*)buff,"CMD-103",GAME_OVER_REPORT);
			motoDef.num = 0;
			flag_rfid = 0;
			rfid_flag = 0;
			machineDef.state = stop;
			break;
	}
}
