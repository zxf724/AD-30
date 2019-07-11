/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date           Author         Notes
 * 2019.6.26      Yang Song      first version.
 */

/* Includes ------------------------------------------------------------------*/
#include "user_comm.h"
/* Private typedef -----------------------------------------------------------*/
// 定义步进电机运动状态
typedef enum
{
  motor_start,
  motor_start_fast,
  motor_fast,
  motor_slowdown,
  motor_slow,
  motor_stop
} MotorStatusEnum;

/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern IWDG_HandleTypeDef hiwdg;

// X Z轴的位置
static uint32_t gs_x_pos = 0, gs_z_pos = 0;

// 定义各货道
static uint32_t gs_road_pos[16][2] = {
  { 2055, 61790 }, { 1380, 61790 }, { 705, 61790 }, { 30, 61790 },
  { 2055, 45960 }, { 1380, 45960 }, { 705, 45960 }, { 30, 45960 },
  { 2055, 30130 }, { 1380, 30130 }, { 705, 30130 }, { 30, 30130 },
  { 2055, 14300 }, { 1380, 14300 }, { 705, 14300 }, { 30, 14300 },
};

static uint32_t gs_backup_pos[2] = { 30, 0 };
static uint32_t gs_show_pos[2] = { 1042, 30000 };

/* Private function prototypes -----------------------------------------------*/
static void MotorDelayUs(uint32_t us);
static void motor_Console(int argc, char* argv[]);

/* External functions --------------------------------------------------------*/

/**
 * 电机控制初始化
 */
void MotorControlInit(void) {
  CMD_ENT_DEF(motor, motor_Console);
  Cmd_AddEntrance(CMD_ENT(motor));

  DBG_LOG("Motor control init OK.");
}


/**
 * 步进电机复位，用于货架归位
 */
void MotorSetpeerReset(void) {
  uint16_t plusX = MOTOR_X_START_PLUS;
  uint16_t plusZ = MOTOR_Z_START_PLUS;
  uint16_t ipX = 0, ipZ = 0;
  uint32_t iz = 0, tsReset;

  BOOL zrst = FALSE, xrst = FALSE;

  MOTOR_X_RIGHT();
  MOTOR_Z_DOWN();

  IO_H(PUL2);
  IO_H(PUL1);

  TS_INIT(tsReset);
  while ((!TS_IS_OVER(tsReset, MOTOR_RESET_TIME_MAX))
         && (xrst == FALSE  || zrst == FALSE)) {
    MotorDelayUs(1);
    HAL_IWDG_Refresh(&hiwdg);

    // X轴输出脉冲
    if (ipX++ >= plusX && xrst == FALSE) {
      ipX = 0;
      if (!IS_IR_DECT(22)) {
        MOTOR_X_TOGGLE();
      } else {
        IO_H(PUL2);
        xrst = TRUE;
        DBG_LOG("X reset OK.");
      }
    }

    // Z轴输出脉冲
    if (ipZ++ >= plusZ && zrst == FALSE) {
      ipZ = 0;
      iz++;

      if (!IS_IR_DECT(19)) {
        MOTOR_Z_TOGGLE();
      } else {
        IO_H(PUL1);
        zrst = TRUE;
        DBG_LOG("Z reset OK.");
      }
    }

    // Z轴速度控制
    if ((iz > 200) && (iz % 10 == 0)) {
      if (plusZ > (MOTOR_Z_FAST_PLUS * 3)) {
        plusZ--;
      }
    }
  }

  IO_H(PUL2);
  IO_H(PUL1);

  gs_x_pos = 0;
  gs_z_pos = 0;
}

/**
 * 步进电机运行至指定坐标
 *
 * @param x      X坐标
 * @param z      Z坐标
 */
