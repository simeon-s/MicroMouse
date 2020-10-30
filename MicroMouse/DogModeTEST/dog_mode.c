/*
 * dog_mode.c
 *
 *  Created on: Aug 12, 2020
 *      Author: simeon
 */
#include "../Menu/dev_setup.h"
#include "../Libraries/Encoders.h"
#include "../Libraries/motors.h"
#include "../Inc/main.h"
#include "dog_mode.h"
#include "../Menu/dev_setup.h"

//#include "../Libraries/Glcd.h"
//#include "../Fonts/fonts.h"
//#include "../Fonts/splash_screen.h"

int8_t control_value = 0;
int8_t left_error =0;
int8_t leftIntegral = 0;
int8_t lastLeft_error = 0;
int8_t right_error = 0;
int8_t rightIntegral = 0;
int8_t lastRight_error = 0;
int8_t variableDeclarationTest = 0;
int8_t dogState = 0;
int8_t dogTurnFlag;
int8_t prevAngle = 0;

void DogModeFun(void)
{

	switch(dogState)
	{
	case DOG_SEARCHING:
		if(dogTurnFlag)
			dogTurnFlag = 0;
//		if((right_side<=WALL_SIDE_THRESHOLD) && (frontL_distance>FRONT_MAX_DISTANCE_DOG) && (frontR_distance>FRONT_MAX_DISTANCE_DOG))
		if(right_side<=WALL_SIDE_THRESHOLD)
		{
			turn+=TURN_RIGHT_ANGLE;
			dogState = TURN_RIGHT_DOG;
			turn+=TURN_RIGHT_ANGLE;
			prevAngle = lAngle;
			dogTurnFlag = 1;
			break;
		}
		if(left_side<=WALL_SIDE_THRESHOLD)  // (&& (frontL_distance>FRONT_MAX_DISTANCE_DOG) && (frontR_distance>FRONT_MAX_DISTANCE_DOG))
		{
			turn += TURN_LEFT_ANGLE;
			dogState = TURN_LEFT_DOG;
			prevAngle = lAngle;
			dogTurnFlag = 1;
			break;
		}


		 if ( frontL_distance <MIN_DISTANCE_DOG)
			   frontL_distance = 0;
		  left_error =  frontL_distance - FRONT_DESIRED_DISTANCE_DOG;

		  if ( frontL_distance > FRONT_MAX_DISTANCE_DOG ) {
			left_error = 0;
			leftIntegral = 0;
		  }
		  control_value = KP * left_error + KD * (left_error - lastLeft_error) + KI_DOG * leftIntegral;
			lastLeft_error = left_error;
			leftIntegral += left_error;
			if (leftIntegral > 800)
			  leftIntegral = 800;
			if (leftIntegral < -800)
			  leftIntegral = -800;

			if (control_value > 200)
			  control_value = 200;
			if (control_value < -200)
			  control_value = -200;

				left_pwm = control_value;

			//--------------------- Right PID ---------------------
			if (frontR_distance  <MIN_DISTANCE_DOG)
				frontR_distance  = 0;
			right_error = frontR_distance - FRONT_DESIRED_DISTANCE_DOG;
			// ERROR = 20-15=5
			// ERROR - 10-15=-5
			if ( frontR_distance  > FRONT_MAX_DISTANCE_DOG ) {
				right_error = 0;
				rightIntegral = 0;
			}

			// control_value = 4*5+ 10*(5-6)+0.03*500;
			// control_value = 20-10+15=25
			// controL_value = 4*-5+10*(-5+6)+0.03*500;
			// control_value = -20+10*1+15=5
			control_value = KP * right_error + KD * (right_error - lastRight_error) + KI_DOG * rightIntegral;
			lastRight_error = right_error;
			rightIntegral += right_error;
			if (rightIntegral > 800)
			  rightIntegral = 800;
			if (rightIntegral < -800)
			  rightIntegral = -800;

			if (control_value > 200)
			  control_value = 200;
			if (control_value < -200)
			  control_value = -200;

			right_pwm =  control_value;

			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);

			break;

		case TURN_LEFT_DOG:
			if (( prevAngle+TURN_LEFT_ANGLE) <=lAngle)
			{
				dogState = DOG_SEARCHING;
				prevAngle = 0;

			}
			left_pwm = control_valueW - TURN_SPEED;
			right_pwm = TURN_SPEED-control_valueW;
			break;

		case TURN_RIGHT_DOG:
			if ((abs(errorW) >2) && (prevAngle+TURN_RIGHT_ANGLE) <=lAngle)
			{
				dogState = DOG_SEARCHING;
				prevAngle = 0;

			}
			left_pwm = TURN_SPEED-control_valueW;
			right_pwm = control_valueW - TURN_SPEED;
			break;
	}




	    MotorLeftSpeed(left_pwm);
		MotorRightSpeed(right_pwm);


}


