/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f7xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TRIGGER_xxV_12_Pin GPIO_PIN_3
#define TRIGGER_xxV_12_GPIO_Port GPIOE
#define TRIGGER_xxV_11_Pin GPIO_PIN_4
#define TRIGGER_xxV_11_GPIO_Port GPIOE
#define TRIGGER_xxV_10_Pin GPIO_PIN_5
#define TRIGGER_xxV_10_GPIO_Port GPIOE
#define TRIGGER_xxV_9_Pin GPIO_PIN_6
#define TRIGGER_xxV_9_GPIO_Port GPIOE
#define INDICATOR_R_Pin GPIO_PIN_0
#define INDICATOR_R_GPIO_Port GPIOB
#define INDICATOR_G_Pin GPIO_PIN_1
#define INDICATOR_G_GPIO_Port GPIOB
#define INDICATOR_B_Pin GPIO_PIN_2
#define INDICATOR_B_GPIO_Port GPIOB
#define SIMULATE_OPTICAL_CURTAIN_Pin GPIO_PIN_13
#define SIMULATE_OPTICAL_CURTAIN_GPIO_Port GPIOF
#define TRIGGER_24V_8_Pin GPIO_PIN_8
#define TRIGGER_24V_8_GPIO_Port GPIOE
#define TRIGGER_24V_7_Pin GPIO_PIN_9
#define TRIGGER_24V_7_GPIO_Port GPIOE
#define TRIGGER_24V_6_Pin GPIO_PIN_10
#define TRIGGER_24V_6_GPIO_Port GPIOE
#define TRIGGER_24V_5_Pin GPIO_PIN_11
#define TRIGGER_24V_5_GPIO_Port GPIOE
#define TRIGGER_24V_4_Pin GPIO_PIN_12
#define TRIGGER_24V_4_GPIO_Port GPIOE
#define SIMULATE_ENCODER_CH2_Pin GPIO_PIN_13
#define SIMULATE_ENCODER_CH2_GPIO_Port GPIOE
#define TRIGGER_24V_2_Pin GPIO_PIN_14
#define TRIGGER_24V_2_GPIO_Port GPIOE
#define TRIGGER_24V_1_Pin GPIO_PIN_15
#define TRIGGER_24V_1_GPIO_Port GPIOE
#define ENC_24V_1_Pin GPIO_PIN_14
#define ENC_24V_1_GPIO_Port GPIOB
#define ENC_24V_1_EXTI_IRQn EXTI15_10_IRQn
#define ENC_24V_2_Pin GPIO_PIN_15
#define ENC_24V_2_GPIO_Port GPIOB
#define ENC_24V_2_EXTI_IRQn EXTI15_10_IRQn
#define ENC_24V_3_Pin GPIO_PIN_10
#define ENC_24V_3_GPIO_Port GPIOD
#define ENC_24V_3_EXTI_IRQn EXTI15_10_IRQn
#define ENC_24V_4_Pin GPIO_PIN_11
#define ENC_24V_4_GPIO_Port GPIOD
#define ENC_24V_4_EXTI_IRQn EXTI15_10_IRQn
#define ENC_24V_5_Pin GPIO_PIN_12
#define ENC_24V_5_GPIO_Port GPIOD
#define ENC_24V_6_Pin GPIO_PIN_13
#define ENC_24V_6_GPIO_Port GPIOD
#define USB_DM___Pin GPIO_PIN_11
#define USB_DM___GPIO_Port GPIOA
#define USB_DM__A12_Pin GPIO_PIN_12
#define USB_DM__A12_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define SIMULATE_ENCODER_CH1_Pin GPIO_PIN_4
#define SIMULATE_ENCODER_CH1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