void MotorMoveTo(uint32_t x, uint32_t z) {
  int xsign = 1, zsign = 1;

  // 计算新坐标与原坐标的差值，设置电机的方向
  if (x > gs_x_pos) {
    x = x - gs_x_pos;
    xsign = 1;
    MOTOR_X_LEFT();
  } else {
    x = gs_x_pos - x;
    xsign = -1;
    MOTOR_X_RIGHT();
  }

  if (z > gs_z_pos) {
    z = z - gs_z_pos;
    zsign = 1;
    MOTOR_Z_UP();
  } else {
    z = gs_z_pos - z;
    zsign = -1;
    MOTOR_Z_DOWN();
  }
  // 电机动作
  MotorSetpperMove(x, z);

  // 记录电机新坐标
  if (xsign == 1) {
    gs_x_pos += x;
  } else {
    gs_x_pos -= x;
  }

  if (zsign == 1) {
    gs_z_pos += z;
  } else {
    gs_z_pos -= z;
  }
}

/**
 * 步进电机移动
 *
 * @param xstep  X轴移动的步数
 * @param zstep  Z轴移动的步数
 */
void MotorSetpperMove(uint32_t xstep, uint32_t zstep) {
  uint32_t tsX, tsZ;
  uint32_t iX = 0, iZ = 0, iX_slow = xstep, iZ_slow = zstep;
  uint16_t plusX = MOTOR_X_START_PLUS;
  uint16_t plusZ = MOTOR_Z_START_PLUS;
  uint16_t ipX = 0, ipZ = 0;
  MotorStatusEnum statusZ = motor_start;
  MotorStatusEnum statusX = motor_start;

  xstep *= 2;
  zstep *= 2;

  if (zstep > MOTOR_Z_SPEED_SLOWDOWN_COUNT) {
    iZ_slow = zstep - MOTOR_Z_SPEED_SLOWDOWN_COUNT;
  }
  if (xstep > MOTOR_X_SPEED_SLOWDOWN_COUNT) {
    iX_slow = xstep - MOTOR_X_SPEED_SLOWDOWN_COUNT;
  }

  IO_H(PUL2);
  IO_H(PUL1);
  TS_INIT(tsX);
  TS_INIT(tsZ);
  while (iX < xstep || iZ < zstep) {
    MotorDelayUs(1);
    HAL_IWDG_Refresh(&hiwdg);

    // X轴输出脉冲
    if (ipX++ >= plusX && iX < xstep) {
      ipX = 0;
      iX++;
      MOTOR_X_TOGGLE();

      // X轴速度控制,确保脉冲完整
      if (iX % 2 == 0) {
        switch (statusX) {
          case motor_start:
            plusX = MOTOR_X_START_PLUS;
            // 80ms后开始加速
            if (TS_IS_OVER(tsX, 80)) {
              TS_INIT(tsX);
              statusX = motor_start_fast;
            }
            break;
          case motor_start_fast:
            TS_INIT(tsX);
            plusX -= 2;
            if (plusX <= MOTOR_X_FAST_PLUS) {
              plusX = MOTOR_X_FAST_PLUS;
              statusX = motor_fast;
            }
            break;
          case motor_fast:
            break;
          case motor_slowdown:
            TS_INIT(tsX);
            plusX++;
            if (plusX >= MOTOR_X_START_PLUS) {
              plusX = MOTOR_X_START_PLUS;
              statusX = motor_slow;
            }
            break;
          case motor_slow:
            break;
          case motor_stop:
            break;
          default:
            break;
        }
        // X需要减速
        if (iX > iX_slow && statusX < motor_slowdown) {
          statusX = motor_slowdown;
        }
      }
    }
    // Z轴输出脉冲
    if (ipZ++ >= plusZ && iZ < zstep) {
      ipZ = 0;
      iZ++;
      MOTOR_Z_TOGGLE();

      // Z轴速度控制,确保脉冲完整
      if (iZ % 2 == 0) {
        // Z轴速度控制
        switch (statusZ) {
          case motor_start:
            plusZ = MOTOR_Z_START_PLUS;
            // 100ms后开始加速
            if (TS_IS_OVER(tsZ, 100)) {
              statusZ = motor_start_fast;
              TS_INIT(tsZ);
            }
            break;
          case motor_start_fast:
            if (TS_IS_OVER(tsZ, MOTOR_Z_SPEED_CHANGE_TIME)) {
              TS_INIT(tsZ);
              plusZ--;
              if (plusZ <= MOTOR_Z_FAST_PLUS) {
                plusZ = MOTOR_Z_FAST_PLUS;
                statusZ = motor_fast;
              }
            }
            break;
          case motor_fast:
            break;
          case motor_slowdown:
            if (TS_IS_OVER(tsZ, MOTOR_Z_SPEED_CHANGE_TIME)) {
              TS_INIT(tsZ);
              plusZ++;
              if (plusZ >= MOTOR_Z_START_PLUS) {
                plusZ = MOTOR_Z_START_PLUS;
                statusZ = motor_slow;
              }
            }
            break;
          case motor_slow:
            break;
          case motor_stop:
            break;
          default:
            break;
        }
        // Z需要减速
        if (iZ > iZ_slow && statusZ < motor_slowdown) {
          statusZ = motor_slowdown;
        }
      }
    }
  }

  IO_H(PUL2);
  IO_H(PUL1);

  // 避免低优先级看门狗复位
  osDelay(2);
}

