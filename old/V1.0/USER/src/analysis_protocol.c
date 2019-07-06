#include "analysis_protocol.h"
#include "fifo.h"
#include "usart.h"	
#include "prjlib.h"
#include "ananlysis_data.h"
#include "delay.h"
#include "moto.h"
#include "cJSON.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "loopback.h"
#include "m_w5500_conf.h"
#include <stdlib.h>
#include "prjlib.h"
#include "timer.h"
#include "schedule.h"
#include "malloc.h"	 
#include "wdg.h"
#include "prjlib.h"

mSocket   SocketDef = {0};
uint8_t  CmdRecBuf[COMMAND_MAX] = {0};
uint8_t  CmdRecBuf_RFID[COMMAND_RFID_MAX] = {0};
uint8_t  socket_recive_buff[512] = {0};
extern uint8_t  serve_ip[4];
extern wiz_NetInfo NetConf;
extern mMachine machineDef;
extern app_fifo_t  rx_fifo_Rfid_Def;

uint8_t rfid_flag = 0;
void Rfid_CommandReceive_Poll(void) {
  uint32_t len = 0;
	uint8_t i = 0;
   len = fifo_length(&rx_fifo_Rfid_Def);
	if(len >= 5) {
		IWDG_Feed();
		delay_ms(100);
		len = fifo_length(&rx_fifo_Rfid_Def);
		for(i=0;i<len;i++) {
			IWDG_Feed();
			app_uart_get(&CmdRecBuf_RFID[i],RFID);   //one bit
		}
		Uart_Protocol_Cmd_Analy(CmdRecBuf_RFID,len); 
		len = 0;
	}	
}

void DBG_CommandReceive_Poll(void) 
{
  uint16_t index = 0;

	while(app_uart_get(&CmdRecBuf[index],DBG) == NRF_SUCCESS) 
  {  
		if (CmdRecBuf[index] == '\n') 
		{
			CmdRecBuf[index+1] = '\0';
			Cmd_Handle((char*)&CmdRecBuf[0]);
			index = 0;
		}else 
		{
			delay_ms(2);
			index++;
		}
  }
}

void SocketData_Receive_Poll(void) 
{
	uint16_t size = 0;
	int ret = 0;
	loopback_tcpc(machineDef.netcon.tcp_socket,socket_recive_buff,machineDef.netcon.server_ip,machineDef.netcon.tcp_port);
	if(SocketDef.state == mconnect)
	{
		if((size = getSn_RX_RSR(machineDef.netcon.tcp_socket)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
		{
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
			ret = recv(machineDef.netcon.tcp_socket, socket_recive_buff, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)
			if(ret <= 0) return; // If the received data length <= 0, receive failed and process end
			ArrivePath(socket_recive_buff,0);
		}
	}	
	Send_Heart_Beat();//上传心跳包
}

void Uart_Protocol_Cmd_Analy(uint8_t* CmdRecBuf_RFID,uint8_t length) {
		for(uint8_t i=0;i<=length;i++) {
			IWDG_Feed();
			DBG_LOG("CmdRecBuf_RFID[%d] = 0x%02x",i,CmdRecBuf_RFID[i]);
			DBG_LOG("length = %d",length);
		}
		if(length >= 5) {
			rfid_flag = 1;
			length = 0;
			IWDG_Feed();
			DBG_LOG("hello,world??");
		}
}
 
void ArrivePath(uint8_t *dat, uint16_t len)
{
	static cJSON *timestamp ;
	static cJSON * root = NULL,*cmd = NULL,*messageid = NULL ,*heartbeat = NULL ,* port= NULL;
	root = cJSON_Parse((const char *)dat);
	if (root != NULL)
	{
		timestamp = cJSON_GetObjectItem(root, "timestamp");
		if(timestamp != NULL && timestamp->type == cJSON_Number)
		{
			printf("this times tamp\r\n");     
		}
		messageid = cJSON_GetObjectItem(root, "messageid");
		cmd = cJSON_GetObjectItem(root, "cmd");
		if(cmd != NULL && messageid != NULL && messageid->type == cJSON_String)
		{
			if(strstr(cmd->valuestring, "CMD-01"))
			{
				DBG_LOG("Resert"); //单片机进行复位
				while(1);
			}else if(strstr(cmd->valuestring, "CMD-02"))
			{
				cJSON * desired = cJSON_GetObjectItem(root, "desired");
				if (desired && desired->type == cJSON_Object)
				{
					cJSON * ip = cJSON_GetObjectItem(desired, "ip");
					if(ip != NULL && ip->type == cJSON_String)
					{
              Get_NewIP(ip);
					}
					port = cJSON_GetObjectItem(desired, "port");
					if(port != NULL && port->type == cJSON_Number)
					{
						DBG_LOG("Set port ...");
						machineDef.netcon.tcp_port = port->valueint;
						DBG_LOG("port is %d",machineDef.netcon.tcp_port);
					}
					heartbeat = cJSON_GetObjectItem(desired, "heartbeat");
					if(heartbeat != NULL && heartbeat->type == cJSON_String)
					{
						DBG_LOG("Set heartbeat...");
						machineDef.netcon.ischange = 1;
						disconnect(machineDef.netcon.tcp_socket);
						Json_Pack((char*)CmdRecBuf,"CMD-99",GENEAL);
					}             
				}
			}else if(strstr(cmd->valuestring, "CMD-03"))
			{
				cJSON * desired = cJSON_GetObjectItem(root, "desired");
				if (desired && desired->type == cJSON_Object)
				{
					cJSON *cargoway = cJSON_GetObjectItem(desired, "cargoway");
					if(cargoway != NULL && cargoway->type == cJSON_Number)
					{					
						Get_Mote_Data(cargoway->valueint);					
					}else
					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
				}
			}else if(strstr(cmd->valuestring, "CMD-04"))
			{
				cJSON * desired = cJSON_GetObjectItem(root, "desired");
				if (desired && desired->type == cJSON_Object)
				{
					cJSON *inquiry  = cJSON_GetObjectItem(desired, "inquiry ");
					if(inquiry  != NULL && inquiry->type == cJSON_String)
					{
					  DBG_LOG("inquiry =  %s",inquiry->string);
						Json_Pack((char*)CmdRecBuf,"CMD-99",PARAMETE_REPORT);
					}  
				}
			}
		}
		cJSON_Delete(root);
	}else
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr());
	}
}


