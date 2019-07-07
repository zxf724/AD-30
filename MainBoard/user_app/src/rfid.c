/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date         Author         Notes
 * 2019.7.5     Yang Song      first version.
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
/* External functions --------------------------------------------------------*/

/**
 * 读RFID
 *
 * @param id     返回的卡号
 *
 * @return 读到有卡返回TRUE
 */
BOOL ReadRFID(uint32_t* id) {
  uint8_t * pbuf = NULL,*p;
  uint16_t len = 0;
  BOOL ret = FALSE;

  len = UART_DataSize(RFID_UART_PORT);
  if (len > 9) {
    pbuf = MMEMORY_ALLOC(len);
    if (pbuf != NULL) {
      UART_ReadData(RFID_UART_PORT, pbuf, len);
      p = pbuf;

      while ((*p != 0x7E || *(p + 1) != 0x1B) && len >= 9) {
        p++;
        len--;
      }
      // 读到卡
      if (len >= 9 && *(p + 3) == 0x12) {
        *id = *((uint32_t*)p + 4);
        ret = TRUE;
        DBG_LOG("read RFID:%#x", *id);
      }

      MMEMORY_FREE(pbuf);
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/


