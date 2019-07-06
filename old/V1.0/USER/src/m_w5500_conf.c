#include "spi.h"
#include "wizchip_conf.h"
#include "usart.h"
#include "m_w5500_conf.h"
#include <string.h>
#include "dhcp.h"
#include "loopback.h"
#include "dns.h"
#include "timer.h"
#include "delay.h"
#include "analysis_protocol.h"
#include "schedule.h"
#include "socket.h"

uint8_t ar[16] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}; 
uint8_t gDATABUF[DATA_BUF_SIZE];

char*  deviceid = "PAD06-0001";
char*  hard_ver = "1.0";
char*  soft_ver = "1.0";
uint8_t domain_name[]="www.baidu.com";
uint8_t domain_ip[4]={0};
uint8_t heart_time = 0;

extern mSocket SocketDef;
extern mMachine machineDef;

wiz_NetInfo NetConf = {
  {0x0c,0x29,0xab,0x7c,0x04,0x03}, // mac
//   {169,254,227,40},                 // IP
  {120,76,56,50},
  {255,255,255,0},                 //子网掩码
  {169,254,227,51},                   //网关
  {169,254,227,51},                   // DNS
  NETINFO_STATIC                   
};

#if 0
 uint8_t  serve_ip[] =  {192,168,0,60};
#else
//  uint8_t  serve_ip[] =  {120,76,56,120};
//  uint8_t  serve_ip[] =  {169,254,227,51};
 uint8_t  serve_ip[] =  {120,76,56,120};
#endif
 

void W5500_Init(void)
{	
	uint8_t tmp;
	SPI2_Init();		   	//初始化SPI
	Reset_W5500();
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//注册临界区函数
	reg_wizchip_spi_cbfunc(SPI2_ReadByte,SPI2_WriteByte);
  	reg_wizchip_cs_cbfunc(CS_Low,CS_High);
	Set_Machine(deviceid,soft_ver,hard_ver,1883,serve_ip,&NetConf);  // old 1883
 	if(!ctlwizchip(CW_INIT_WIZCHIP,ar))
	  printf("WIZCHIP Initialized success.\r\n");
	
	do {
	 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1) {
			printf("Unknown PHY Link stauts.\r\n");
	 }
	} while(tmp == PHY_LINK_OFF);
}

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOB, 10);
  delay_us(2);
  GPIO_SetBits(GPIOB, 10);
  delay_ms(1600);
}


void NetworkParameterConfiguration(wiz_NetInfo* netinfo)
{
  uint8_t tmpstr[6];
  wiz_NetInfo conf;
	
  ctlwizchip(CW_GET_ID,(void*)tmpstr);
	printf("id is %s\r\n",tmpstr);
  while(memcmp(&conf,netinfo,sizeof(wiz_NetInfo)) != 0){
		 ctlnetwork(CN_SET_NETINFO, (void*)netinfo);
     ctlnetwork(CN_GET_NETINFO, (void*)&conf);
     printf("net fail\r\n");
  }
  printf("net success\r\n");
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",conf.mac[0],conf.mac[1],conf.mac[2],
			conf.mac[3],conf.mac[4],conf.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", conf.ip[0],conf.ip[1],conf.ip[2],conf.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", conf.gw[0],conf.gw[1],conf.gw[2],conf.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", conf.sn[0],conf.sn[1],conf.sn[2],conf.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", conf.dns[0],conf.dns[1],conf.dns[2],conf.dns[3]);
	printf("===========================\r\n");
}
  
void DHCP_SET(void)
{
		uint8_t dhcp_ret = 0; 
 
	  wiz_NetInfo conf;
	  setSHAR(NetConf.mac);
		DHCP_init(machineDef.netcon.dhcp_socket,gDATABUF);
		reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);
		while(dhcp_ret != DHCP_IP_LEASED)
		{
			dhcp_ret = DHCP_run();
		}
    ctlnetwork(CN_GET_NETINFO, (void*)&conf);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",conf.mac[0],conf.mac[1],conf.mac[2],
            conf.mac[3],conf.mac[4],conf.mac[5]);
    printf("SIP: %d.%d.%d.%d\r\n", conf.ip[0],conf.ip[1],conf.ip[2],conf.ip[3]);
    printf("GAR: %d.%d.%d.%d\r\n", conf.gw[0],conf.gw[1],conf.gw[2],conf.gw[3]);
    printf("SUB: %d.%d.%d.%d\r\n", conf.sn[0],conf.sn[1],conf.sn[2],conf.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", conf.dns[0],conf.dns[1],conf.dns[2],conf.dns[3]);
    printf("======================\r\n");
}

void my_ip_assign(void)
{
    getIPfromDHCP(NetConf.ip);
    getGWfromDHCP(NetConf.gw);
    getSNfromDHCP(NetConf.sn);
    getDNSfromDHCP(NetConf.dns);
    NetConf.dhcp = NETINFO_DHCP;

    ctlnetwork(CN_SET_NETINFO, (void*)&NetConf);     

    printf("DHCP LEASED TIME : %d Sec.\r\n", getDHCPLeasetime());
}

void my_ip_conflict(void)
{
   printf("CONFLICT IP from DHCP\r\n");
   while(1); 
}

void Dns_Initation()
{
	int8_t ret = 0;
  DNS_init(machineDef.netcon.dns_socket, gDATABUF);
	Timer_Start();
	if ((ret = DNS_run(NetConf.dns, domain_name, domain_ip)) > 0){ // try to 1st DNS
		printf("> 1st DNS Reponsed\r\n");
	}else if(ret == -1){
		printf("> MAX_DOMAIN_NAME is too small. Should be redefined it.\r\n");
		Timer_Stop();
		while(1);
	}else{
		printf("> DNS Failed\r\n");
		Timer_Stop();
		while(1);
	}
	//DNS解析成功
	if(ret > 0){
		printf("> Translated %s to %d.%d.%d.%d\r\n",domain_name,domain_ip[0],domain_ip[1],domain_ip[2],domain_ip[3]);
	}
	Timer_Stop();
}

void Send_Heart_Beat()
{
	static uint8_t flag = 0;
	uint8_t buf[512] = {0};
	int32_t len = 0;
	int32_t ret = 0; 
  if(SocketDef.state == mconnect && !flag) 
	{
		flag = 1;
		Timer2_Start();
	}else if(!SocketDef.state)
	{
		flag  = 0;
		Timer2_Stop();
	}
	if(SocketDef.state == mconnect &&  heart_time) //上传心跳包
	{
		heart_time = 0;
		len = Json_Pack((char*)buf,"CMD-100",HEART_BEAR_REPORT);
		printf("len ok  = %d\r\n",len);
		ret = send(machineDef.netcon.tcp_socket,buf,len);
		if(ret == len)
		{
		  printf("send ok  = %d\r\n",len);
		}else
		{
		  printf("ret=%d\r\n",ret);
		}
	}
}

void Set_Machine(char* deviceid,char* soft_ver,char* hard_ver,uint16_t port,uint8_t* ser_ip,wiz_NetInfo* netinfo)
{
  machineDef.deviceid = deviceid;
	machineDef.soft_ver = soft_ver;
	machineDef.hard_ver = hard_ver;
	machineDef.netcon.dhcp_socket = M_DHCP_SOCKET;
	machineDef.netcon.dns_socket  = M_DNS_SOCKET;
	machineDef.netcon.tcp_socket =  M_TCP_SOCKET;
	machineDef.netcon.tcp_port = port;
	machineDef.netcon.server_ip = ser_ip;
	NetworkParameterConfiguration(netinfo);
}
