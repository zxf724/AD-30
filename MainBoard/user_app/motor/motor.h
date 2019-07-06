/*
 * Copyright © 2019 copyright ShenZhen AFU Smart Internet Technology Co.,Ltd.
 * All Right Reserved.
 *
 * Version Logs:
 * Date           Author         Notes
 * 2019.6.26      Yang Song      first version.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MOTOR_H_
#define MOTOR_H_

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DC_MOTOR_MOVE_TIME_MAX           10000
#define CARGO_MOTOR_MOVE_TIME_MAX        3000
#define DOOR_MOTOR_MOVE_TIME_MAX         3000

#define MOTOR_RESET_TIME_MAX             35000

// 步进电机启停时的脉宽
#define MOTOR_X_START_PLUS               2000
#define MOTOR_Z_START_PLUS               500

// 步进电机高速运转时的脉宽
#define MOTOR_X_FAST_PLUS                400
#define MOTOR_Z_FAST_PLUS                16

// 步进电机调速间隔的时间
#define MOTOR_X_SPEED_CHANGE_TIME        1
#define MOTOR_Z_SPEED_CHANGE_TIME        4

// 步进电机需减速时的剩余脉冲数
#define MOTOR_X_SPEED_SLOWDOWN_COUNT     500
#define MOTOR_Z_SPEED_SLOWDOWN_COUNT     3000

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// 货道直流电机启动
#define MOTOR_DC_ACTION(x)    IO_H(MOTOR##x)

// 货道直流电机停止
#define MOTOR_DC_STOP(x)      IO_L(MOTOR##x)

// 红外检测
#define IS_IR_DECT(x)         (IO_READ(DEM##x) == 0)

// 货舱电机正反转
#define DOOR_MOVE_FORWARD() do {IO_H(MOTOR_R1); IO_L(MOTOR_L1);} while (0)
#define DOOR_MOVE_REVERSE() do {IO_H(MOTOR_L1); IO_L(MOTOR_R1);} while (0)
#define DOOR_MOVE_STOP()    do {IO_L(MOTOR_L1); IO_L(MOTOR_R1);} while (0)

// 舱门电机正反转
#define CARGO_MOVE_FORWARD() do {IO_H(MOTOR_L2); IO_L(MOTOR_R2);} while (0)
#define CARGO_MOVE_REVERSE() do {IO_H(MOTOR_R2); IO_L(MOTOR_L2);} while (0)
#define CARGO_MOVE_STOP()    do {IO_L(MOTOR_L2); IO_L(MOTOR_R2);} while (0)

// X轴与Z轴步进电机使能
#define MOTOR_Z_ENABLE(en)  IO_L(EN1)
#define MOTOR_X_ENABLE(en)  IO_L(EN2)

// X轴与Z轴步进电机失能
#define MOTOR_Z_DISABLE(en) IO_H(EN1)
#define MOTOR_X_DISABLE(en) IO_H(EN2)

// X轴电机方向左右移
#define MOTOR_X_LEFT()      IO_H(DIR2)
#define MOTOR_X_RIGHT()     IO_L(DIR2)

// Z轴电机方向左右移
#define MOTOR_Z_UP()        IO_L(DIR1)
#define MOTOR_Z_DOWN()      IO_H(DIR1)

// X轴电机步进
#define MOTOR_Z_STEP(x)     do {IO_H(PUL1); MotorDelayUs(x); IO_L(PUL1); MotorDelayUs(x);\
                                  IO_H(PUL1); MotorDelayUs(x); IO_L(PUL1); MotorDelayUs(x);} while (0)


// Z轴电机步进
#define MOTOR_X_STEP(x)     do {IO_H(PUL2); MotorDelayUs(x); IO_L(PUL2); MotorDelayUs(x);\
                                  IO_H(PUL2); MotorDelayUs(x); IO_L(PUL2); MotorDelayUs(x);} while (0)

// 步进电机翻转
#define MOTOR_X_TOGGLE()    IO_TOGGLE(PUL2)
#define MOTOR_Z_TOGGLE()    IO_TOGGLE(PUL1)

/* External variables --------------------------------------------------------*/
/* External functions --------------------------------------------------------*/

void MotorControlInit(void);

void MotorSetpeerReset(void);
void MotorSetpperMove(uint32_t xstep, uint32_t zstep);
void GoodsPickup(uint8_t index);
void GoodsShow(void);
BOOL GoodsCheck(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
