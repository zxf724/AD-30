/**
 * Copyright (c) 2017, Afuiot Tech, Co., Ltd
 * All rights reserverd.
 * File name: Process.c
 * Description: Protocol processing source file.
 * Other: none
 * Version: V1.0
 * History: Version, Writer, modifier, modify time, modify content
             V1.0     wucb     wucb    2018.11.15   first version
 */

/* Includes ------------------------------------------------------------------*/
#include "Process.h"
#include "MQTTClient.h"
#include "usart.h"
#include "mqtt_conn.h"
#include <stdlib.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
char subscribeTopic[36] = { 0 };
char publishTopic[36] = { 0 };
extern char Connect_deviceID[32];
/* Private function prototypes -----------------------------------------------*/
static uint8_t ArriveDataProc(char* cmd, cJSON* desired);
static void process_Console(int argc, char* argv[]);

/* Exported functions --------------------------------------------------------*/
/**
 * Function: Process_Init
 * Description: Initialize protocol processing.
 * Parameter: None
 * Return: None
 * Others: Add a command entry for uart control.
 */
void Process_Init(void) {
	CMD_ENT_DEF(process, process_Console);
  Cmd_AddEntrance(CMD_ENT(process));
  strcpy(publishTopic, "/AD-26");
  strcpy(subscribeTopic, "/AD-26/");
  strcat(subscribeTopic, "no imei");

  Subscribe_MQTT(subscribeTopic, QOS0, ArrivePath);

  DBG_LOG("Process Start.");
}
/**
 * Function: CMD_Updata
 * Description: General response.
 * Parameter: ack_cmd - the cmd that you need to response.
              ret - results of execution.
 * Return: None
 * Others: None
 */
BOOL CMD_Updata(char* cmd, cJSON* desired) {
  BOOL ret = FALSE;
  char* s = NULL;
  uint16_t s_len = 0;
  char msgidBuf[20];
  cJSON* root = NULL;

  static uint32_t msgid = 0;

  root = cJSON_CreateObject();
  if (root != NULL) {
    /* Bulid the message frame. */
    uitoa(msgid++, msgidBuf);
    cJSON_AddStringToObject(root, "messageId", msgidBuf);
   // cJSON_AddNumberToObject(root, "timestamp", RTC_ReadCount());
    cJSON_AddStringToObject(root, "deviceId", Connect_deviceID);
    cJSON_AddStringToObject(root, "cmd", cmd);
    cJSON_AddItemToObjectCS(root, "desired", desired);

    s = cJSON_PrintBuffered(root, 256, 0);
    if (s != NULL) {
      s_len = strlen(s);
      /* Save the position into buffer. */
      ret = Publish_MQTT(publishTopic, QOS0, (uint8_t*)s, s_len);
      free(s);
    } else {
      DBG_LOG("malloc string failed.");
    }
    cJSON_Delete(root);
  } else {
    DBG_LOG("malloc root failed.");
  }
  return ret;
}

/**
 * Function: GeneralResponse
 * Description: General response.
 * Parameter: ack_cmd - the cmd that you need to response.
              ret - results of execution.
 * Return: None
 * Others: None
 */
void GeneralResponse(char* ack_cmd, char* ret, char* msgid) {
  cJSON* desired = NULL;

  desired = cJSON_CreateObject();

  if (desired != NULL) {
    cJSON_AddStringToObject(desired, "cmd", ack_cmd);
    cJSON_AddStringToObject(desired, "ret", ret);
    cJSON_AddStringToObject(desired, "messageId", msgid);

    CMD_Updata("CMD-99", desired);
  }
}

/**
 * Function: ArrivePath
 * Description: Analysis of received data
 * Parameter: dat - received data buffer.
              len - data len .
 * Return: None
 * Others: None
 */
void ArrivePath(uint8_t* dat, uint16_t len) {
  uint8_t ret = 0;
  static char msgIDold[32] = { 0 }, *retstr;

  cJSON* root = NULL;
  cJSON* messageId = NULL;
  cJSON* deviceId = NULL;
  cJSON* cmd = NULL;
  cJSON* desired = NULL;

  *(dat + len) = 0;
  root = cJSON_Parse((const char*)dat);

  if (root != NULL) {
    messageId = cJSON_GetObjectItem(root, "messageId");
    deviceId = cJSON_GetObjectItem(root, "deviceId");
    desired = cJSON_GetObjectItem(root, "desired");
    cmd = cJSON_GetObjectItem(root, "cmd");

    if (messageId != NULL && messageId->type == cJSON_String
        && strcmp(msgIDold, messageId->valuestring) != 0) {
      if (deviceId != NULL && deviceId->type == cJSON_String
          && (strcmp(Connect_deviceID, deviceId->valuestring) == 0)) {
        ret = ArriveDataProc(cmd->valuestring, desired);
      } else {
        ret = 2;
      }
      strcpy(msgIDold, messageId->valuestring);
    } else {
      ret = 3;
    }
    if (ret == 0) {
      retstr = "Unknow Command";
    } else if (ret == 1) {
      retstr = "OK";
    } else if (ret == 2) {
      retstr = "Invaild deviceid";
    } else if (ret == 3) {
      retstr = "Invaild messageid";
    }
    GeneralResponse(cmd->valuestring, retstr,  messageId->valuestring);
    cJSON_Delete(root);
  }
}

/**
 * Function: ArriveDataProc
 * Description: Process the message that are received.
 * Parameter: cmd - The command from server.
              desired - message.
 * Return: None
 * Others: None
 */
static uint8_t ArriveDataProc(char* cmd, cJSON* desired) {
  uint8_t ret = 0;
  cJSON* mac = NULL, *name = NULL, *time = NULL;

  /* Login response */
  if (STR_EQUAL(cmd, "CMD-02")) {
    time = cJSON_GetObjectItem(desired, "chargeTime");
    name = cJSON_GetObjectItem(desired, "blename");
    mac = cJSON_GetObjectItem(desired, "macAddress");
    if (time != NULL && time->type == cJSON_Number && (name != NULL || mac != NULL)) {
      
      ret = 1;
    }
  }
  return ret;
}

/**
 * Function: process_Console
 * Description: Analysis the command from uart.
 * Parameter: argc - Number of parameters.
              argv - All parameters.
 * Return: None
 * Others: None
 */
static void process_Console(int argc, char* argv[]) {
  cJSON* desired = NULL;
  argv++;
  argc--;

  if (ARGV_EQUAL("receive")) {
    ArrivePath((uint8_t*)argv[1], strlen(argv[1]));
  } else if (ARGV_EQUAL("send")) {
    desired = cJSON_Parse((const char*)argv[2]);
    if (desired != NULL) {
      CMD_Updata(argv[1], desired);
      cJSON_Delete(desired);
    }
  }
}