int32_t Json_Pack(char *buf,char* cmd,uint8_t a)
{
    cJSON *root;
    char *s;
    //char time[32];
	  char buff[20] = {'\0'};
		char ip[5] = {0};
    
    root = cJSON_CreateObject();
    if (root != NULL) 
		{
        cJSON *bodyMain;
        cJSON_AddStringToObject(root, "messageid","25");
			  //RTC_ReadTimeStr(time);
			  cJSON_AddNumberToObject(root, "timestamp",123123);
			  cJSON_AddStringToObject(root, "deviceid",machineDef.deviceid);
		  	cJSON_AddStringToObject(root, "cmd",cmd);
        cJSON_AddItemToObjectCS(root, "desired", bodyMain = cJSON_CreateObject());
			if(a == GENEAL)
			{
  			cJSON_AddStringToObject(bodyMain, "messageid","25");
				cJSON_AddStringToObject(bodyMain, "ret","ok");
			}else if(a == PARAMETE_REPORT)
			{
			  strcat(buff,strcat(uitoa(NetConf.ip[0],ip),"."));	
				strcat(buff,strcat(uitoa(NetConf.ip[1],ip),"."));
				strcat(buff,strcat(uitoa(NetConf.ip[2],ip),"."));
				strcat(buff,strcat(uitoa(NetConf.ip[3],ip),"\0"));;    
				
				cJSON_AddStringToObject(bodyMain, "ip",buff);
				cJSON_AddNumberToObject(bodyMain, "port",machineDef.netcon.tcp_port);
				cJSON_AddNumberToObject(bodyMain, "heartbeat ",60);
				cJSON_AddStringToObject(bodyMain, "project","AD-30");
				cJSON_AddStringToObject(bodyMain, "firmware",machineDef.soft_ver);
				cJSON_AddStringToObject(bodyMain, "hardware",machineDef.hard_ver);
				cJSON_AddStringToObject(bodyMain, "status","ok");
			}else if(a == HEART_BEAR_REPORT)
			{
			  cJSON_AddNumberToObject(bodyMain, "heartbeat",5);
			}else if(a == GAME_OVER_REPORT)
			{
			   cJSON_AddNumberToObject(bodyMain, "bagid",10000001);
			}
		  
			s = cJSON_PrintUnformatted(root);
			if(s != NULL)
			{
					strcpy(buf,s);
					myfree(s);
			}
			cJSON_Delete(root);
    }else
		  printf("cJSON_CreateObject fail\r\n");
    return strlen(buf);
}
