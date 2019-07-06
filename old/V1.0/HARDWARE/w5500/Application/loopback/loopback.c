#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "m_w5500_conf.h"

extern mSocket  SocketDef;

#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
	int32_t ret;

	#ifdef _LOOPBACK_DEBUG_
	uint8_t destip[4];
	uint16_t destport;
	#endif

	switch(getSn_SR(sn))
	{
		case SOCK_ESTABLISHED :
			if(getSn_IR(sn) & Sn_IR_CON)
			{
				#ifdef _LOOPBACK_DEBUG_
				getSn_DIPR(sn, destip);
				destport = getSn_DPORT(sn);

				printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
				#endif
				setSn_IR(sn,Sn_IR_CON);
				SocketDef.state = 1;
			}
		break;
		case SOCK_CLOSE_WAIT :
			#ifdef _LOOPBACK_DEBUG_
			//printf("%d:CloseWait\r\n",sn);
			#endif
		if((ret = disconnect(sn)) != SOCK_OK) return ret;
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:Socket Closed\r\n", sn);
			#endif
		  SocketDef.state = 0;
		break;
		case SOCK_INIT :
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
			#endif
		if( (ret = listen(sn)) != SOCK_OK) return ret;
		break;
		case SOCK_CLOSED:
			#ifdef _LOOPBACK_DEBUG_
			//printf("%d:TCP server loopback start\r\n",sn);
			#endif
		if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
			#ifdef _LOOPBACK_DEBUG_
			//printf("%d:Socket opened\r\n",sn);
			#endif
		break;
		default:
		break;
	}
	return 1;
}


int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport)
{
   int32_t ret; 

   static uint16_t any_port = 50000;

   switch(getSn_SR(sn))
   {
			case SOCK_ESTABLISHED :
			if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
			{
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
			#endif
			 setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
			 SocketDef.state  = mconnect;
			}
			break;

			case SOCK_CLOSE_WAIT :
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:CloseWait\r\n",sn);
			#endif
			if((ret=disconnect(sn)) != SOCK_OK) return ret;
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:Socket Closed\r\n", sn);
			#endif
			 SocketDef.state = mdisconnect;
			break;

			case SOCK_INIT :
			#ifdef _LOOPBACK_DEBUG_
				printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
			#endif 	
			if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
			break;

			case SOCK_CLOSED:
				printf("%d:close\r\n",sn);
			close(sn);
			if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){
				if(any_port == 0xffff) any_port = 50000;
				return ret; // TCP socket open with 'any_port' port number
			} 
			#ifdef _LOOPBACK_DEBUG_
			//printf("%d:TCP client loopback start\r\n",sn);
			//printf("%d:Socket opened\r\n",sn);
			#endif
			break;
			default:
			break;
   }
   return 1;
}


int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;

		switch(getSn_SR(sn))
		{
			case SOCK_UDP :
				if((size = getSn_RX_RSR(sn)) > 0)
				{
					if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
					ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
					if(ret <= 0)
					{
						#ifdef _LOOPBACK_DEBUG_
						printf("%d: recvfrom error. %ld\r\n",sn,ret);
						#endif
						return ret;
					}
					size = (uint16_t) ret;
					sentsize = 0;
					while(sentsize != size)
					{
						ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
						if(ret < 0)
						{
						#ifdef _LOOPBACK_DEBUG_
						printf("%d: sendto error. %ld\r\n",sn,ret);
						#endif
						return ret;
						}
						sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
					}
				}
			break;
			case SOCK_CLOSED:
			#ifdef _LOOPBACK_DEBUG_
				//printf("%d:UDP loopback start\r\n",sn);
				#endif
				if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
				return ret;
				#ifdef _LOOPBACK_DEBUG_
					printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
				#endif
			break;
			default :
			break;
		}
		return 1;
}

#endif
