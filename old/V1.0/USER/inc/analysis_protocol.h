#ifndef _ANALYSIS_PROTOCOL_H
#define _ANALYSIS_PROTOCOL_H

#include "stm32f10x.h"

#define COMMAND_MAX   512
#define COMMAND_RFID_MAX    120

#define   GENEAL             1
#define   PARAMETE_REPORT    2
#define   HEART_BEAR_REPORT  3
#define   GAME_OVER_REPORT   4    


void Rfid_CommandReceive_Poll(void);
void DBG_CommandReceive_Poll(void);
void SocketData_Receive_Poll(void);

void Uart_Protocol_Cmd_Analy(uint8_t* CmdRecBuf,uint8_t length);
void ArrivePath(uint8_t *dat, uint16_t len);
int32_t Json_Pack(char *buf,char* cmd,uint8_t a);
#endif

