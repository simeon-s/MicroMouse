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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

extern signed int turn; // mazeswitch (maze_state) {



//------------------------ Robot ---------------------
#define ROBOT_N1


//--------------------- Macros definitions -----------
// Delay 0.1ms
#define delay(ms) gulTickCountDown = ms; while (gulTickCountDown);

//=========================================================================
#define SENSORS_NR		6

//------------- Walls ---------
#define NO_WALL		0
#define LEFT_WALL		1
#define RIGHT_WALL	2
#define FRONT_WALL	4

//---------  Robot ----------
#define ROBOT_LENGTH		90		// mm
#define ROBOT_WIDTH			100		// mm
#define SENSORS_GAP			65		// mm

typedef enum {
	FORWARD, GOTO_END, DISCOVER, PREPARE_TURN, TURN, TURN_LEFT, TURN_RIGHT, STOPPING, CIRCLE_RIGHT
} E_3DMAZE_STATE;





//------------------------------------------------------------------------------
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;


//---------------- Global variables -------------------
extern volatile uint32_t gulTick;
extern volatile uint32_t gulTickCountDown;
extern volatile uint32_t gulTimeTick;
extern volatile uint32_t gulSysTimer;
extern volatile unsigned char gucSecondsTimer;
extern volatile unsigned char gucSecondsFlag;
extern uint32_t gulCurrentTimestamp;
extern uint32_t gulEditTimestamp;
//------------

extern volatile uint16_t sensor_values[SENSORS_NR];

//E_3DMAZE_STATE maze_state;

// ---------- Keyboard ----------
extern volatile unsigned short gucKeyboardTimer;
extern volatile unsigned char gucRepeatOff;
extern volatile unsigned char gucKeyboardKey;
extern volatile unsigned short gusKeyRepeatTimer;
extern volatile unsigned char gucKeyHoldTimer;
extern unsigned char last_key;

//----------------------------------------------------
extern unsigned int frontL_distance;
extern unsigned int frontR_distance;
extern unsigned int left_distance;
extern unsigned int right_distance;

//------------------  PID -----------------------
// Linear error - distance from Walls
extern signed int errorX;
extern signed int diff;
extern signed int errors_sumX;
extern signed int lastErrorX;
extern signed int proportionalX;
extern signed long integralX;
extern signed long derivateX;
extern signed int control_valueX;
extern signed int speedX;

//  Rotational error - angular error from Gyro  - W = omega
extern signed int errorW;
extern signed int errors_sumW;
extern signed int lastErrorW;
extern signed int lastErrorW1;
extern signed int lastErrorW2;
extern signed int proportionalW;
extern signed long integralW;
extern signed long derivateW;
extern signed int control_valueW;
extern signed int speedW;


extern E_3DMAZE_STATE maze_state;
//extern E_DOG_MODE_STATE dogState;

//------------
extern signed int left_pwm;
extern  signed int right_pwm;
extern float speed_ramp;
extern float current_speed;
extern signed int speed_setpoint;
extern signed int distance;
extern signed int angle;

//--------------
extern volatile uint8_t mpu_irq;

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
//--------------
extern int32_t i32IPart;
extern int32_t i32FPart;
void float_format(float inpData);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PING1_Pin GPIO_PIN_2
#define PING1_GPIO_Port GPIOA
#define PING2_Pin GPIO_PIN_3
#define PING2_GPIO_Port GPIOA
#define LCD_A0_Pin GPIO_PIN_4
#define LCD_A0_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_5
#define LCD_CS_GPIO_Port GPIOC
#define BUTT1_Pin GPIO_PIN_0
#define BUTT1_GPIO_Port GPIOB
#define BUTT2_Pin GPIO_PIN_1
#define BUTT2_GPIO_Port GPIOB
#define BUTT3_Pin GPIO_PIN_2
#define BUTT3_GPIO_Port GPIOB
#define BUTT4_Pin GPIO_PIN_10
#define BUTT4_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13 // 12
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_12 // 13
#define LED2_GPIO_Port GPIOB
#define GYRO_Pin GPIO_PIN_15
#define GYRO_GPIO_Port GPIOB
#define GYRO_EXTI_IRQn EXTI15_10_IRQn
#define IR_Pin GPIO_PIN_11
#define IR_GPIO_Port GPIOA
#define IR_EXTI_IRQn EXTI15_10_IRQn
#define BEEP_Pin GPIO_PIN_2
#define BEEP_GPIO_Port GPIOD
#define DIR1_Pin GPIO_PIN_4
#define DIR1_GPIO_Port GPIOB
#define DIR2_Pin GPIO_PIN_5
#define DIR2_GPIO_Port GPIOB
#define ENC1_Pin GPIO_PIN_6
#define ENC1_GPIO_Port GPIOB
#define ENC1_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
