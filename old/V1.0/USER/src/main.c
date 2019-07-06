#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "fifo.h"
#include "analysis_protocol.h"
#include "prjlib.h"
#include "rtc.h"
#include "moto.h"
#include "schedule.h"
#include "wdg.h"
#include "ds1302.h"
#include "wizchip_conf.h"
#include <string.h>
#include "spi.h"
#include "dhcp.h"
#include "cJSON.h"
#include "dns.h"	
#include "socket.h"
#include "m_w5500_conf.h"
#include "moto.h"
#include "stm32f10x_gpio.h"
#include "schedule.h"
#include "moto.h"

extern mMachine machineDef;

extern uint8_t write[];
extern uint8_t read[];
extern Moto motoDef;
extern mPin Pin_Array[];
extern uint8_t rfid_flag;
uint8_t time[] = {0,0,11,29,3,5,19};
extern uint8_t Tim2_flag;

static void funControl(int argc, char* argv[]);

_calendar_obj fff;
extern _calendar_obj calendar;//ʱ�ӽṹ�� 
uint8_t time_test;

uint8_t my_dhcp_retry = 0; 

int main(void)
{
  CMD_ENT_DEF(MOTO, funControl);
 	Cmd_AddEntrance(CMD_ENT(MOTO));
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart2_init(38400);
	uart3_init(115200);
	TIM3_Int_Init(7199,9);		// 200,36000	
	TIM2_Int_Init(49999,14399);	
	// TIM4_Int_Init(80000,14399);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
	Moto_Init();
	ds1302_GPIO_Configuration();
	ds1302_init(write,time);
	write_1302(0x80,0); 
	IWDG_Init(6,1024);    //���Ƶ���?64,����ֵΪ625,���ʱ���?1s	
	
	RTC_Get();
	time_test = calendar.min;
	// DBG_LOG("hello,world!");
	// z_down();
	// x_down();

	// W5500_Init();
	// DHCP_SET();
 	// Dns_Initation();	no use!
	// if(socket(machineDef.netcon.tcp_socket,Sn_MR_TCP,5000,SF_IO_NONBLOCK) != M_TCP_SOCKET){
	// 	printf("%d:Socket Error\r\n",machineDef.netcon.tcp_socket);
	// }else{
	// 	printf("%d:Opened\r\n",machineDef.netcon.tcp_socket);
	// }
	DBG_LOG("system start\r\n");
	while(1) {
	  IWDG_Feed();
	//  time = mytime_2_utc_sec(&fff,0);
		DBG_CommandReceive_Poll();
		if(rfid_flag != 1) {
	 		Rfid_CommandReceive_Poll();
		}
		// SocketData_Receive_Poll();		// hart_heat
		Start_Schedule();
		test();
		// DBG_LOG("hello,world!");
	}
}

static void funControl(int argc, char* argv[]) {

	uint8_t i = 0;
  argv++;
  argc--;
  if(ARGV_EQUAL("RUN_ALL"))
	{
	  for(i = 1;i < 18;i++)
		{
		   motoDef.open_moto(i);
		}	
	}else if(ARGV_EQUAL("CLOSE_ALL")) 
	{  
	   for(i = 0;i < 17;i++)
		 {
		   motoDef.close_moto(i);
		 }
	}else if(ARGV_EQUAL("RUN")) 
	{
		  motoDef.open_moto(uatoi(argv[1]));
	}else if(ARGV_EQUAL("CLOSE")) 
	{  
		  motoDef.close_moto(uatoi(argv[1]));
	}else if(ARGV_EQUAL("GOODS_RIGHT")) //		in
	{
	   DBG_LOG("MOTO_CLAIM_GOODS_RIGHT_RUN");
	   MOTO_CLAIM_GOODS_RIGHT_RUN;
	}else if(ARGV_EQUAL("GOODS_LEFT"))		// out
	{
	   DBG_LOG("MOTO_CLAIM_GOODS_LEFT_RUN");
	   MOTO_CLAIM_GOODS_LEFT_RUN;
	}else if(ARGV_EQUAL("OUT_GOODS")) // 
	{
		DBG_LOG("hello,world!1");
		OPEN_17_MOTOR;
	}else if(ARGV_EQUAL("OUT_GOODS_OFF")) // 
	{
		DBG_LOG("hello,world!1");
		CLOSE_17_MOTOR;
	}
	else if(ARGV_EQUAL("UNCAP_LEFT"))  //   down
	{
		DBG_LOG("hello,world!2");
		ungap_down();
	}else if(ARGV_EQUAL("UNCAP_RIGHT")) //   up
	{
		DBG_LOG("hello,world!3");
		ungap_up();
	}else if(ARGV_EQUAL("UNCAP_STOP")) { 
		MOTO_UNCAP_STOP;
	} else if(ARGV_EQUAL("STEP1_RIGHT"))  //  86 z down
	{
		// 3*34
		DBG_LOG("hello,world!4");
		x_z_sametime(uatoi(argv[1]),0,1);
	}else if(ARGV_EQUAL("STEP1_LEFT"))  //  86 z up
	{
		DBG_LOG("hello,world!45");
		x_z_sametime(uatoi(argv[1]),0,0);
	}else if(ARGV_EQUAL("STEP2_RIGHT"))//  57  LEFT
	{
		DBG_LOG("hello,world!24");
		x_z_sametime(0,uatoi(argv[1]),0);
	}else if(ARGV_EQUAL("STEP2_LEFT"))   //  57  RIGHT
	{
		DBG_LOG("hello,world!42");
		x_z_sametime(0,uatoi(argv[1]),1);
	} else if(ARGV_EQUAL("RUN_17")) {
		DBG_LOG("hello,world!5");
		// motoDef.open_moto(17);
		GPIO_SetBits(GPIOE,GPIO_Pin_11);
	} else if(ARGV_EQUAL("RUN_17_T")) {
		DBG_LOG("hello,world!6"); 
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
	} else if(ARGV_EQUAL("NUM_START")) {		// NUM_START
		DBG_LOG("hello,world!18"); 
		motoDef.num = uatoi(argv[1]);
 	} else if(ARGV_EQUAL("RESET")) {
		z_down();
		x_down();
	} else if (ARGV_EQUAL("X_Z_SAMETIME")) {
		DBG_LOG("hello,world!123");
		x_z_sametime(5000,500,0);
	} else if (ARGV_EQUAL("X_Z_SAMETIME_BACK")) {
		x_z_sametime(5000,2000,1);
	} else if(ARGV_EQUAL("X_SAME_TIME")) {
		DBG_LOG("hello,world!78"); 
		x_sametime(uatoi(argv[1]),1);
 	} else if(ARGV_EQUAL("X_SAME_TIME_BACK")) {
		DBG_LOG("hello,world!78"); 
		x_sametime(uatoi(argv[1]),0);
 	} else if (ARGV_EQUAL("TEST")) {
		DBG_LOG("hello,world!7");
	}
}

