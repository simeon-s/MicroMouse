
#include "menu_items.h"
#include "../Menu/menu_functions.h"
#include "../Menu/dev_setup.h"

const char mainTitle[]		 = "Главно меню";
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
		{0, 		"Фронт Ляв",				NULL, 				frontL_callibrate},
		{0, 		"Фронт Десен",				NULL, 				frontR_callibrate},
		{0, 		"Ляв Ъглов",				NULL, 				sideL_callibrate},
		{0, 		"Десен Ъглов",				NULL, 				sideR_callibrate},
		{0, 		"Лява страна",				NULL, 				left_callibrate},
		{0, 		"Дясна страна",				NULL, 				right_callibrate},
		{0, 		"Жироскоп калибр.",			NULL, 				gyro_callibrate},
		{0,			"Фабр. настройки",			NULL,				factorySettings},
		{0,			NULL,              			NULL, 				NULL}             // end of menu
};


ST_MENUPOINT const mazeMenu[] =
{
		{0, 		"Стратегия",				NULL, 				maze_type_set},
		{0,		"Ширина", 					NULL,				maze_width_set},
		{0,		"Дебелина на стена",		NULL,			maze_wallWidth_set},
		{0, 		"Дистанция до ъгъл",		NULL, 			gotoEnd_set},

		{0,			NULL,               NULL, 				NULL}             // end of menu
};



ST_MENUPOINT const rampsMenu[] =
{
		{0,			"Стартова скорост", 	NULL,				starting_spped_set},
		{0,			"Спиране", 				NULL,				stopping_spped_set},
		{0,			"Рампа",					NULL,				ramp_set},

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
	{0,			"Скорост напред",    NULL,				fast_spped_set},
	{0,			"Завой",    			NULL, 			turn_spped_set},
	{0,			"Клампа завой",		NULL,				turn_clamp_set},
	{0,			"Обратен завой",		NULL,				uturn_spped_set},
	{0,			NULL,               	NULL, 				NULL}             // end of menu
};


//========================================  ROOT ====================================
ST_MENUPOINT const rootMenu[] =
{ 
		{0,			"Скорост",     		speedMenu,			NULL},
		{0,			"Рампи",					rampsMenu,			NULL},
		{0,			"Лабиринт",				mazeMenu,			NULL},
		{0,			"PID", 					pidMenu,			   NULL},
		{0,			"Калибриране",			callibrationMenu,	NULL},
		{0,			"Начално движение",		NULL,				start_way_set},
		{0,				NULL,               NULL, 				NULL}             // end of menu
};





//===============================================================================================
//-------------------------------------------Warnings -------------------------------------------
//===============================================================================================

ST_MENUPOINT const menuWarnings[] = {
	{ 0,			" ", 	                 	NULL, 				NULL},          	// NO ERROR - dummy warning
	{0,				NULL,                  		NULL, 				NULL}           	// end of menu
};



