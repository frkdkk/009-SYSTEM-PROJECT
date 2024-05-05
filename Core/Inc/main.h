/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define ADC1_OLC2_Pin GPIO_PIN_0
#define ADC1_OLC2_GPIO_Port GPIOC
#define ADC1_OLC3_Pin GPIO_PIN_1
#define ADC1_OLC3_GPIO_Port GPIOC
#define ADC1_OLC5_Pin GPIO_PIN_3
#define ADC1_OLC5_GPIO_Port GPIOC
#define REF_Pin GPIO_PIN_0
#define REF_GPIO_Port GPIOA
#define L1_OLC_Pin GPIO_PIN_1
#define L1_OLC_GPIO_Port GPIOA
#define L2_OLC_Pin GPIO_PIN_2
#define L2_OLC_GPIO_Port GPIOA
#define L3_OLC_Pin GPIO_PIN_3
#define L3_OLC_GPIO_Port GPIOA
#define DAC_VOLTAJ_OUT_Pin GPIO_PIN_4
#define DAC_VOLTAJ_OUT_GPIO_Port GPIOA
#define DAC_AKIM_OUT_Pin GPIO_PIN_5
#define DAC_AKIM_OUT_GPIO_Port GPIOA
#define I1_OLC_Pin GPIO_PIN_6
#define I1_OLC_GPIO_Port GPIOA
#define I2_OLC_Pin GPIO_PIN_7
#define I2_OLC_GPIO_Port GPIOA
#define AKIM_FB_Pin GPIO_PIN_5
#define AKIM_FB_GPIO_Port GPIOC
#define I3_OLC_Pin GPIO_PIN_0
#define I3_OLC_GPIO_Port GPIOB
#define ADC1_OLC_Pin GPIO_PIN_1
#define ADC1_OLC_GPIO_Port GPIOB
#define SD_CARD_TX_Pin GPIO_PIN_10
#define SD_CARD_TX_GPIO_Port GPIOB
#define SD_CARD_RX_Pin GPIO_PIN_11
#define SD_CARD_RX_GPIO_Port GPIOB
#define NEXTION_TX_Pin GPIO_PIN_5
#define NEXTION_TX_GPIO_Port GPIOD
#define NEXTION_RX_Pin GPIO_PIN_6
#define NEXTION_RX_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
