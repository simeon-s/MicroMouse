/*
 * wall_following.c
 *
 *  Created on: Aug 13, 2020
 *      Author: simeon
 */
#include "../Menu/dev_setup.h"
#include "../Libraries/Encoders.h"
#include "../Libraries/motors.h"
#include "../Inc/main.h"
#include "wall_following.h"
#include "../Menu/dev_setup.h"


int16_t ramp_step;
int16_t speed_ramp_diff;
// Wall PID

int16_t errorSum = 0;
int16_t control_valX = 0;
E_3DMAZE_STATE STATE;
int16_t wallSideerrorX= 0;
int16_t l_distance = 0;



int8_t flagForward = 0;
int8_t flagStopTurn = 1;
void initVar()
{
 int current_speed = 0;

}
void forward()
{
//	MotorLeftSpeed(100);
//	delay(50);
	MotorLeftSpeed(80);
	MotorRightSpeed(80);
	delay(400);
}
void clearErrors()
{
	lastErrorX = 0;
	errorSum =0;
}
void clearEncoders()
{
	enc_leftCounter = 0;
	enc_rightCounter = 0;

	HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);

}

void WallFollowing(void)
{
//		if((right_side>(WALL_DESIRED_DISTANCE*3)) && (flagForward == 1)) // If there is no wall
//			if((right_side>(WALL_DESIRED_DISTANCE*3)) && enc_rightCounter >= 300*STEPK)
//				STATE = TURN_RIGHT;
//			else
//				STATE = FORWARD   ;

		switch(STATE){
		case FORWARD:
			initVar();
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
			 // Doesn't work at the top
		   //control_value = kp*errorX+ kd*(error-lastErrorX) + ki * errorSum
			l_distance = left_distance;
			if(left_distance>130)
				l_distance = 130;
			if(left_distance < 70)
				l_distance = 70;
			errorX = WALL_FOLLOWING_TARGET - l_distance; // rd - 10, 15,20
//			control_valX = KP*errorX+ KD*(error-lastErrorX) + KI*(errorSum);
			proportionalX = dev_settings.kp *  errorX;
			derivateX = dev_settings.kd * (errorX- lastErrorX);
			proportionalX /=10;
			derivateX /=10;
			control_valX = proportionalX + derivateX;
			lastErrorX = errorX;

//			errorSum += error;
//			if(errorSum >2000)
//			{
//				errorSum = 2000;
//			}

			// Acceleration
			if( enc_rightCounter<RAMP*STEPK)
			{
				speed_ramp_diff = dev_settings.fast_speed-dev_settings.starting_speed; // 150 - 50 = 100;
				ramp_step = speed_ramp_diff / (dev_settings.ramp*STEPK); // 100/100 = 1;
				current_speed = dev_settings.starting_speed + ramp_step*enc_rightCounter*STEPK; // 50 + 1*50 = 100 speed na 50
			}
			else
			{
				current_speed = dev_settings.fast_speed;
			}

//			left_pwm = dev_settings.fast_speed + control_valX;
//			right_pwm = dev_settings.fast_speed - control_valX;
			left_pwm = current_speed + control_valX + control_valueW;
			right_pwm = current_speed - control_valX + control_valueW;
			if((left_distance >200) && (enc_rightCounter > 300*STEPK))
			{
				clear_encoders();
				STATE = GOTO_END;

			}
			//TODO if turn => clr enc
			break;
		case GOTO_END:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);

			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);

			errorX = WALL_FOLLOWING_TARGET - left_distance; // rd - 10, 15,20
//			control_valX = KP*errorX+ KD*(error-lastErrorX) + KI*(errorSum);
			proportionalX = dev_settings.kp *  errorX;
			derivateX = dev_settings.kd * (errorX- lastErrorX);
			proportionalX /=10;
			derivateX /=10;
			control_valX = proportionalX + derivateX;
			lastErrorX = errorX;


			left_pwm = 80;
			right_pwm = 80;
			if(left_side>150)
			{


				MotorLeftSpeed(80);
				MotorRightSpeed(80);
				delay(1200);
				STATE = TURN_LEFT;

			}
			break;
		case TURN_RIGHT:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);

			if(enc_leftCounter<200)
			{
				left_pwm = dev_settings.turn_speed;
				right_pwm = 0;
			}
			else
				STATE = FORWARD;

			break;


		case TURN_LEFT:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);

			if(left_distance>120)
			{
				left_pwm = -20;
				right_pwm = dev_settings.turn_speed;
				delay(200);

			}
			else
			{
				forward();
				STATE = FORWARD;
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);
			}


			break;

		}
		MotorLeftSpeed(left_pwm);
		MotorRightSpeed(right_pwm);

}
