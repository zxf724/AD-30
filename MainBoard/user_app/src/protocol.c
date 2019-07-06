/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date         Author         Notes
 * 2019.2.8     Yang Song      first version.
 */

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

void ArrivePath(uint8_t* dat, uint16_t len) {
  static cJSON* timestamp;
  static cJSON* root = NULL, *cmd = NULL, *messageid = NULL, *heartbeat = NULL, *port = NULL;
  root = cJSON_Parse((const char*)dat);
  if (root != NULL) {
    timestamp = cJSON_GetObjectItem(root, "timestamp");
    if (timestamp != NULL && timestamp->type == cJSON_Number) {
      printf("this times tamp\r\n");
    }
    messageid = cJSON_GetObjectItem(root, "messageid");
    cmd = cJSON_GetObjectItem(root, "cmd");
    if (cmd != NULL && messageid != NULL && messageid->type == cJSON_String) {
      if (strstr(cmd->valuestring, "CMD-01")) {
        DBG_LOG("Resert");
        while (1);
      } else if (strstr(cmd->valuestring, "CMD-02")) {
        cJSON* desired = cJSON_GetObjectItem(root, "desired");
        if (desired && desired->type == cJSON_Object) {
          cJSON* ip = cJSON_GetObjectItem(desired, "ip");
          if (ip != NULL && ip->type == cJSON_String) {
            Get_NewIP(ip);
          }
          port = cJSON_GetObjectItem(desired, "port");
          if (port != NULL && port->type == cJSON_Number) {
            DBG_LOG("Set port ...");
            machineDef.netcon.tcp_port = port->valueint;
            DBG_LOG("port is %d", machineDef.netcon.tcp_port);
          }
          heartbeat = cJSON_GetObjectItem(desired, "heartbeat");
          if (heartbeat != NULL && heartbeat->type == cJSON_String) {
            DBG_LOG("Set heartbeat...");
            machineDef.netcon.ischange = 1;
            disconnect(machineDef.netcon.tcp_socket);
            Json_Pack((char*)CmdRecBuf, "CMD-99", GENEAL);
          }
        }
      } else if (strstr(cmd->valuestring, "CMD-03")) {
        cJSON* desired = cJSON_GetObjectItem(root, "desired");
        if (desired && desired->type == cJSON_Object) {
          cJSON* cargoway = cJSON_GetObjectItem(desired, "cargoway");
          if (cargoway != NULL && cargoway->type == cJSON_Number) {
            Get_Mote_Data(cargoway->valueint);
          } else
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
      } else if (strstr(cmd->valuestring, "CMD-04")) {
        cJSON* desired = cJSON_GetObjectItem(root, "desired");
        if (desired && desired->type == cJSON_Object) {
          cJSON* inquiry  = cJSON_GetObjectItem(desired, "inquiry ");
          if (inquiry  != NULL && inquiry->type == cJSON_String) {
            DBG_LOG("inquiry =  %s", inquiry->string);
            Json_Pack((char*)CmdRecBuf, "CMD-99", PARAMETE_REPORT);
          }
        }
      }
    }
    cJSON_Delete(root);
  } else {
    printf("Error before: [%s]\n", cJSON_GetErrorPtr());
  }
}


/* Private functions ---------------------------------------------------------*/


