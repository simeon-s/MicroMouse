#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "common.h"

#include "../Libraries/Glcd.h"
#include "../Libraries/lcd_SPI_drv.h"
#include "../Libraries/KBD_scan.h"

#include "../Menu/menu.h"
#include "../Menu/menu_functions.h"
#include "../Language/menu_items.h"
#include "../Language/strings.h"

#include "../Menu/dev_setup.h"

#include "../MPU6050/sd_hal_mpu6050.h"

//---------------------------------------------------
#define ST_ECR_INIT	0
#define ST_ECR_INPUT	1
#define ST_ECR_EDIT	2
#define ST_ECR_CONFIRM	3

static char setup_state = ST_ECR_INIT;

//============================= Global Structrures ============================
ST_DEV_SETTINGS dev_settings;
ST_MAZE_SETTINGS maze_settings;

//----------------------------------------------------------
static unsigned long usInpValue;
//static char char_value;
//static unsigned char tmp_edit_buff[16];

unsigned int front_a_value;
unsigned int front_b_value;

//-------------------------- Robot Settings ------------------------

void dev_factory_settings(void) {
	unsigned int *devset_ptr;
	unsigned char i;
	unsigned int chk_sum;

	//clear the whole structure because of strings
	memset((char*) &dev_settings, 0, sizeof(ST_DEV_SETTINGS));
	dev_settings.test = 0xAA55;
	dev_settings.brightness = 220;

	dev_settings.fast_speed = FAST_SPEED;
	dev_settings.turn_speed = TURN_SPEED;
	dev_settings.uturn_speed = UTURN_SPEED;
	dev_settings.starting_speed = STARTING_SPEED;
	dev_settings.stopping_speed = STOPPING_SPEED;
	dev_settings.ramp = RAMP;
	dev_settings.turn_clamp = UTURN_SPEED;

	dev_settings.kp = KP;
	dev_settings.kd = KD;
	dev_settings.ki = KI;

	dev_settings.wkp = WKP;
	dev_settings.wkd = WKD;
	dev_settings.wki = WKI;

	dev_settings.frontL_callibration = FRONT_CAL;
	dev_settings.frontR_callibration = FRONT_CAL;
	dev_settings.sideL_callibration = SIDE_CALL;
	dev_settings.sideR_callibration = SIDE_CALL;
	dev_settings.left_callibration = FRONT_CAL;
	dev_settings.right_callibration = FRONT_CAL;
	dev_settings.gyro_offset = 0;

	devset_ptr = (unsigned int*) &dev_settings;
	chk_sum = 0;
	for (i = (sizeof(ST_DEV_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_DEV_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}
	dev_settings.chk_sum = chk_sum;
	eepr_buffWrite((uint8_t *) &dev_settings, DEV_SETTINGS, sizeof(ST_DEV_SETTINGS)); // 4 ����� - 32����

	//-------- Maze ----
	maze_settings.maze_width = MAZE_WIDTH;
	maze_settings.wall_width = WALL_WIDTH;
	maze_settings.maze_type = MAZE_TYE;
	maze_settings.goto_end_distance = GOTO_END_DISTANCE;
	chk_sum = 0;
	devset_ptr = (unsigned int*) &maze_settings;
	for (i = (sizeof(ST_MAZE_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_MAZE_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}
	maze_settings.chk_sum = chk_sum;
	eepr_buffWrite((uint8_t *) &maze_settings, MAZE_SETTINGS, sizeof(ST_MAZE_SETTINGS)); // 4 ����� - 32����

}

//---------------------------------
void dev_settings_save(void) {
	unsigned int *devset_ptr;
	unsigned char i;
	unsigned int chk_sum;

	dev_settings.test = 0xAA55;
	devset_ptr = (unsigned int*) &dev_settings;
	chk_sum = 0;
	for (i = (sizeof(ST_DEV_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_DEV_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}
	dev_settings.chk_sum = chk_sum;
	eepr_buffWrite((uint8_t *) &dev_settings, DEV_SETTINGS, sizeof(ST_DEV_SETTINGS)); // 4 ����� - 32����
}

//---------------------------------
void maze_settings_save(void) {
	unsigned int *devset_ptr;
	unsigned char i;
	unsigned int chk_sum;

	chk_sum = 0;
	devset_ptr = (unsigned int*) &maze_settings;
	for (i = (sizeof(ST_MAZE_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_MAZE_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}
	maze_settings.chk_sum = chk_sum;
	eepr_buffWrite((uint8_t *) &maze_settings, MAZE_SETTINGS, sizeof(ST_MAZE_SETTINGS)); // 4 ����� - 32����
}

//------------------------------------------------------

unsigned char dev_settings_chk(void) {
	unsigned int *devset_ptr;
	unsigned char i;
	unsigned int chk_sum;

	eepr_buffRead((uint8_t *) &dev_settings, DEV_SETTINGS, sizeof(ST_DEV_SETTINGS));	// 4 ����� - 32����
	devset_ptr = (unsigned int*) &dev_settings;
	chk_sum = 0;
	for (i = (sizeof(ST_DEV_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_DEV_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}

	if ((chk_sum != dev_settings.chk_sum) || (dev_settings.test != 0xAA55))
		return 1; // Error
	else
		return 0;

}

//------------------------------------------------------

unsigned char maze_settings_chk(void) {
	unsigned int *devset_ptr;
	unsigned char i;
	unsigned int chk_sum;

	eepr_buffRead((uint8_t *) &maze_settings, MAZE_SETTINGS, sizeof(ST_MAZE_SETTINGS)); // 4 ����� - 32����
	devset_ptr = (unsigned int*) &maze_settings;
	chk_sum = 0;
	for (i = (sizeof(ST_MAZE_SETTINGS) / 4 - 1); i > 0; i--) { //sizeof (ST_MAZE_SETTINGS)
		chk_sum += *devset_ptr;
		devset_ptr++;
	}

	if (chk_sum != maze_settings.chk_sum)
		return 1; // Error
	else
		return 0;

}

//===============================================================================

unsigned char factorySettings(unsigned char kbd_key) {

	dev_factory_settings();
	LcdErase();
	TextBox(1, 3, 19, BOLD_FNT, (char*) "�������� ���������\n������������1!");

	gulTickCountDown = 20000;
	while (gulTickCountDown)
		;

	return 1;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------

unsigned char fast_spped_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 2;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.fast_speed;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 50;
		property.uiMaxValue = 230;

		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.fast_speed = usInpValue;

#ifdef AUTO_TUNING
		dev_settings.ramp = dev_settings.fast_speed * 0.75;
		//  �� �� ���� ������ ����� �� ����� ��� ����� ����� � �����  ��� �����.
		dev_settings.uturn_speed = dev_settings.turn_speed - ( 10 - ((dev_settings.fast_speed - 100) * 0.3));
#endif

		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char turn_spped_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 2;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.turn_speed;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 0;
		property.uiMaxValue = 255;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.turn_speed = usInpValue;

#ifdef AUTO_TUNING
		// ����� � ���� ���������� ������� �� ��������� � �����. ������ ������ �� � ������� ����������,
		//  �� �� ���� ������ ����� �� ���� � ������ �� ������ �� turn_speed,  ��� �� ��� �����������.
		dev_settings.starting_speed = dev_settings.turn_speed * 0.8;
		dev_settings.stopping_speed = dev_settings.turn_speed * 0.7;
		// uturn_speed  � �������� �� ��������� �������. �� ������ �� � ����� ��� ����� ��-������ �� ��������� ,
		//  �� �� ���� ������ ����� �� ����� ��� ����� ����� � �����  ��� �����. ( -5 ���� �� ������ �� � +5 )
//		dev_settings.uturn_speed = dev_settings.turn_speed + 10;
		dev_settings.uturn_speed = dev_settings.turn_speed - ( 10 - ((dev_settings.fast_speed - 100) * 0.3));//���� � ���
#endif

		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char uturn_spped_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 2;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.uturn_speed;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 40;
		property.uiMaxValue = 255;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.uturn_speed = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char starting_spped_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 2;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.starting_speed;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 30;
		property.uiMaxValue = 150;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.starting_speed = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back with new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char stopping_spped_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 2;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.stopping_speed;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 20;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.stopping_speed = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char ramp_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 5; // input step
		usInpValue = dev_settings.ramp;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.ramp = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char turn_clamp_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.turn_clamp;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 255;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.turn_clamp = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char kp_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.kp;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 50;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.kp = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char kd_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.kd;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.kd = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char ki_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.ki;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.ki = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//===========================================================================
//--------------------------------------------------------------------------

unsigned char wkp_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.wkp;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 50;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.wkp = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char wkd_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.wkd;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.wkd = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------

unsigned char wki_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = dev_settings.wki;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		dev_settings.wki = usInpValue;
		dev_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

/*
 //--------------------------------------------------------------------------

 unsigned char display_contrast_set(unsigned char kbd_key) {
 unsigned char result;
 unsigned short chk_sum;
 unsigned short* devset_ptr;
 unsigned char i;

 result = 0;
 switch (setup_state) {
 case ST_ECR_INIT:
 property.ucInit = 1;
 property.ucIntSize = 2;
 property.ucFractSize = 0;
 usInpValue = dev_settings.lcd_contrast;
 property.pucValue = ( char*) &usInpValue;
 property.ucFont = BIG_FNT;
 property.x_pos = 5;
 property.y_pos = 3;
 property.uiMinValue = 10;
 property.uiMaxValue = 40;
 property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

 result = EditValue(0xFF, &property);
 setup_state = ST_ECR_INPUT;

 break;

 case ST_ECR_INPUT:
 result = EditValue(kbd_key, &property);
 //result = 1;
 break;
 }

 if (result == 1) {
 dev_settings.lcd_contrast = usInpValue;

 devset_ptr = (unsigned short*) &dev_settings;
 chk_sum = 0;
 for (i = (sizeof (ST_DEV_SETTINGS) / 2 - 1); i > 0; i--) { //sizeof (ST_DEV_SETTINGS)
 chk_sum += *devset_ptr;
 devset_ptr++;
 }
 dev_settings.chk_sum = chk_sum;
 eepr_buffWrite(DEV_SETTINGS, (unsigned char *) &dev_settings, sizeof (dev_settings));
 LcdContrast(dev_settings.lcd_contrast);
 setup_state = ST_ECR_INIT;
 return 1; // ESC  - go back without new value in the buffer

 }

 //------- Go back -------
 //if ( kbd_key == KEY_MOD)  {
 if (result == 2) {
 setup_state = ST_ECR_INIT;
 return 2; // ESC  - go back without new value in the buffer
 }
 return 0;
 }
 */

//--------------------------------------------------------------------------
unsigned char maze_width_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 1; // input step
		usInpValue = maze_settings.maze_width;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 50;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		maze_settings.maze_width = usInpValue;
		maze_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------------------------------------------------------
unsigned char gotoEnd_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 10; // input step
		usInpValue = maze_settings.goto_end_distance;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 200;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		maze_settings.goto_end_distance = usInpValue;
		maze_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//-------------------------------------------------------------------
unsigned char maze_wallWidth_set(unsigned char kbd_key) {
	unsigned char result;

	result = 0;
	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucInit = 1;
		property.ucIntSize = 1;
		property.ucFractSize = 5; // input step
		usInpValue = maze_settings.wall_width;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 5;
		property.y_pos = 3;
		property.uiMinValue = 1;
		property.uiMaxValue = 50;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);
		setup_state = ST_ECR_INPUT;

		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		maze_settings.wall_width = usInpValue;
		maze_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//----------------------------------------------------------------------
unsigned char maze_type_set(unsigned char kbd_key) {
	unsigned char result;
	result = 0;

	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucIntSize = 16;
		property.pucSelect = (char**) &str_MazeRun;
		property.ucOptions = 4;
		usInpValue = maze_settings.maze_type;
		property.pucValue = (char*) &usInpValue;
		property.x_pos = 1;
		property.y_pos = 1;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;
		SelectBox(0xFF, &property);
		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = SelectBox(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		maze_settings.maze_type = usInpValue;
		maze_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

	//------- Go back -------
	//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

//--------------------------- Callibration --------------------------
extern SD_MPU6050 mpu1;
#define SAMPLES_NR		6000
//-------------------------------------------------------------------
// ---------- MPU6050 callibrate --------
/**
 *
 * @param kbd_key
 * @return
 */
unsigned char gyro_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	uint32_t tmp_long;
	uint32_t gulDeltaT;
	char sign;

	LcdBox(2, 4, 125, 60);
	LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);
	LcdTextXY(1, 3, BOLD_FNT, CENTERED, "���� ���������!");
	LcdUpdate();

	gulTickCountDown = 30000;
	while (gulTickCountDown)
		;

	LcdTextXY(1, 4, BOLD_FNT, CENTERED, "< < < * > > >");
	LcdUpdate();
	g_pfGyroOffset[2] = 0;

	tmp_long = 0;
	while (tmp_long < SAMPLES_NR) {
		if (mpu_irq) {
			mpu_irq = 0;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
			SD_MPU6050_ReadGyroscope(&hi2c1, &mpu1);

			//
			// Convert the gyroscope values into grad/sec
			//
			g_pfGyro[2] = (float) mpu1.Gyroscope_Z * mpu1.Gyro_Mult;				// Get float
			gulDeltaT = gulTimeTick;
			gulTimeTick = 0;
			fAngle += (g_pfGyro[2] - g_pfGyroOffset[2]) * 0.0001 * gulDeltaT;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);

			//g_pfGyroOffset[0] += g_pfGyro[0];
			//g_pfGyroOffset[1] += g_pfGyro[1];
			g_pfGyroOffset[2] += g_pfGyro[2];
			tmp_long ++;
		}
	}
//g_pfGyroOffset[0] /= SAMPLES_NR;
//g_pfGyroOffset[1] /= SAMPLES_NR;
	g_pfGyroOffset[2] /= SAMPLES_NR;

	dev_settings.gyro_offset = g_pfGyroOffset[2];
	dev_settings_save();
	float_format(g_pfGyroOffset[2]);
	sign = ' ';
	if (g_pfGyroOffset[2] < 0)
		sign = '-';
	LcdTextXY(1, 3, BOLD_FNT, CENTERED, "                ");
	sprintf(tmp_str, "%c%ld,%03ld", sign, i32IPart, i32FPart);
	LcdTextXY(1, 3, BOLD_FNT, CENTERED, tmp_str);
	LcdUpdate();

	gulTickCountDown = 30000;
	while (gulTickCountDown)
		;

	return 1;
}

//--------------------------------------
unsigned char frontL_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 750; // input step
		usInpValue = dev_settings.frontL_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 500;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.frontL_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_a_value = usInpValue / sensor_values[4];
	sprintf(tmp_str, "%3d", front_a_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}

unsigned char frontR_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 250; // input step
		usInpValue = dev_settings.frontR_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 10000;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.frontR_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_b_value = usInpValue / sensor_values[1];
	sprintf(tmp_str, " %3d", front_b_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}

//-------------------------------------------------------------------
//--------------------------------------
unsigned char sideL_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 250; // input step
		usInpValue = dev_settings.sideL_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 10000;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.sideL_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_a_value = usInpValue / sensor_values[3];
	sprintf(tmp_str, "%3d", front_a_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}

unsigned char sideR_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 250; // input step
		usInpValue = dev_settings.sideR_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 10000;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.sideR_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_b_value = usInpValue / sensor_values[2];
	sprintf(tmp_str, "%3d", front_b_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}
//=====================================================================================

//--------------------------------------
unsigned char left_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 250; // input step
		usInpValue = dev_settings.left_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 10000;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.left_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_a_value = usInpValue / sensor_values[5];
	sprintf(tmp_str, "%3d", front_a_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}

unsigned char right_callibrate(unsigned char kbd_key) {
	char tmp_str[24];
	char result;

	switch (setup_state) {
	case ST_ECR_INIT:
		LcdBox(2, 4, 125, 60);
		LcdTextXY(0, 0, NORMAL_FNT, CENTERED, (char*) currentMenu[currentMenuNumber].menuTitle);

		property.ucInit = 1;
		property.ucIntSize = 5;
		property.ucFractSize = 250; // input step
		usInpValue = dev_settings.right_callibration;
		property.pucValue = (char*) &usInpValue;
		property.ucFont = BIG_FNT;
		property.x_pos = 4;
		property.y_pos = 4;
		property.uiMinValue = 10000;
		property.uiMaxValue = 200000;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;

		result = EditValue(0xFF, &property);

		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = EditValue(kbd_key, &property);
		if (result == 1) {
			dev_settings.right_callibration = usInpValue;
			dev_settings_save();
		}
		if (result) {
			setup_state = ST_ECR_INIT;
			gusKeyRepeatTimer = 0;
			return 1; // ESC  - go back without new value in the buffer
		}
		break;
	}

	front_b_value = usInpValue / sensor_values[0];
	sprintf(tmp_str, " %3d", front_b_value);
	LcdTextXY(0, 2, BIG_FNT, CENTERED | NORM_LINE, tmp_str);
	LcdUpdate();

	/*
	 //------- Go back -------
	 if ( kbd_key == KEY_BACK)  {
	 setup_state = ST_ECR_INIT;
	 gusKeyRepeatTimer = 0
	 return 2; // ESC  - go back without new value in the buffer
	 }
	 */
	gusKeyRepeatTimer = 10;		// 100ms
	return 0;
}


//-------------------------------------------------------------------
unsigned char start_way_set(unsigned char kbd_key) {
	unsigned char result;
	result = 0;

	switch (setup_state) {
	case ST_ECR_INIT:
		property.ucIntSize = 16;
		property.pucSelect = (char**) &str_MazeStart;
		property.ucOptions = 3;
		usInpValue = maze_settings.start_way;
		property.pucValue = (char*) &usInpValue;
		property.x_pos = 1;
		property.y_pos = 1;
		property.pucTitle = (char*) currentMenu[currentMenuNumber].menuTitle;
		SelectBox(0xFF, &property);
		setup_state = ST_ECR_INPUT;
		break;

	case ST_ECR_INPUT:
		result = SelectBox(kbd_key, &property);
		//result = 1;
		break;
	}

	if (result == 1) {
		maze_settings.start_way = usInpValue;
		maze_settings_save();
		setup_state = ST_ECR_INIT;
		return 1; // ESC  - go back without new value in the buffer

	}

//------- Go back -------
//if ( kbd_key == KEY_MOD)  {
	if (result == 2) {
		setup_state = ST_ECR_INIT;
		return 2; // ESC  - go back without new value in the buffer
	}
	return 0;
}