void z_down(void) {
	DBG_LOG("hello,world!8");
	while(1) {
		if((GPIO_ReadInputDataBit(Pin_Array[42].port,Pin_Array[42].pin)) != 0) {
			MOTO_STEP1_RIGHT_RUN;
			Z_One_Step();
		}
		if((GPIO_ReadInputDataBit(Pin_Array[42].port,Pin_Array[42].pin)) == 0) break;
	}
}

void x_down(void) {
	DBG_LOG("hello,world!9");
	while(1) {
		if((GPIO_ReadInputDataBit(Pin_Array[45].port,Pin_Array[45].pin)) != 0) {
			x_sametime(1,0);
		}
		if((GPIO_ReadInputDataBit(Pin_Array[45].port,Pin_Array[45].pin)) == 0) break;
	}
}

void x_down_origin(void) {
	while(1) {
		if((GPIO_ReadInputDataBit(Pin_Array[45].port,Pin_Array[45].pin)) != 0) {
			Start_Step(2,1,1);
		}
		if((GPIO_ReadInputDataBit(Pin_Array[45].port,Pin_Array[45].pin)) == 0) break;
	}
		delay_ms(100);
		Start_Step(2,0,5);		// x
}

void ungap_up(void) {
	DBG_LOG("hello,world!10");
	IWDG_Feed();
	while (1) {
		if((GPIO_ReadInputDataBit(Pin_Array[43].port,Pin_Array[43].pin)) != 0) {
	   		MOTO_UNCAP_RIGHT_RUN;
			IWDG_Feed();
			DBG_LOG("hello,world!11");
		}
		if((GPIO_ReadInputDataBit(Pin_Array[43].port,Pin_Array[43].pin)) == 0) {
			MOTO_UNCAP_STOP;
			IWDG_Feed();
			break;
		}
	}
}

void ungap_down(void) {
	while (1) {
		if((GPIO_ReadInputDataBit(Pin_Array[44].port,Pin_Array[44].pin)) != 0) {	
			MOTO_UNCAP_LEFT_RUN;
			DBG_LOG("hello,world!12");
			IWDG_Feed();
		}
		if((GPIO_ReadInputDataBit(Pin_Array[44].port,Pin_Array[44].pin)) == 0) {
			MOTO_UNCAP_STOP;
			break;
		}
	}
}

void test(void) {
	// OPEN_17_MOTOR;
	// if(Tim2_flag == 1) {
	// 	DBG_LOG("hello,there!!");
	// 	Tim2_flag = 0;
	// 	TIM_Cmd(TIM3,DISABLE);
	// }
	// MOTO_CLAIM_GOODS_LEFT_RUN;

	// delay_ms_whx test
	// IWDG_Feed();
	// delay_ms_whx(5000);
	// DBG_LOG("hello,world!");

	// delay_us(100);
	// MOTO_STEP1_PUL_ON;
	// delay_us(100);	
	// MOTO_STEP1_PUL_OFF;

	// if((GPIO_ReadInputDataBit(Pin_Array[16+17].port,Pin_Array[16+17].pin)) != 0) {
	// 	DBG_LOG("hello,world!");
	// }
	// if((GPIO_ReadInputDataBit(Pin_Array[16+17].port,Pin_Array[16+17].pin)) == 0) {
	// 	DBG_LOG("2222222");
	// }
	// if((GPIO_ReadInputDataBit(Pin_Array[46].port,Pin_Array[46].pin)) == 0) {
		// DBG_LOG("2222222");
	// }
	// if((GPIO_ReadInputDataBit(Pin_Array[46].port,Pin_Array[46].pin)) != 0) {
	// 	DBG_LOG("123456");
	// }
	// ungap_down();
	// delay_ms_whx(3000);
	// ungap_up();
	// MOTO_CLAIM_GOODS_LEFT_RUN;
	// if((GPIO_ReadInputDataBit(Pin_Array[46].port,Pin_Array[46].pin)) == 0) {
	// 	DBG_LOG("123456");
	// }
	// if((GPIO_ReadInputDataBit(Pin_Array[46].port,Pin_Array[46].pin)) == 1) {
	// 	DBG_LOG("hello,world!");
	// }
	// while(1) {
	// 	delay_us(100);
	// 	MOTO_STEP1_PUL_OFF;
	// 	delay_us(100);
	// 	MOTO_STEP1_PUL_ON; 
	// }
	// if((GPIO_ReadInputDataBit(Pin_Array[18].port,Pin_Array[18].pin)) == 0) {
	// 	DBG_LOG("hello,world!");
	// }
}
