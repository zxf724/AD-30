/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date         Author         Notes
 * 2019.2.8     Yang Song      first version.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AFUIOT_MAIN_H_
#define AFUIOT_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* External functions --------------------------------------------------------*/

void ProtocolInit(void);
BOOL CMD_Updata(char *cmd, cJSON *desired);
void Get_NewIP(cJSON* ip);
static BOOL CMD_Confirm_Rsp(char *ordermsgid);
void Get_Mote_Data(uint8_t data);
void return_all_parameter(void);


#ifdef __cplusplus
}
#endif

#endif