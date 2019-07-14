/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date         Author         Notes
 * 2019.2.8     Yang Song      first version.
 */

/* Includes ------------------------------------------------------------------*/
#include "user_comm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void ArrivePath(uint8_t* dat, uint16_t len);
static BOOL CMD_Confirm_Rsp(char* ordermsgid);

/* External functions --------------------------------------------------------*/

/**
 * 协议处理初始化
 */
void ProtocolInit(void) {

  TCP_SetArrived(ArrivePath);

  DBG_LOG("protocol init.");
}

/**
 * 上传命令
 *
 * @param cmd     命令字
 * @param desired 消息内容
 *
 * @return 发送成功返回TRUE
 */
BOOL CMD_Updata(char* cmd, cJSON* desired) {
  BOOL ret = FALSE;
  char* s = NULL;

  cJSON* root = NULL;

  root = cJSON_CreateObject();
  if (root != NULL) {
    cJSON_AddNumberToObject(root, "messageid", HAL_GetTick());
    cJSON_AddNumberToObject(root, "timestamp", RTC_ReadTick());
    cJSON_AddStringToObject(root, "cmd", cmd);
    cJSON_AddStringToObject(root, "deviceid", WorkParam.mqtt.MQTT_ClientID);
    cJSON_AddItemToObjectCS(root, "desired", desired);

    s = cJSON_PrintUnformatted(root);
    if (s != NULL) {
      DBG_LOG("CMD_Updata ts:%u,data:%s", HAL_GetTick(), s);

      if (TCP_SendData((uint8_t*)s, strlen(s))) {
        ret = TRUE;
      }
      MMEMORY_FREE(s);
    }
    cJSON_Delete(root);
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/

/**
 * 消息处理
 *
 * @param dat
 * @param len
 */
void ArrivePath(uint8_t* dat, uint16_t len) {
  char recmsgid[32] = { 0 };
  cJSON* root = NULL, *timestamp = NULL, *msgid = NULL, *cmd = NULL, *desired = NULL, *deviceid = NULL;

  *(dat + len) = 0;
  DBG_INFO("ArrivePath ts:%u,data:%s", HAL_GetTick(), dat);

  root = cJSON_Parse((const char*)dat);
  if (root != NULL) {
    msgid = cJSON_GetObjectItem(root, "messageid");

    // 判断设备ID一致
    deviceid = cJSON_GetObjectItem(root, "deviceid");
    if (msgid != NULL && msgid->type == cJSON_String &&
        deviceid != NULL && (strcmp(deviceid->valuestring, WorkParam.mqtt.MQTT_ClientID) == 0)) {
      strcpy(recmsgid, msgid->valuestring);
      timestamp = cJSON_GetObjectItem(root, "timestamp");
      desired = cJSON_GetObjectItem(root, "desired");
      cmd = cJSON_GetObjectItem(root, "cmd");

      /*RTC校时*/
      if (timestamp != NULL && timestamp->type == cJSON_String) {
        timeRTC_t time;

        RTC_TickToTime(uatoi(timestamp->valuestring), &time);
        RTC_SetTime(&time);
      }
      // 消息处理
      if (desired != NULL && cmd != NULL) {
        // 系统复位
        if (strcmp(cmd->valuestring, "CMD-01") == 0) {
          NVIC_SystemReset();
        }
        // TODO 添加剩余的消息处理
      }
    }
    cJSON_Delete(root);


  }
  // 消息应答
  if (recmsgid[0] != '0') {
    CMD_Confirm_Rsp(recmsgid);
  }
}


/**
 * 上传数据
 *
 * @param cmd        上传的命令
 * @param ordermsgid 上行的消息ID
 * @return 返回上传结果
 */
static BOOL CMD_Confirm_Rsp(char* ordermsgid) {
  BOOL r = FALSE;

  cJSON* bodydesired;

  bodydesired = cJSON_CreateObject();
  if (bodydesired != NULL) {
    cJSON_AddStringToObject(bodydesired, "messageid", ordermsgid);
    cJSON_AddStringToObject(bodydesired, "ret", "OK");

    r = CMD_Updata("CMD-99", bodydesired);
  }
  return r;
}

