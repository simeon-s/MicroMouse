
#include "menu_items.h"
#include "../Menu/menu_functions.h"
#include "../Menu/dev_setup.h"

const char mainTitle[]		 = "������ ����";
const char str_Service[]	= "SERVICE";

#define NULL	0

//============================================================================================================
//                           Menu structures for menus and menu items
//	EXAMPLE:
//	ST_MENUPOINT rootMenu[] =  {
//  //access_level	menuTitle				psubMenu			menuHandlerCallback
//  ....................
//  {0,				NULL,                  NULL, 				NULL}             // end of menu
//	}
//                String maximum length = 20 characters
//============================================================================================================

ST_MENUPOINT const callibrationMenu[] =
{
		{0, 		"����� ���",				NULL, 				frontL_callibrate},
		{0, 		"����� �����",				NULL, 				frontR_callibrate},
		{0, 		"��� �����",				NULL, 				sideL_callibrate},
		{0, 		"����� �����",				NULL, 				sideR_callibrate},
		{0, 		"���� ������",				NULL, 				left_callibrate},
		{0, 		"����� ������",				NULL, 				right_callibrate},
		{0, 		"�������� ������.",			NULL, 				gyro_callibrate},
		{0,			"����. ���������",			NULL,				factorySettings},
		{0,			NULL,              			NULL, 				NULL}             // end of menu
};


ST_MENUPOINT const mazeMenu[] =
{
		{0, 		"���������",				NULL, 				maze_type_set},
		{0,		"������", 					NULL,				maze_width_set},
		{0,		"�������� �� �����",		NULL,			maze_wallWidth_set},
		{0, 		"��������� �� ����",		NULL, 			gotoEnd_set},

		{0,			NULL,               NULL, 				NULL}             // end of menu
};



ST_MENUPOINT const rampsMenu[] =
{
		{0,			"�������� �������", 	NULL,				starting_spped_set},
		{0,			"�������", 				NULL,				stopping_spped_set},
		{0,			"�����",					NULL,				ramp_set},

		{0,			NULL,               NULL, 				NULL}             // end of menu
};


//-----------------------------
ST_MENUPOINT const pidMenu[] =
{
		{0,			"KP", 				NULL,				kp_set},
		{0,			"KD", 				NULL,				kd_set},
		{0,			"KI", 				NULL,				ki_set},
		{0,         "WKP",            NULL,           wkp_set},
		{0,         "WKD",            NULL,           wkd_set},
		{0,         "WKI",            NULL,            wki_set},
		{0,			NULL,             NULL, 				NULL}             // end of menu
};


//-------------------------------
ST_MENUPOINT const speedMenu[] =
{
	{0,			"������� ������",    NULL,				fast_spped_set},
	{0,			"�����",    			NULL, 			turn_spped_set},
	{0,			"������ �����",		NULL,				turn_clamp_set},
	{0,			"������� �����",		NULL,				uturn_spped_set},
	{0,			NULL,               	NULL, 				NULL}             // end of menu
};


//========================================  ROOT ====================================
ST_MENUPOINT const rootMenu[] =
{ 
		{0,			"�������",     		speedMenu,			NULL},
		{0,			"�����",					rampsMenu,			NULL},
		{0,			"��������",				mazeMenu,			NULL},
		{0,			"PID", 					pidMenu,			   NULL},
		{0,			"�����������",			callibrationMenu,	NULL},
		{0,			"������� ��������",		NULL,				start_way_set},
		{0,				NULL,               NULL, 				NULL}             // end of menu
};





//===============================================================================================
//-------------------------------------------Warnings -------------------------------------------
//===============================================================================================

ST_MENUPOINT const menuWarnings[] = {
	{ 0,			" ", 	                 	NULL, 				NULL},          	// NO ERROR - dummy warning
	{0,				NULL,                  		NULL, 				NULL}           	// end of menu
};