/**
 * 取货，从指定的货道将货取出到货舱
 *
 * @param index  货道的序号
 */
void GoodsPickup(uint8_t index) {
  uint32_t ts, tspick, ir = 0, rfid = 0;
  BOOL rfidread = FALSE;

  TS_INIT(tspick);
  DBG_LOG("goods pickup action, from road%d", index);

  // 货舱运行至指定货道的坐标
  MotorMoveTo(gs_road_pos[index - 1][0], gs_road_pos[index - 1][1]);

  DBG_LOG("goods pickup XZ move used time:%dms", TS_COUNT(tspick));
  // 启动货道电机
  TS_INIT(ts);
  switch (index) {
    case 1:
      MOTOR_DC_ACTION(16);
      break;
    case 2:
      MOTOR_DC_ACTION(15);
      break;
    case 3:
      MOTOR_DC_ACTION(14);
      break;
    case 4:
      MOTOR_DC_ACTION(13);
      break;
    case 5:
      MOTOR_DC_ACTION(12);
      break;
    case 6:
      MOTOR_DC_ACTION(11);
      break;
    case 7:
      MOTOR_DC_ACTION(10);
      break;
    case 8:
      MOTOR_DC_ACTION(9);
      break;
    case 9:
      MOTOR_DC_ACTION(8);
      break;
    case 10:
      MOTOR_DC_ACTION(7);
      break;
    case 11:
      MOTOR_DC_ACTION(6);
      break;
    case 12:
      MOTOR_DC_ACTION(5);
      break;
    case 13:
      MOTOR_DC_ACTION(4);
      break;
    case 14:
      MOTOR_DC_ACTION(3);
      break;
    case 15:
      MOTOR_DC_ACTION(2);
      break;
    case 16:
      MOTOR_DC_ACTION(1);
      break;
    default:
      break;
  }
  // 启动货舱电机
  CARGO_MOVE_FORWARD();

  // 等待货舱传感器检测到红外
  TS_INIT(ts);
  ir = 0;
  while (ir == 0 && !TS_IS_OVER(ts, DC_MOTOR_MOVE_TIME_MAX + CARGO_MOTOR_MOVE_TIME_MAX)) {
    //货舱电机传感器
    ir = IS_IR_DECT(18);
    osDelay(2);
  }
  // 停止货道电机
  switch (index) {
    case 1:
      MOTOR_DC_STOP(16);
      break;
    case 2:
      MOTOR_DC_STOP(15);
      break;
    case 3:
      MOTOR_DC_STOP(14);
      break;
    case 4:
      MOTOR_DC_STOP(13);
      break;
    case 5:
      MOTOR_DC_STOP(12);
      break;
    case 6:
      MOTOR_DC_STOP(11);
      break;
    case 7:
      MOTOR_DC_STOP(10);
      break;
    case 8:
      MOTOR_DC_STOP(9);
      break;
    case 9:
      MOTOR_DC_STOP(8);
      break;
    case 10:
      MOTOR_DC_STOP(7);
      break;
    case 11:
      MOTOR_DC_STOP(6);
      break;
    case 12:
      MOTOR_DC_STOP(5);
      break;
    case 13:
      MOTOR_DC_STOP(4);
      break;
    case 14:
      MOTOR_DC_STOP(3);
      break;
    case 15:
      MOTOR_DC_STOP(2);
      break;
    case 16:
      MOTOR_DC_STOP(1);
      break;
    default:
      break;
  }
  // 等待读出RFID
  TS_INIT(ts);
  rfidread = FALSE;
  while (rfidread == FALSE && !TS_IS_OVER(ts, CARGO_MOTOR_MOVE_TIME_MAX)) {
    osDelay(2);
    rfidread = ReadRFID(&rfid);
  }
  // 货舱电机停止
  CARGO_MOVE_STOP();

  DBG_LOG("goods pickup used time:%dms", TS_COUNT(tspick));
}

