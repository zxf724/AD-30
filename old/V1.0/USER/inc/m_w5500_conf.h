#ifndef __M_W5500_CONF_H
#define __M_W5500_CONF_H

#include <stdint.h>
#include "wizchip_conf.h"
#define  M_TCP_SOCKET  4
#define  M_DNS_SOCKET  2
#define  M_DHCP_SOCKET 3


typedef enum
{
	mdisconnect = 0,
  mconnect
}mState;

typedef struct	
{
  uint8_t state;
}mSocket;


void W5500_Init(void);

void NetworkParameterConfiguration(wiz_NetInfo* netinfo);
void network_init(void);

void DHCP_SET(void);
void Dns_Initation(void);
void my_ip_assign(void);
void my_ip_conflict(void);
void Send_Heart_Beat(void);
void Set_Machine(char* deviceid,char* soft_ver,char* hard_ver,uint16_t port,uint8_t* ser_ip,wiz_NetInfo* netinfo);
#endif



