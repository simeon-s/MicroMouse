/*
 * ir_module.c
 *
 *  Created on: 21.10.2015 �.
 *      Author: Simo
 */

#include <stdint.h>
#include <stdbool.h>
#include "ir_module.h"

#include "main.h"

/*=====================================================================*/
/* HW-dependent stuff. Change these macros if your hardware differs. */
/*
 * Timer0 A and EINT0 (PE_0)
 */

//#define IR_EDGE_LOW			HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin)
//#define IR_EDGE_HIGH		HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin)
/* ===================================================================== */
volatile uint8_t ir_data_ready;
volatile uint8_t pinIRQenable = 1;

unsigned long ulCountsPerMicrosecond;
unsigned long gulTicks;
unsigned char ir_repeat_cmd;

// -------------------------- END HW-stuff --------------------------- //

#ifdef	IR_RC5_PROTOCOL

/*
 * Protocol definitions
 */
#define ADDR_LEN	5
#define CMD_LEN		6

/*
 * Timings in us
 */
#define HALF_BIT_LEN	889UL
#define BIT_LEN			1778UL 		//- 1333UL

/*
 * Timer constants
 */
#define MAX_ERROR		120				// Maximum timing error in timer ticks

#define HALF_BIT_CNT	HALF_BIT_LEN
#define BIT_CNT			BIT_LEN
#define IR_CNT			TIM6->CNT

/*
 * Interrupt stuff
 */
#define	IR_STARTBIT1	0
#define	IR_STARTBIT2	1
#define	IR_TOGGLEBIT	2
#define	IR_ADDR			3
#define	IR_CMD			4

/*
 * Variables
 */
volatile static int8_t state;
volatile static unsigned char bit_counter = 0;
volatile static unsigned char last_bit;
volatile static unsigned char ir_error;
volatile static unsigned long last_data;
volatile IRDATA ir_data;

/*
 * Variables
 */
volatile static int8_t status;
volatile static int8_t toggle;
volatile static uint8_t repeats;
volatile static uint8_t buffer_read, buffer_write;

/*
 * Buffer with received data
 */
volatile static IRDATA buffer[IR_BUFFER_SIZE];

/*
 * Data handler function
 */
static void (*ir_data_func)(IRDATA *data) = 0;

//-------------------------------------------------------------------------------
void ir_init(void) {
	/*
	 * Variables
	 */
	state = IR_STARTBIT1;
	bit_counter = 0;
	ir_data_ready = 0;
	pinIRQenable = 1;
	buffer_read = 0;
	buffer_write = 0;
	toggle = 0;
	HAL_TIM_Base_Start_IT(&htim6);
	TIM6->CNT = 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/*
 void IRpinIntHandler(void) {
 unsigned long delay;

 // Workaround to interrupt on any edge
 if (!ir_data_ready) {
 delay = BIT_LEN - HALF_BIT_LEN / 2;				//1333UL;
 __HAL_TIM_SET_AUTORELOAD(&htim6, delay);
 HAL_TIM_Base_Start_IT(&htim6);
 TIM6->CNT = 0;
 pinIRQenable = 0;

 }
 }
 */

//=========================================================================================
//=========================================================================================
void IRpinIntHandler(void) {
	uint8_t edge;
	static uint32_t pulse_time;
	static uint32_t error;
	static uint32_t bit_counter;

	static uint8_t index = 0, addr = 0, cmd = 0, toggle_last = 0;

	pulse_time = IR_CNT;
	error = MAX_ERROR + 1;
	bit_counter = BIT_CNT + 1;

	edge = HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin);

	//  - - - interrupt on any edge - - -
	switch (status) {
	case IR_STARTBIT1:
		if (edge == 0) {
			status = IR_STARTBIT2;
			IR_CNT = 0;
		} else {
			status = IR_STARTBIT1;
		}
		break;

	case IR_STARTBIT2:
		if ((pulse_time - error) < bit_counter && (pulse_time + error) > bit_counter) {
			status = IR_TOGGLEBIT;
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
			IR_CNT = 0;

		} else if ((IR_CNT - MAX_ERROR) < HALF_BIT_CNT && (IR_CNT + MAX_ERROR) > HALF_BIT_CNT) {
			IR_CNT = HALF_BIT_CNT;	// Synchronize..
			return;
		} else {
			status = IR_STARTBIT1;
			return;
		}
		break;

	case IR_TOGGLEBIT:
		if ((IR_CNT - MAX_ERROR) < BIT_CNT && (IR_CNT + MAX_ERROR) > BIT_CNT) {

			toggle_last = toggle;

			if (edge == 0) {
				toggle = 1;
			} else {
				toggle = 0;
			}

			index = 0;
			addr = 0;
			cmd = 0;

			IR_CNT = 0;
			status = IR_ADDR;

		} else if ((IR_CNT - MAX_ERROR) < HALF_BIT_CNT && (IR_CNT + MAX_ERROR) > HALF_BIT_CNT) {
			IR_CNT = HALF_BIT_CNT;	// Synchronize..
			return;
		} else {
			status = IR_STARTBIT1;
			return;
		}
		break;

	case IR_ADDR:
		if ((IR_CNT - MAX_ERROR) < BIT_CNT && (IR_CNT + MAX_ERROR) > BIT_CNT) {

			if (edge == 0) {
				addr |= 0x01;
			}
			index++;
			if (index == ADDR_LEN) {
				index = 0;
				status = IR_CMD;
			} else {
				addr <<= 1;
			}
			IR_CNT = 0;
		} else if ((IR_CNT - MAX_ERROR) < HALF_BIT_CNT && (IR_CNT + MAX_ERROR) > HALF_BIT_CNT) {
			IR_CNT = HALF_BIT_CNT;	// Synchronize..
			return;
		} else {
			status = IR_STARTBIT1;
			return;
		}
		break;

	case IR_CMD:
		if ((IR_CNT - MAX_ERROR) < BIT_CNT && (IR_CNT + MAX_ERROR) > BIT_CNT) {

			if (edge == 0) {
				cmd |= 0x01;
			}

			index++;
			if (index == CMD_LEN) {
				ir_data.address = addr;
				ir_data.command = cmd;
				repeats = 0;
				ir_data_ready = 1;
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
				status = IR_STARTBIT1;
			}

			cmd <<= 1;
			IR_CNT = 0;
		} else if ((IR_CNT - MAX_ERROR) < HALF_BIT_CNT && (IR_CNT + MAX_ERROR) > HALF_BIT_CNT) {
			IR_CNT = HALF_BIT_CNT;	// Synchronize..
			return;
		} else {
			status = IR_STARTBIT1;
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
			return;
		}
		break;
	}
}