/**
 * 货物展示，从取货的货通道运行至贩卖机中间位置，打开盒子展示完毕后回收货物
 */
void GoodsShow(void) {
  uint32_t ts, tspick, ir = 0;

  TS_INIT(tspick);
  DBG_LOG("goods show action");

  // 货舱运行至展示的坐标
  MotorMoveTo(gs_show_pos[0], gs_show_pos[1]);
  DBG_LOG("goods show XZ move used time:%dms", TS_COUNT(tspick));

  // 盒子开盖
  ir = 0;
  TS_INIT(ts);
  DOOR_MOVE_FORWARD();
  while (ir == 0 && !TS_IS_OVER(ts, DOOR_MOTOR_MOVE_TIME_MAX)) {
    osDelay(2);
    ir = IS_IR_DECT(20);
  }
  DOOR_MOVE_STOP();

  // 展示3秒
  osDelay(3000);

  // 盒子关盖
  ir = 0;
  TS_INIT(ts);
  DOOR_MOVE_REVERSE();
  while (ir == 0 && !TS_IS_OVER(ts, DOOR_MOTOR_MOVE_TIME_MAX)) {
    osDelay(2);
    ir = IS_IR_DECT(21);
  }
  DOOR_MOVE_STOP();

  DBG_LOG("goods show door move used time:%dms", TS_COUNT(tspick));

  // 货舱运行至回流货道的坐标
  MotorMoveTo(gs_backup_pos[0], gs_backup_pos[1]);
  DBG_LOG("goods recover move used time:%dms", TS_COUNT(tspick));

  // 回流货道启动回收货物
  MOTOR_DC_ACTION(17);

  // 货舱电机反转
  CARGO_MOVE_REVERSE();
  ir = 0;
  TS_INIT(ts);
  while (ir == 0 && !TS_IS_OVER(ts, DC_MOTOR_MOVE_TIME_MAX)) {
    osDelay(2);
    ir = IS_IR_DECT(17);
  }
  // 货舱电机停止
  CARGO_MOVE_STOP();

  // 红外检测到后再运行2秒
  if (ir == 1) {
    osDelay(2000);
  }
  MOTOR_DC_STOP(17);

  // 回复至原点
  MotorSetpeerReset();

  DBG_LOG("goods show used time:%dms", TS_COUNT(tspick));
}

/**
 * 检查货道是否有货物货
 *
 * @param index  货道的序号
 *
 * @return 有货物返回TRUE
 */
