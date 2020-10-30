/*
 * motors.c
 *
 *  Created on: 10.07.2015
 *      Author: Simo
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "main.h"



//===============================================  Turbo ========================================
void TurboStart(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

//----------------------
void TurboStop(void) {
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}


//---------------------
void TurboSpeed(int32_t speed) {
	unsigned long duty;
	duty = abs(speed);
	if (duty > 128)
		duty = 128;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty); //sets the PWM duty cycle (Capture Compare Value)
}


//============================================= Motors ========================================

void MotorsStart(void) {
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

//----------------------
void MotorsStop(void) {
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
}

//---------------------
void MotorLeftSpeed(int32_t speed) {
	unsigned long duty;

	if (speed == 0)
		speed = 1;
	if (speed > 0) {
		HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR2_Pin, RESET);
	} else {
		HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR2_Pin, SET);
	}
	duty = abs(speed);
	if (duty > 255)
		duty = 255;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty); //sets the PWM duty cycle (Capture Compare Value)
}

//---------------------
void MotorRightSpeed(int32_t speed) {
	unsigned long duty;

	if (speed == 0)
		speed = 1;
	if (speed > 0) {
		HAL_GPIO_WritePin(DIR2_GPIO_Port, DIR1_Pin, SET);
	} else {
		HAL_GPIO_WritePin(DIR2_GPIO_Port, DIR1_Pin, RESET);
	}
	duty = abs(speed);
	if (duty > 255)
		duty = 255;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty); //sets the PWM duty cycle (Capture Compare Value)
}

