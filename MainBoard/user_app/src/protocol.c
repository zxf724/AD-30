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
static void ArrivePath(uint8_t *dat, uint16_t len);
/* External functions --------------------------------------------------------*/

static void protocol_Console(int argc, char *argv[]);
/**
 * 协议处理初始化
 */
void ProtocolInit(void) {
  TCP_SetArrived(ArrivePath);
  CMD_ENT_DEF(protocol, protocol_Console);
  Cmd_AddEntrance(CMD_ENT(protocol));

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
BOOL CMD_Updata(char *cmd, cJSON *desired) {
  BOOL ret = FALSE;
  char *s = NULL;
  static char temp[300];

  cJSON *root = NULL;

  root = cJSON_CreateObject();
  if (root != NULL) {
    cJSON_AddNumberToObject(root, "messageid", HAL_GetTick());
    cJSON_AddNumberToObject(root, "timestamp", RTC_ReadTick());
    cJSON_AddStringToObject(root, "cmd", cmd);
    cJSON_AddStringToObject(root, "deviceid", WorkParam.mqtt.MQTT_ClientID);
    cJSON_AddItemToObjectCS(root, "desired", desired);

    s = cJSON_PrintUnformatted(root);
    if (s != NULL) {
      strcpy(temp, s);
      strcat(temp, "@");
      MMEMORY_FREE(s);
      DBG_LOG("CMD_Updata ts:%u,data:%s", HAL_GetTick(), temp);

      if (TCP_SendData((uint8_t *)temp, strlen(temp))) {
        ret = TRUE;
      }
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
void ArrivePath(uint8_t *dat, uint16_t len) {
  char recmsgid[32] = {0};
  cJSON *root = NULL, *timestamp = NULL, *msgid = NULL, *cmd = NULL,
        *desired = NULL, *deviceid = NULL;

  *(dat + len) = 0;
  DBG_INFO("ArrivePath ts:%u,data:%s", HAL_GetTick(), dat);

  root = cJSON_Parse((const char *)dat);
  if (root != NULL) {
    msgid = cJSON_GetObjectItem(root, "messageid");
    // 判断设备ID一致
    deviceid = cJSON_GetObjectItem(root, "deviceid");
    // test
    if (msgid != NULL) {
      DBG_LOG("msgid != NULL");
    }
    if (msgid->type == cJSON_String) {
      DBG_LOG("msgid->type == cJSON_String");
    }
    if (deviceid != NULL) {
      DBG_LOG("deviceid != NUL");
    }
    if (strcmp(deviceid->valuestring, WorkParam.mqtt.MQTT_ClientID) == 0) {
      DBG_LOG(
          "strcmp(deviceid->valuestring, WorkParam.mqtt.MQTT_ClientID) == 0");
    }

    if (msgid != NULL && msgid->type == cJSON_String && deviceid != NULL) {
      DBG_LOG("data is correct!");
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
        } else if (strcmp(cmd->valuestring, "CMD-02") == 0) {
          cJSON *ip = cJSON_GetObjectItem(desired, "ip");
          if (ip != NULL && ip->type == cJSON_String) {
            Get_NewIP(ip);  // get ip
            WorkParam_Save();
          }
          cJSON *port = cJSON_GetObjectItem(desired, "port");
          if (port != NULL && port->type == cJSON_Number) {
            DBG_LOG("Set port ...");
            WorkParam.mqtt.MQTT_Port = port->valueint;
            WorkParam_Save();
          }
          cJSON *heartbeat = cJSON_GetObjectItem(desired, "heartbeat");
          if (heartbeat != NULL && heartbeat->type == cJSON_String) {
            DBG_LOG("Set heartbeat...");
            WorkParam.mqtt.MQTT_PingInvt = heartbeat->valueint;
            CMD_Confirm_Rsp(recmsgid);  // send back
          }
        } else if (strcmp(cmd->valuestring, "CMD-03") == 0)  // select!
        {
          DBG_LOG("select!");

          cJSON *cargoway = cJSON_GetObjectItem(desired, "cargoway");
          if (cargoway != NULL && cargoway->type == cJSON_Number) {
            Get_Mote_Data(cargoway->valueint);
          } else {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
          }
        } else if (strcmp(cmd->valuestring, "CMD-04") ==
                   0)  // return all the parameter
        {
          DBG_LOG("return_all_parameter!");
          return_all_parameter();
        } else if (strcmp(cmd->valuestring, "CMD-99") == 0) {
          DBG_LOG("receive CMD-99");
        }
      }
      cJSON_Delete(root);
    }
  }
}

/**
 * 上传数据
 *
 * @param cmd        上传的命令
 * @param ordermsgid 上行的消息ID
 * @return 返回上传结果
 */
static BOOL CMD_Confirm_Rsp(char *ordermsgid) {
  BOOL r = FALSE;
  cJSON *bodydesired;
  bodydesired = cJSON_CreateObject();
  if (bodydesired != NULL) {
    cJSON_AddStringToObject(bodydesired, "messageid", ordermsgid);
    cJSON_AddStringToObject(bodydesired, "ret", "OK");

    r = CMD_Updata("CMD-99", bodydesired);
  }
  return r;
}

void Get_NewIP(cJSON *ip) {
  uint8_t serve_ip[4];
  DBG_LOG("Get  ip...");
  serve_ip[0] = uatoi(ip->valuestring);
  serve_ip[1] = uatoi(strstr(ip->valuestring, ".") + 1);
  serve_ip[2] = uatoi(strstr(strstr(ip->valuestring, ".") + 1, ".") + 1);
  serve_ip[3] = uatoi(
      strstr((strstr(strstr(ip->valuestring, ".") + 1, ".") + 1), ".") + 1);
  strcpy((char *)WorkParam.mqtt.MQTT_Server, serve_ip);
  DBG_LOG("ip is %d,%d,%d,%d", serve_ip[0], serve_ip[1], serve_ip[2],
          serve_ip[3]);
}

/**
 * 货道货物不足 finish_process
 * @param argc 参数项数量
 * @param argv 参数列表
 */
void has_no_goods(void) {
  cJSON *desired = NULL;

  DBG_LOG("has no goods!");

  desired = cJSON_CreateObject();
  if (desired != NULL) {
    cJSON_AddNumberToObject(desired, "lacknum", 5);  // 相应货道号
    if (CMD_Updata("CMD-105", desired)) {
      DBG_LOG("has_no_goods send data success!");
    } else {
      DBG_LOG("has_no_goods send data error!");
    }
  }
}

void Get_Mote_Data(uint8_t data) {
  uint8_t i = 0;
  uint8_t good_check = 0;
  DBG_LOG("get moto data!!");
  DBG_LOG("data is %d", data);
  if (i > 0 && i <= 16) {
    GoodsPickup(data);
    GoodsShow();
    good_check = GoodsCheck(data);
    DBG_LOG("rooad%d check result:d", data, good_check);
    if (good_check == 0) {
      DBG_LOG("%d road has no goods!", data);
      has_no_goods();
    }
  }
}

/**
 * •	查询设备参数时的应答 finish_process
 * @param argc 参数项数量
 * @param argv 参数列表
 */
void return_all_parameter(void) {
  cJSON *desired = NULL;

  DBG_LOG("return all parameter!!");

  desired = cJSON_CreateObject();
  if (desired != NULL) {
    cJSON_AddStringToObject(desired, "ip",
                            MQTT_SERVER_DEF);  // 连接的服务器IP地址或域名
    cJSON_AddNumberToObject(desired, "port",
                            MQTT_PORT_DEF);  // 连接的服务器端口
    cJSON_AddNumberToObject(desired, "heartbeat",
                            PING_INVT_DEF);  // 心跳间隔,单位秒
    cJSON_AddStringToObject(desired, "project", PROJECT);   // 项目名称
    cJSON_AddStringToObject(desired, "firmware", VERSION);  // 固件版本
    cJSON_AddStringToObject(desired, "hardware", VERSION_HARDWARE);  // 硬件版本
    cJSON_AddStringToObject(desired, "status", "ok");  // 工作与故障状态
    if (CMD_Updata("CMD-104", desired)) {
      DBG_LOG("has_no_goods send data success!");
    } else {
      DBG_LOG("has_no_goods send data error!");
    }
  }
}

/**
 * 调试函数
 * @param argc 参数项数量
 * @param argv 参数列表
 */
static void protocol_Console(int argc, char *argv[]) {
  argc--;
  argv++;
  if (ARGV_EQUAL("SEND")) {
    DBG_LOG("send successed!");
    ArrivePath((uint8_t *)argv[1], strlen(argv[1]));
  } else if (ARGV_EQUAL("test")) {
    DBG_LOG("hello,world!");
  }
}