BOOL GoodsCheck(uint8_t index) {

  uint32_t ts, tspick, ir = 0;
  TS_INIT(tspick);
  DBG_LOG("goods check action, road%d", index);

  // 启动货道电机
  TS_INIT(ts);
  switch (index) {
    case 1:
      MOTOR_DC_ACTION(16);
      break;
    case 2:
      MOTOR_DC_ACTION(15);
      break;
    case 3:
      MOTOR_DC_ACTION(14);
      break;
    case 4:
      MOTOR_DC_ACTION(13);
      break;
    case 5:
      MOTOR_DC_ACTION(12);
      break;
    case 6:
      MOTOR_DC_ACTION(11);
      break;
    case 7:
      MOTOR_DC_ACTION(10);
      break;
    case 8:
      MOTOR_DC_ACTION(9);
      break;
    case 9:
      MOTOR_DC_ACTION(8);
      break;
    case 10:
      MOTOR_DC_ACTION(7);
      break;
    case 11:
      MOTOR_DC_ACTION(6);
      break;
    case 12:
      MOTOR_DC_ACTION(5);
      break;
    case 13:
      MOTOR_DC_ACTION(4);
      break;
    case 14:
      MOTOR_DC_ACTION(3);
      break;
    case 15:
      MOTOR_DC_ACTION(2);
      break;
    case 16:
      MOTOR_DC_ACTION(1);
      break;
    default:
      break;
  }

  // 货道红外检测
  ir = 0;
  TS_INIT(ts);
  while (ir == 0 && !TS_IS_OVER(ts, DC_MOTOR_MOVE_TIME_MAX)) {
    osDelay(2);
    switch (index) {
      case 1:
        ir = IS_IR_DECT(16);
        break;
      case 2:
        ir = IS_IR_DECT(15);
        break;
      case 3:
        ir = IS_IR_DECT(14);
        break;
      case 4:
        ir = IS_IR_DECT(13);
        break;
      case 5:
        ir = IS_IR_DECT(12);
        break;
      case 6:
        ir = IS_IR_DECT(11);
        break;
      case 7:
        ir = IS_IR_DECT(10);
        break;
      case 8:
        ir = IS_IR_DECT(9);
        break;
      case 9:
        ir = IS_IR_DECT(8);
        break;
      case 10:
        ir = IS_IR_DECT(7);
        break;
      case 11:
        ir = IS_IR_DECT(6);
        break;
      case 12:
        ir = IS_IR_DECT(5);
        break;
      case 13:
        ir = IS_IR_DECT(4);
        break;
      case 14:
        ir = IS_IR_DECT(3);
        break;
      case 15:
        ir = IS_IR_DECT(2);
        break;
      case 16:
        ir = IS_IR_DECT(1);
        break;
      default:
        break;
    }
  }

  // 关闭货道电机
  TS_INIT(ts);
  switch (index) {
    case 1:
      MOTOR_DC_STOP(16);
      break;
    case 2:
      MOTOR_DC_STOP(15);
      break;
    case 3:
      MOTOR_DC_STOP(14);
      break;
    case 4:
      MOTOR_DC_STOP(13);
      break;
    case 5:
      MOTOR_DC_STOP(12);
      break;
    case 6:
      MOTOR_DC_STOP(11);
      break;
    case 7:
      MOTOR_DC_STOP(10);
      break;
    case 8:
      MOTOR_DC_STOP(9);
      break;
    case 9:
      MOTOR_DC_STOP(8);
      break;
    case 10:
      MOTOR_DC_STOP(7);
      break;
    case 11:
      MOTOR_DC_STOP(6);
      break;
    case 12:
      MOTOR_DC_STOP(5);
      break;
    case 13:
      MOTOR_DC_STOP(4);
      break;
    case 14:
      MOTOR_DC_STOP(3);
      break;
    case 15:
      MOTOR_DC_STOP(2);
      break;
    case 16:
      MOTOR_DC_STOP(1);
      break;
    default:
      break;
  }


  DBG_LOG("goods check used time:%dms, return:%d", TS_COUNT(tspick), ir);

  return (BOOL)ir;
}

/* Private functions ---------------------------------------------------------*/


/**
 * 电机控制微秒延时
 *
 * @param us     延时的微秒
 */
static void MotorDelayUs(uint32_t us) {

  volatile uint16_t i, j;

  for (i = 0; i < us; i++) {
    j = 5;
    while (j--) {
      continue;
    }
  }
}