void Timer0AIntHandler() {
	status = IR_STARTBIT1;
}

#endif   ///---  #ifdef	IR_RC5_PROTOCOL

//==================================================================================================
//==================================================================================================
//==================================================================================================
#ifdef IR_NEC_PROTOCOL

/*
 * Protocol definitions
 */
#define ADDR_LEN	16
#define CMD_LEN		16

#define IR_CNT_GET			gulTicks
#define IR_CNT_SET(ticks)	gulTicks = ticks

/*
 * Interrupt stuff
 */
#define	IR_STARTBIT1	0
#define	IR_STARTBIT2	1
#define	IR_ADDRL		2
#define	IR_ADDRH		3
#define	IR_CMD			4
#define	IR_CMDN			5

/*
 * Variables
 */
volatile static int8_t status;
volatile static uint8_t bits;
volatile unsigned char edge = 0;
volatile IRDATA ir_data;
volatile unsigned long *ir_command;

//-------------------------------------------------------------------------------
void ir_init(void) {

	/*
	 * Variables
	 */
	status = IR_STARTBIT1;
	ir_data_ready = 0;
	bits = 0;
	ir_command = 0;
	pinIRQenable = 1;
	edge = 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void IRpinIntHandler(void) {
	unsigned long period;

	if (HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin) == edge) {
		period = IR_CNT_GET;
		IR_CNT_SET(0);

		switch (status) {
			case IR_STARTBIT1:
			// Workaround to interrupt on any edge
			if (!edge) {
				edge = 1;
			} else {
				if ((period >= 85) && (period < 96) && !ir_data_ready) {
					status = IR_STARTBIT2;
				}
				edge = 0;
			}
			break;

			case IR_STARTBIT2:
			if ((period > 40) && (period < 50)) {
				status = IR_CMD;
				edge = 0;
				ir_command = (unsigned long*) &ir_data;
				*ir_command = 0;
				bits = 0;

			} else {
				status = IR_STARTBIT1;
				return;
			}
			break;

			case IR_CMD:
			if ((bits < 32) && (period < 30)) {
				ir_command = (unsigned long*) &ir_data;
				if ((period > 10) && (period < 15)) {
					*ir_command >>= 1;
				} else if (period > 16) {
					*ir_command >>= 1;
					*ir_command |= 0x80000000;
				}
				bits++;
			}
			if (bits >= 32) {
				ir_data_ready = 1;
				status = IR_STARTBIT1;
				edge = 0;
				return;
			}

			if (period > 30) {
				status = IR_STARTBIT1;
				edge = 0;
				return;
			}

			break;

		}
	}

}

//==================  Timer 0 A ===================================
void IR_TimerHandler(void) {
//
// Clear the timer interrupt flag.
//

	gulTicks++;

	if (gulTicks > 110) {
		status = IR_STARTBIT1;
		edge = 0;
		// Reset to Falling Edge
		//ROM_GPIOIntTypeSet(IR_PORT, IR_PIN, GPIO_FALLING_EDGE);
	}
}

#endif 	/// IR_NEC_PROTOCOL

