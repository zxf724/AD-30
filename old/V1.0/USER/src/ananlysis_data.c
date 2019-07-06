#include "ananlysis_data.h"
#include "moto.h"
#include "delay.h"
#include "string.h"
#include "analysis_protocol.h"
#include "usart.h"
#include "fifo.h"
#include "cJSON.h"
#include "prjlib.h"
#include "schedule.h"

extern Moto motoDef;
extern mError errorDef;
extern uint8_t  serve_ip[];
extern mMachine machineDef;

uint8_t g_start_cmd[7] = {0};
uint8_t g_bar_code[50] = {'\0'};

void Get_Time(uint8_t* data)
{
   
}

void Get_Mote_Data(uint8_t data)
{
	motoDef.num = data;
	printf("rec mote %d\r\n",motoDef.num);
}

void Get_NewIP(cJSON* ip)
{
	DBG_LOG("Get  ip...");					
	serve_ip[0] = uatoi(ip->valuestring);
	serve_ip[1] = uatoi(strstr(ip->valuestring,".")+1);
	serve_ip[2] = uatoi(strstr(strstr(ip->valuestring,".")+1,".")+1);
	serve_ip[3] = uatoi(strstr((strstr(strstr(ip->valuestring,".")+1,".")+1),".")+1);
	machineDef.netcon.server_ip = serve_ip;
	DBG_LOG("ip is %d,%d,%d,%d",serve_ip[0],serve_ip[1],serve_ip[2],serve_ip[3]);
}


void Report_State(uint8_t cmd,uint8_t* data,uint8_t len)
{

}


