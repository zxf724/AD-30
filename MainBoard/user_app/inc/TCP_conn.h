
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TCP_CONN_H
#define _TCP_CONN_H


/* Includes ------------------------------------------------------------------*/
#include "prjlib.h"

/* Exported define -----------------------------------------------------------*/
/*MQTT调试使能*/
#define TCP_DEBUG                      1


/*MQTT数据发送的超时时间，单位毫秒*/
#define PING_TIMEOUT_DEF               10

#define PING_INVT_DEF                  30

/*连接失败超时时间,单位秒*/
#define CONNECT_FAIL_TIMEOUT           10

#define TCP_TASK_STK_SIZE              384
#define TCP_TASK_PRIO                  osPriorityNormal

#define TCP_RX_BUFF_SIZE               1024

/* Exported types ------------------------------------------------------------*/

/*数据接收回调函数类型*/
typedef void (*Arrived_t)(uint8_t *data, uint16_t len);

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void TCP_Conn_Init(void);

BOOL TCP_IsDataFlow(void);
BOOL TCP_IsConnected(void);

int16_t TCP_SendData(uint8_t *dat, uint16_t len);

void TCP_SetArrived(Arrived_t fun);


#endif
