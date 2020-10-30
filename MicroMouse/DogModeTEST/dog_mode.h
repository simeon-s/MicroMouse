//#define DOG_MODE_ON

#ifndef DOG_MODE_H_
#define DOG_MODE_H_

typedef enum {
	DOG_SEARCHING, TURN_LEFT_DOG, TURN_RIGHT_DOG
} E_DOG_MODE_STATE;

extern unsigned int right_side;
extern unsigned int left_side;
//extern E_DOG_MODE_STATE dogState;
extern int8_t dogTurnFlag;
extern signed int lAngle;
extern void DogModeFun(void);

#endif