/**
 * 电机控制调试命令行
 *
 * @param argc   参数数量
 * @param argv   参数列表
 */
static void motor_Console(int argc, char* argv[]) {
  uint32_t i = 0, j = 0;

  argc--;
  argv++;

  if (ARGV_EQUAL("xdir")) {
    argc--;
    argv++;
    if (ARGV_EQUAL("left")) {
      MOTOR_X_LEFT();
      DBG_LOG("motor X left.", );
    } else if (ARGV_EQUAL("right")) {
      MOTOR_X_RIGHT();
      DBG_LOG("motor X right.", );
    }
  } else if (ARGV_EQUAL("zdir")) {
    argc--;
    argv++;
    if (ARGV_EQUAL("up")) {
      MOTOR_Z_UP();
      DBG_LOG("motor Z up.", );
    } else if (ARGV_EQUAL("down")) {
      MOTOR_Z_DOWN();
      DBG_LOG("motor Z down.", );
    }
  } else if (ARGV_EQUAL("xstep")) {
    i = uatoi(argv[1]);
    j = uatoi(argv[2]);
    while (j--) {
      MOTOR_X_STEP(i);
    }
    DBG_LOG("motor x step over.", );
  } else if (ARGV_EQUAL("zstep")) {
    i = uatoi(argv[1]);
    j = uatoi(argv[2]);
    while (j--) {
      MOTOR_Z_STEP(i);
    }
    DBG_LOG("motor z step over.", );
  } else if (ARGV_EQUAL("setpper")) {
    i = uatoi(argv[1]);
    j = uatoi(argv[2]);
    MotorSetpperMove(i, j);
    DBG_LOG("motor X move:%d, Z move:%d.", i, j);
  } else if (ARGV_EQUAL("reset")) {
    DBG_LOG("setpeer motor reset beginning.");
    MotorSetpeerReset();
    DBG_LOG("setpeer motor reset complite..");
  } else if (ARGV_EQUAL("moveroad")) {
    i = uatoi(argv[1]);
    DBG_LOG("move to road:%d", i);
    if (i > 0 && i <= 16) {
      MotorMoveTo(gs_road_pos[i - 1][0], gs_road_pos[i - 1][1]);
    }
  } else if (ARGV_EQUAL("moveback")) {
    DBG_LOG("move to backup road.");
    MotorMoveTo(gs_backup_pos[0], gs_backup_pos[1]);
  } else if (ARGV_EQUAL("moveshow")) {
    DBG_LOG("move to show position.");
    MotorMoveTo(gs_show_pos[0], gs_show_pos[1]);
  } else if (ARGV_EQUAL("test")) {
    i = uatoi(argv[1]);
    DBG_LOG("pickup goods road:%d", i);
    if (i > 0 && i <= 16) {
      GoodsPickup(i);
      GoodsShow();
      DBG_LOG("rooad%d check result:d", GoodsCheck(i));
    }
  }
  // test
  else if (ARGV_EQUAL("gap_door_up")) {   // gap_door_up
    DBG_LOG("gap_door_up");
    DOOR_MOVE_FORWARD();
  } else if (ARGV_EQUAL("gap_door_down")) {   // gap_door_down
    DBG_LOG("gap_door_down");
    DOOR_MOVE_REVERSE();
  } else if (ARGV_EQUAL("gap_door_stop")) {   // gap_door_stop
    DBG_LOG("gap_door_stop");
    DOOR_MOVE_STOP();
  } else if (ARGV_EQUAL("cargo_motor_in")) {   // cargo_motor_in
    DBG_LOG("cargo_motor_in");
    CARGO_MOVE_FORWARD();
  } else if (ARGV_EQUAL("cargo_motor_out")) {   // cargo_motor_out
    DBG_LOG("cargo_motor_out");
    CARGO_MOVE_REVERSE();
  } else if (ARGV_EQUAL("cargo_motor_stop")) {   // cargo_motor_stop
    DBG_LOG("cargo_motor_stop");
    CARGO_MOVE_STOP();
  }
}

