/* Includes ------------------------------------------------------------------*/
#include "user_comm.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define mqttPar     (WorkParam.mqtt)

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t tsPingOut = 0, tsDataFlow;

static uint8_t DataFlowCnt = 0;

static BOOL isconnect = FALSE, waitack = FALSE;

static uint8_t rxbuf[TCP_RX_BUFF_SIZE];

static Arrived_t recFun = NULL;

/* Private function prototypes -----------------------------------------------*/
void TCP_Conn_Task(void const* argument);
static void Manager_TCP(void);
static void Connect_TCP(void);
static void SendHeartBeat(void);
static BOOL isACK(char* msg);
static void tcp_Console(int argc, char* argv[]);

/* Exported functions --------------------------------------------------------*/

/**
 * 协议处理初始化
 */
void TCP_Conn_Init(void) {

  osThreadDef(tcp_conn, TCP_Conn_Task, TCP_TASK_PRIO, 0, TCP_TASK_STK_SIZE);
  osThreadCreate(osThread(tcp_conn), NULL);

  CMD_ENT_DEF(tcp, tcp_Console);
  Cmd_AddEntrance(CMD_ENT(tcp));

  DBG_LOG("Protocl init OK.");
}

/**
 * 协议处理的任务
 * @param argument
 */
void TCP_Conn_Task(void const* argument) {
  TWDT_DEF(mqttConTask, 60000);
  TWDT_ADD(mqttConTask);
  TWDT_CLEAR(mqttConTask);


  DBG_LOG("TCP task start.");
  while (1) {
    TWDT_CLEAR(mqttConTask);

    Manager_TCP();
    osDelay(20);
  }
}

/**
 * TCP数据传输状态查询
 */
BOOL TCP_IsDataFlow(void) {
  return (BOOL)DataFlowCnt;
}

/**
 * 返回TCP连接状态
 *
 * @return 返回TCP连接状态
 */
BOOL TCP_IsConnected(void) {
  return isconnect;
}

/**
 * TCP发送数据
 *
 * @param dat    待发送的数据内容
 * @param len    数据长度
 *
 * @return 返回发送结果
 */
int16_t TCP_SendData(uint8_t* dat, uint16_t len) {
  int16_t rc = -1;

  if (System_SockIsConnected(NULL, NULL) > 0 && System_SockIsLock() == FALSE) {
    TS_INIT(tsPingOut);

    /*用于网络指示灯*/
    DataFlowCnt += 1;
    rc = System_SockSend(dat, len);
#if TCP_DEBUG > 1
    if (rc != 0) {
      DBG_LOG("TCP send data, rc:%d", rc);
    }
#if TCP_DEBUG > 2
    CMD_HEX_Print(dat, len);
#endif
#endif
  }
  return rc;
}

/**
 * 设置消息接收回调函数
 *
 * @param fun    函数指针
 */
void TCP_SetArrived(Arrived_t fun) {
  recFun = fun;
}


/* Private function prototypes -----------------------------------------------*/

/**
 * MQTT连接管理
 */
static void Manager_TCP(void) {
  int rc = 0;
  static int8_t pathS = 0, flowcnt = 0;

  char* addr = NULL;
  int8_t path = 0;
  uint16_t port = 0;

  /*网络传输计数，用于网络指示灯*/
  if (DataFlowCnt > 0 && flowcnt == 0) {
    flowcnt = 1;
    TS_INIT(tsDataFlow);
  }
  if (TS_IS_OVER(tsDataFlow, 100) && DataFlowCnt > 0) {
    DataFlowCnt = 0;
    flowcnt = 0;
  }

  /*网络类型变化或者掉线时重新登陆*/
  path = System_SockIsConnected(&addr, &port);
  if (pathS != path || addr != mqttPar.MQTT_Server || port != mqttPar.MQTT_Port) {
    pathS = path;
    // 重新登陆
    isconnect = FALSE;
  }

  /*socket未占用时连接到服务器*/
  if (System_SockIsLock() == FALSE) {
    isconnect = FALSE;
    System_SetSocket(mqttPar.MQTT_Server, mqttPar.MQTT_Port);
  } else {
    // 设备登陆
    if (isconnect == FALSE && waitack == FALSE) {
      waitack = TRUE;
      DBG_LOG("System socket path:%d, server:%s, port:%u.",
              path, mqttPar.MQTT_Server, mqttPar.MQTT_Port);
      Connect_TCP();
    } else {
      // 发送心跳
      if (TS_IS_OVER(tsPingOut, PING_INVT_DEF)) {
        TS_INIT(tsPingOut);
        waitack = TRUE;
        SendHeartBeat();
      }
      // 心跳超时重新登陆
      if (waitack && TS_IS_OVER(tsPingOut, PING_TIMEOUT_DEF)) {
        TS_INIT(tsPingOut);
        isconnect = FALSE;
        waitack = FALSE;
        DBG_LOG("heartbeat timeout");
      }
    }
    // 读接收消息
    rc = System_SockRecv(rxbuf, TCP_RX_BUFF_SIZE - 1);
    if (rc > 0) {
#if TCP_DEBUG > 1
      if (rc != 0) {
        DBG_LOG("TCP read data, rc:%d", rc);
      }
#if TCP_DEBUG > 2
      CMD_HEX_Print(rxbuf, rc);
#endif
#endif

      rxbuf[rc] = 0;
      // 接收消息回调
      if (recFun != NULL) {
        recFun(rxbuf, rc);
      }
      // 处理登陆及心跳应答
      if (waitack != FALSE && isACK((char*)rxbuf)) {
        waitack = FALSE;
      }
    }
  }
}


/**
 * TCP登陆
 *
 */
static void Connect_TCP(void) {
  cJSON* desired = NULL;

  DBG_LOG("Do TCP connect.");

  desired = cJSON_CreateObject();
  if (desired != NULL) {
    cJSON_AddNumberToObject(desired, "heartbeat", 1);
    CMD_Updata("CMD-100", desired);
  }
}

/**
 * 发送心跳包
 */
static void SendHeartBeat(void) {
  cJSON* desired = NULL;

  DBG_LOG("send heartbeat.");

  desired = cJSON_CreateObject();
  if (desired != NULL) {
    cJSON_AddNumberToObject(desired, "heartbeat", 1);
    CMD_Updata("CMD-100", desired);
  }
}

/**
 * 判断是否为有效的登陆及心跳应答
 *
 * @param msg    消息内容
 *
 * @return 收到应答返回TRUE
 */
static BOOL isACK(char* msg) {
  BOOL ret = FALSE;

  cJSON* root = NULL, *msgid = NULL, *cmd = NULL, *desired = NULL, *deviceid = NULL;

  root = cJSON_Parse(msg);
  if (root != NULL) {
    msgid = cJSON_GetObjectItem(root, "messageid");
    deviceid = cJSON_GetObjectItem(root, "deviceid");

    // 判断设备ID是否一致
    if (msgid != NULL && msgid->type == cJSON_String
        && deviceid != NULL && (strcmp(deviceid->valuestring, WorkParam.mqtt.MQTT_ClientID) == 0)) {
      desired = cJSON_GetObjectItem(root, "desired");
      cmd = cJSON_GetObjectItem(root, "cmd");
      if (cmd != NULL && desired != NULL && (strcmp(cmd->valuestring, "CMD-99") == 0)) {
        ret = TRUE;
      }
    }
    cJSON_Delete(root);
  }
  return ret;
}

/**
 * 调试命令
 * @param argc 参数项数量
 * @param argv 参数列表
 */
static void tcp_Console(int argc, char* argv[]) {
  argv++;
  argc--;


}

