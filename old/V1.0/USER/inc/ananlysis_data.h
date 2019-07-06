#ifndef _ANALYSIS_DATA_H
#define _ANALYSIS_DATA_H

#include "sys.h"
#include "cJSON.h"

void Get_Time(uint8_t* data);
void Get_Mote_Data(uint8_t data);
void Get_NewIP(cJSON* ip);

uint8_t Check_Moto(uint8_t num);
void Report_State(uint8_t cmd,uint8_t* data,uint8_t len);
void Get_Bar_Code(uint8_t* data);
#endif
