/*
 * maze.c
 *
 *  Created on: Jun 23, 2020
 *      Author: simo
 */

#include "../Menu/dev_setup.h"
#include "../Libraries/Encoders.h"
#include "../Libraries/motors.h"
#include "../Inc/main.h"
#include "maze.h"
#include "../Menu/dev_setup.h"

#include "../Libraries/Glcd.h"
#include "../Fonts/fonts.h"
#include "../Fonts/splash_screen.h"


uint8_t currentSpeed = 0;
uint8_t rampCoef = 0;
uint8_t flagRampSlowing = 0;
int32_t encLeftTempSaved = 0;
int32_t encRightTempSaved = 0;
int32_t encLeftCurrent = 0;
int32_t encRightCurrent = 0;
int16_t startingSpeedCoeff = 0;
char mapDirections[] = "RRRRLLLS";
int16_t mapDistanceValues[] = {300,300,300}; // to do
signed int distance = 450*STEPK;
int16_t currentMapIndex2 = 0;
int16_t radiusTurnCurrentAngle = 0;


char tempChar;
unsigned int tempAngle = 0;
uint8_t mapArraySize = 0;
int8_t temp = 0;
char tempString[64] = "";

signed int mapSolve()
{
	mapArraySize = sizeof(mapDirections)/sizeof(mapDirections[0]);
	if(mapArraySize <currentMapIndex)
	{
		tempAngle = 0;

	}
	else {
		tempChar = mapDirections[currentMapIndex];
		switch(tempChar)
		{
		case 'L':
			tempAngle = TURN_LEFT_ANGLE;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
			break;

		case 'R':
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
			tempAngle = TURN_RIGHT_ANGLE;
			break;

		case 'S':
			MotorsStop();
			currentMapIndex = 0;
			endMapFlag = 1;
		}

	}

	LcdErase();
	sprintf(tempString, " %4c", tempChar);
	LcdTextXY(0, 2, BIG_FNT, CENTERED, tempString);
	sprintf(tempString, " %4d  %4d", currentMapIndex, currentMapIndex);
	LcdTextXY(0, 4, BIG_FNT, CENTERED, currentMapIndex);
	LcdUpdate();

	currentMapIndex++;

	return tempAngle;

}


void maze(void) {

	switch (maze_state) {
	case FORWARD:
		if (enc_rightCounter>= distance) {
		    mapSolve();
//			turn+=tempAngle;
			speedX = 0;
			if(tempAngle == TURN_LEFT_ANGLE)
			{
				maze_state = TURN_LEFT;
				turn+=tempAngle;
			}
			if(tempAngle == TURN_RIGHT_ANGLE)
				maze_state = TURN_RIGHT;
		}

		if(endMapFlag) // no more map characters or 'S'
		{
			MotorsStop();
		}

		// Acceleration
//		if( enc_rightCounter<RAMP*STEPK)
//		{
//			speedRampDiff = FAST_SPEED-STARTING_SPEED; // 150 - 50 = 100;
//			rampStep = speedRampDiff / (RAMP*STEPK); // 100/100 = 1;
//			currentSpeed = STARTING_SPEED + rampStep*enc_rightCounter*STEPK; // 50 + 1*50 = 100 speed na 50
//		}



		else
		{
			speedX =  FAST_SPEED;
		}

		// Slowing

			 encLeftTempSaved = enc_leftCounter;
			 encRightTempSaved = enc_rightCounter;
		//	 HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
			//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);


		if (flagRampSlowing == 1)
			{
				encLeftCurrent = enc_leftCounter - encLeftTempSaved;
				encRightCurrent = enc_rightCounter - encRightTempSaved;
				currentSpeed = FAST_SPEED-(FAST_SPEED - STOPPING_SPEED)/(RAMP/encRightCurrent); //150 - (150-100)/(100/5); 150 -50/2 = 125
				speedX = currentSpeed;
				//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
			}


		if(speedX <80)
		{

			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
		}
		else
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);

		}

		left_pwm = speedX - control_valueW;
		right_pwm = speedX + control_valueW;
		break;

	case TURN_RIGHT:
		if (radiusTurnCurrentAngle >=90)
		{
			radiusTurnCurrentAngle = 0;
			speedX = 50;
			enc_leftCounter = 0;
			enc_rightCounter = 0;
			maze_state = FORWARD;
			flagRampSlowing = 0;
		}
//		if (enc_rightCounter>= (9*STEPK))
		if(enc_rightCounter >=(DEGREE_STEP_RADIUS_TURN*STEPK*5))
		{
			turn+=5;
			radiusTurnCurrentAngle += 5;
			enc_rightCounter =0;
		}
		break;

	case TURN_LEFT:

		if (abs(errorW) >2)
		{
			speedX = 50;
			enc_leftCounter = 0;
			enc_rightCounter = 0;
			maze_state = FORWARD;
			//speedX = dev_settings.fast_speed;
			flagRampSlowing = 0;
		}

			left_pwm = 0;
			right_pwm = control_valueW;

		break;
	}

	//-----------------------------------------------------------



	MotorLeftSpeed(left_pwm);
	MotorRightSpeed(right_pwm);

}
