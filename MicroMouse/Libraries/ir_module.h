/*
 * ir_module.h
 *
 *  Created on: 21.10.2015 �.
 *      Author: Simo
 */

#ifndef LIBRARIES_IR_MODULE_H_
#define LIBRARIES_IR_MODULE_H_


/*
 * Proocol
 */
#define IR_NEC_PROTOCOL
#define IR_NEC_REMOTE1
//#define IR_NEC_REMOTE2


//#define IR_RC5_PROTOCOL
//#define IR_RC5_PROTOCOL_STD		// For standart Philips TV remote control

/*
 * HW-dependent stuff. Change these macros if your hardware differs.
 */


/*
 * Debugging
 */
#define IR_DBG_EN	0

/*
 * Buffer size
 */
#define IR_BUFFER_SIZE 10
//extern volatile static IRDATA buffer[IR_BUFFER_SIZE];


#ifdef	IR_NEC_PROTOCOL
typedef struct {
	unsigned char address;
	unsigned char address_high;
	unsigned char command;
	unsigned char command_n;
} IRDATA;





#if IR_DBG_EN
extern volatile unsigned int 	cnt_start,
								cnt_space,
								cnt_addr1,
								cnt_addr2,
								cnt_cmd1,
								cnt_cmd2;
#endif

/*
 * Functions
 */
void ir_init(void);
unsigned char ir_get_repeats(void);

extern volatile IRDATA ir_data;
extern volatile uint8_t ir_data_ready;

extern volatile uint8_t pinIRQenable;
#endif


#ifdef	IR_RC5_PROTOCOL
// RC5 specific settings
#define IR_RC5_DISABLE_REPEATS	1

typedef struct {
	uint8_t command;
	uint8_t address;
	uint8_t dummy1;
	uint8_t dummy2;
} IRDATA;

extern volatile uint8_t ir_data_ready;
extern volatile IRDATA ir_data;
extern volatile uint8_t pinIRQenable;

//extern unsigned char ir_repeat_cmd;

#endif
/*
 * Functions
 */
void ir_init();
unsigned char ir_get_repeats();

//----------------
void IRpinIntHandler(void);
void Timer0AIntHandler(void);
void IR_TimerHandler(void);



/*
 * RC5 protocol standard addresses and commands
 */
#define RC5_ADR_TV1				0x00
#define RC5_ADR_TV2				0x01
#define RC5_ADR_TELETEXT		0x02
#define RC5_ADR_VIDEO			0x03
#define RC5_ADR_LV1				0x04
#define RC5_ADR_VCR1			0x05
#define RC5_ADR_VCR2			0x06
#define RC5_ADR_EXPERIMENTAL	0x07
#define RC5_ADR_SAT1			0x08
#define RC5_ADR_CAMERA			0x09
#define RC5_ADR_SAT2			0x0A
#define RC5_ADR_CDV				0x0C
#define RC5_ADR_CAMCORDER		0x0D
#define RC5_ADR_PREAMP			0x10
#define RC5_ADR_TUNER			0x11
#define RC5_ADR_RECORDER1		0x12
#define RC5_ADR_PREAMP2			0x13
#define RC5_ADR_CDPLAYER		0x14
#define RC5_ADR_PHONO			0x15
#define RC5_ADR_SATA			0x16
#define RC5_ADR_RECORDER2		0x17
#define RC5_ADR_CDR				0x1A
#define RC5_ADR_LIGHTING		0x1D
#define RC5_ADR_LIGHTING2		0x1E
#define RC5_ADR_PHONE			0x1F

