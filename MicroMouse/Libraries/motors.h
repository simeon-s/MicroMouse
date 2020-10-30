/*
 * motors.h
 *
 *  Created on: 10.07.2015
 *      Author: Simo
 */

#ifndef MOTORS_H_
#define MOTORS_H_


void TurboStart(void);
void TurboStop(void);
void TurboSpeed(int32_t speed);

void MotorsStart(void);
void MotorsStop(void);
void MotorLeftSpeed(int32_t speed);
void MotorRightSpeed(int32_t speed);

#endif /* MOTORS_H_ */
