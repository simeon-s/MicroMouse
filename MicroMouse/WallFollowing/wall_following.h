/*
 * wall_following.h
 *
 *  Created on: Aug 13, 2020
 *      Author: simeon
 */
#define WALL_FOLLOWING_ON

#ifndef WALL_FOLLOWING_H_
#define WALL_FOLLOWING_H_

extern unsigned int right_side;
extern unsigned int left_side;
extern unsigned int right_distance;
extern unsigned int left_distance;

// Acceleration and slowing
extern uint8_t currentSpeed;
extern uint8_t rampCoef;
extern uint8_t flagRampSlowing;
extern int32_t encLeftTempSaved;
extern int32_t encRightTempSaved;
extern int32_t encLeftCurrent;
extern int32_t encRightCurrent;
extern int16_t startingSpeedCoeff;
extern int flagGOTO_END;
extern signed int control_valueW;

extern void WallFollowing(void);


#endif /* WALL_FOLLOWING_H_ */