#define RC5_CMD_0				0x00
#define RC5_CMD_1				0x01
#define RC5_CMD_2				0x02
#define RC5_CMD_3				0x03
#define RC5_CMD_4				0x04
#define RC5_CMD_5				0x05
#define RC5_CMD_6				0x06
#define RC5_CMD_7				0x07
#define RC5_CMD_8				0x08
#define RC5_CMD_9				0x09
#define RC5_CMD_MIN				0x0A
#define RC5_CMD_STANDBY			0x0C
#define RC5_CMD_MUTE			0x0D
#define RC5_CMD_VPLUS			0x10
#define RC5_CMD_VMIN			0x11
#define RC5_CMD_BPLUS			0x12
#define RC5_CMD_BMIN			0x13
#define RC5_CMD_PPLUS			0x20
#define RC5_CMD_PMIN			0x21
#define RC5_CMD_FRWD			0x32
#define RC5_CMD_FFWD			0x34
#define RC5_CMD_PLAY			0x35
#define RC5_CMD_STOP			0x36
#define RC5_CMD_RECORDING		0x37

#ifdef IR_RC5_PROTOCOL
#define		KEY_1		0x01
#define		KEY_2		0x02
#define		KEY_3		0x03
#define		KEY_4		0x04
#define		KEY_5		0x05
#define		KEY_6		0x06
#define		KEY_7		0x07
#define		KEY_8		0x08
#define		KEY_9		0x09
#define		KEY_0		0x00
#define		KEY_100		0x19
#define		KEY_200		0x0D
#define		KEY_PREV	0x44
#define		KEY_NEXT	0x40
#define		KEY_PLAY	0x0C		// Pwr
#define		KEY_CHUP	0x47
#define		KEY_CHDN	0x45
#define		KEY_CH		0x0D		// RC5_CMD_MUTE
#define		KEY_PLUS	0x15
#define		KEY_MINUS	0x07
#define		KEY_EQ		0x10		// RC5_CMD_VPLUS
#endif

/*
 * JVC observed commands
 */
#define JVC_CMD_10				0x2A
#define JVC_CMD_1				0x21
#define JVC_CMD_2				0x22
#define JVC_CMD_3				0x23
#define JVC_CMD_4				0x24
#define JVC_CMD_5				0x25
#define JVC_CMD_6				0x26
#define JVC_CMD_7				0x27
#define JVC_CMD_8				0x28
#define JVC_CMD_9				0x29
#define JVC_CMD_UP				0x1E
#define JVC_CMD_DOWN			0x1F
#define JVC_CMD_STANDBY			0x17
#define JVC_CMD_PREV			0x18
#define JVC_CMD_NEXT			0x19


#ifdef IR_NEC_PROTOCOL
#ifdef IR_NEC_REMOTE1
//----------------- NEC commands ----------
#define		KEY_1		0x0C
#define		KEY_2		0x18
#define		KEY_3		0x5E
#define		KEY_4		0x08
#define		KEY_5		0x1C
#define		KEY_6		0x5A
#define		KEY_7		0x42
#define		KEY_8		0x52
#define		KEY_9		0x4A
#define		KEY_0		0x16
#define		KEY_100		0x19
#define		KEY_200		0x0D
#define		KEY_PREV	0x44
#define		KEY_NEXT	0x40
#define		KEY_PLAY	0x43
#define		KEY_CHUP	0x47
#define		KEY_CHDN	0x45
#define		KEY_CH		0x46
#define		KEY_PLUS	0x15
#define		KEY_MINUS	0x07
#define		KEY_EQ		0x09
#endif


#ifdef IR_NEC_REMOTE2
//----------------- NEC commands ----------
#define		KEY_1		0x16
#define		KEY_2		0x19
#define		KEY_3		0x0D
#define		KEY_4		0x0C
#define		KEY_5		0x18
#define		KEY_6		0x5E
#define		KEY_7		0x08
#define		KEY_8		0x1C
#define		KEY_9		0x5A
#define		KEY_0		0x52
#define		KEY_PREV	0x44
#define		KEY_NEXT	0x43
#define		KEY_PLAY	0x40
#define		KEY_UP		0x46
#define		KEY_DN		0x15
#define		KEY_STAR	0x42
#define		KEY_SHARP	0x4A

#define 	KEY_CH		0x4A	//  #
#define 	KEY_MINUS	0x42	//  *
#define		KEY_EQ		0x40	//  OK
#endif
#endif

#endif /* LIBRARIES_IR_MODULE_H_ */
