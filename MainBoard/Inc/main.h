/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RTC_CLK_Pin GPIO_PIN_2
#define RTC_CLK_GPIO_Port GPIOE
#define RTC_DATA_Pin GPIO_PIN_3
#define RTC_DATA_GPIO_Port GPIOE
#define RTC_CS_Pin GPIO_PIN_4
#define RTC_CS_GPIO_Port GPIOE
#define DEM21_Pin GPIO_PIN_5
#define DEM21_GPIO_Port GPIOE
#define DEM22_Pin GPIO_PIN_6
#define DEM22_GPIO_Port GPIOE
#define DEM12_Pin GPIO_PIN_0
#define DEM12_GPIO_Port GPIOC
#define MOTOR8_Pin GPIO_PIN_1
#define MOTOR8_GPIO_Port GPIOC
#define MOTOR9_Pin GPIO_PIN_2
#define MOTOR9_GPIO_Port GPIOC
#define MOTOR10_Pin GPIO_PIN_3
#define MOTOR10_GPIO_Port GPIOC
#define MOTOR1_Pin GPIO_PIN_0
#define MOTOR1_GPIO_Port GPIOA
#define MOTOR2_Pin GPIO_PIN_1
#define MOTOR2_GPIO_Port GPIOA
#define MOTOR3_Pin GPIO_PIN_2
#define MOTOR3_GPIO_Port GPIOA
#define MOTOR4_Pin GPIO_PIN_3
#define MOTOR4_GPIO_Port GPIOA
#define MOTOR5_Pin GPIO_PIN_4
#define MOTOR5_GPIO_Port GPIOA
#define MOTOR6_Pin GPIO_PIN_5
#define MOTOR6_GPIO_Port GPIOA
#define MOTOR7_Pin GPIO_PIN_6
#define MOTOR7_GPIO_Port GPIOA
#define DEM1_Pin GPIO_PIN_7
#define DEM1_GPIO_Port GPIOA
#define MOTOR11_Pin GPIO_PIN_4
#define MOTOR11_GPIO_Port GPIOC
#define MOTOR12_Pin GPIO_PIN_5
#define MOTOR12_GPIO_Port GPIOC
#define SFLASH_WP_Pin GPIO_PIN_2
#define SFLASH_WP_GPIO_Port GPIOB
#define SFLASH_CS_Pin GPIO_PIN_1
#define SFLASH_CS_GPIO_Port GPIOB
#define MOTOR13_Pin GPIO_PIN_7
#define MOTOR13_GPIO_Port GPIOE
#define MOTOR14_Pin GPIO_PIN_8
#define MOTOR14_GPIO_Port GPIOE
#define MOTOR15_Pin GPIO_PIN_9
#define MOTOR15_GPIO_Port GPIOE
#define MOTOR16_Pin GPIO_PIN_10
#define MOTOR16_GPIO_Port GPIOE
#define MOTOR17_Pin GPIO_PIN_11
#define MOTOR17_GPIO_Port GPIOE
#define DEM13_Pin GPIO_PIN_12
#define DEM13_GPIO_Port GPIOE
#define DEM14_Pin GPIO_PIN_13
#define DEM14_GPIO_Port GPIOE
#define DEM15_Pin GPIO_PIN_14
#define DEM15_GPIO_Port GPIOE
#define DEM16_Pin GPIO_PIN_15
#define DEM16_GPIO_Port GPIOE
#define W5500_RST_Pin GPIO_PIN_10
#define W5500_RST_GPIO_Port GPIOB
#define W5500_INT_Pin GPIO_PIN_11
#define W5500_INT_GPIO_Port GPIOB
#define W5500_CS_Pin GPIO_PIN_12
#define W5500_CS_GPIO_Port GPIOB
#define EN1_Pin GPIO_PIN_10
#define EN1_GPIO_Port GPIOD
#define DIR1_Pin GPIO_PIN_11
#define DIR1_GPIO_Port GPIOD
#define PUL1_Pin GPIO_PIN_12
#define PUL1_GPIO_Port GPIOD
#define EN2_Pin GPIO_PIN_13
#define EN2_GPIO_Port GPIOD
#define DIR2_Pin GPIO_PIN_14
#define DIR2_GPIO_Port GPIOD
#define LED_NET_Pin GPIO_PIN_7
#define LED_NET_GPIO_Port GPIOC
#define LED_ERR_Pin GPIO_PIN_8
#define LED_ERR_GPIO_Port GPIOC
#define DEM2_Pin GPIO_PIN_8
#define DEM2_GPIO_Port GPIOA
#define DEM3_Pin GPIO_PIN_9
#define DEM3_GPIO_Port GPIOA
#define DEM4_Pin GPIO_PIN_10
#define DEM4_GPIO_Port GPIOA
#define DEM5_Pin GPIO_PIN_11
#define DEM5_GPIO_Port GPIOA
#define DEM6_Pin GPIO_PIN_12
#define DEM6_GPIO_Port GPIOA
#define DEM7_Pin GPIO_PIN_15
#define DEM7_GPIO_Port GPIOA
#define MOTOR_R1_Pin GPIO_PIN_10
#define MOTOR_R1_GPIO_Port GPIOC
#define MOTOR_L1_Pin GPIO_PIN_11
#define MOTOR_L1_GPIO_Port GPIOC
#define MOTOR_R2_Pin GPIO_PIN_0
#define MOTOR_R2_GPIO_Port GPIOD
#define MOTOR_L2_Pin GPIO_PIN_1
#define MOTOR_L2_GPIO_Port GPIOD
#define DEM18_Pin GPIO_PIN_3
#define DEM18_GPIO_Port GPIOD
#define DEM19_Pin GPIO_PIN_4
#define DEM19_GPIO_Port GPIOD
#define DEM20_Pin GPIO_PIN_7
#define DEM20_GPIO_Port GPIOD
#define DEM8_Pin GPIO_PIN_6
#define DEM8_GPIO_Port GPIOB
#define DEM9_Pin GPIO_PIN_7
#define DEM9_GPIO_Port GPIOB
#define DEM10_Pin GPIO_PIN_8
#define DEM10_GPIO_Port GPIOB
#define DEM11_Pin GPIO_PIN_9
#define DEM11_GPIO_Port GPIOB
#define DEM17_Pin GPIO_PIN_0
#define DEM17_GPIO_Port GPIOE
#define PUL2_Pin GPIO_PIN_1
#define PUL2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
