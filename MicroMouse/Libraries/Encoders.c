/*
 Author : Simeon Simeonov
 Name : Optical Encoders driver
 Ver: 1.0.0
 */

//#include "Energia.h"
#include <stdint.h>
#include <stdbool.h>

#include "Encoders.h"

volatile uint32_t enc_leftCounter;
volatile uint32_t enc_rightCounter;

volatile uint32_t enc_last_leftCounter;
volatile uint32_t enc_last_rightCounter;

volatile uint32_t enc_left_speed;
volatile uint32_t enc_right_speed;

/*-----------------------------------------------------------------------
 7 pole magnet - Both edges interrupt - 30 reduction ratio
 420 pulses per RPM
 speed = pulses / 420 * 60 ( rpms) * wheel_length
 speed = 4000/420*60*(42*3.1415) = 4000/7 * 42 *3.1415
 speed = 4000 / 7 * 132
 */

//=======================================================================
void EncodersInit(void) {
	enc_leftCounter = 0;
	enc_rightCounter = 0;

	enc_last_leftCounter = 0;
	enc_last_rightCounter = 0;

	enc_left_speed = 0;
	enc_right_speed = 0;

}

//-----------------  get Counters : -------------
uint32_t enc_getLeftCounter(void) {
	return enc_leftCounter;
}

uint32_t enc_getRightCounter(void) {
	return enc_rightCounter;
}

// ---------------- Clear counters ----------------
void clear_encoders(void) {
	enc_leftCounter = 0;
	enc_rightCounter = 0;

	enc_last_leftCounter = 0;
	enc_last_rightCounter = 0;

	enc_left_speed = 0;
	enc_right_speed = 0;
}

void enc_clearLeftCounter(void) {
	enc_leftCounter = 0;
}

void enc_clearRightCounter(void) {
	enc_rightCounter = 0;
}

