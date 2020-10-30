/*
 * File:        menu.c
 * Purpose:     Menu functions
 * Author:      Simeon Simeonov
 * Modified by:
 * Created:     2007-05-19 
 * Last modify: 2007-10-03  

 */

#include <stdio.h>
#include <string.h>

#include "main.h"

#include "../Libraries/Glcd.h"
#include "../Libraries/lcd_SPI_drv.h"
#include "../Libraries/KBD_scan.h"

#include "../Fonts/splash_screen.h"
#include "menu.h"
#include "../Language/menu_items.h"
#include "../Language/strings.h"

#include "menu_functions.h"

//----------- Global Variables ----------------------
unsigned char op_access_level = 0;
unsigned char menuState = 0;
unsigned char gucWarnings = 0;
char gucEventFlag = 0;						// Global event flag
char gucAdminMode = 0;						// Disable  low level acounts
char gucDefaultMenu = 0;					// Flag for sales mode

char warning_timer;

const ST_MENUPOINT* currentMenu;
unsigned char currentMenuNumber;       // number of selected menu item

//---------------------------------------------------
static unsigned char currentMenuOffset;      // displaying of menu items start from 'offset' - Scrolling
static unsigned char currentMenuSize;        // number of menu items in actual menu point
static unsigned char currentMenuLevel;
static signed char selectedMenuItem;
static const ST_MENUPOINT* parentMenu[MENU_MAX_LEVEL];
static const char * parentMenuTitle[MENU_MAX_LEVEL];
static unsigned char x = MENU_DEFAULT_X;
static unsigned char y = MENU_DEFAULT_Y;
char navbar[24];

//--------------  Function Prototypes ----------------
void MENU_printMenu();
void MENU_printMenuItem(unsigned char menuNumber, char style);
unsigned char MENU_getMenuSize(const ST_MENUPOINT* menu);

// maximum number of items to print
//#define MENU_MAX_MENU_ITEMS_LCD     LCD_HEIGHT / FONT_HEIGHT

//------------------------- Menu INIT -----------------------------------------
// Initializes menu system. After this you should call MENU_handler  periodically.
//
void MENU_init(const ST_MENUPOINT* rootMenu) {
	unsigned short i;
	currentMenu = rootMenu;
	currentMenuOffset = 0;
	currentMenuNumber = 0;
	currentMenuLevel = 0;
	selectedMenuItem = -1;

	op_access_level = 10;
	menuState = 0;
	x = MENU_DEFAULT_X;

	for (i = 0; i < MENU_MAX_LEVEL; i++) {		// Clear parent Menu path
		parentMenu[i] = NULL;
		parentMenuTitle[i] = NULL;
	}
	currentMenuSize = MENU_getMenuSize(currentMenu);
	parentMenuTitle[0] = mainTitle;

	LcdErase();
	MENU_printMenu();

	/*
	 LcdBitmap(16, 0, (IMG_DEF*) &splash_screen);
	 LcdTextXY(0, 7, NORMAL_FNT, NORMAL, " ");
	 LcdUpdate();
	 */
}

//-------------------------------- Print Menu ----------------------------------
void MENU_printMenu() {
	unsigned char i;
	char menu_item[22];

	LcdErase();
	sprintf(navbar, " %s", parentMenuTitle[currentMenuLevel]);
	LcdTextXY(0, 0, BIG_FNT, INV_LINE, navbar);
	//LcdInvertRow(0);
	//LcdHLine(0,127,7,0);
	y = MENU_DEFAULT_Y;
	for (i = currentMenuOffset; i < currentMenuSize && i < currentMenuOffset + MENU_MAX_MENU_ITEMS_LCD; i++) {
		if (op_access_level > currentMenu[i].access_level) {
			sprintf(menu_item, "%d.%s", i + 1, currentMenu[i].menuTitle);
			//LcdTextXY(x,y +( i - currentMenuOffset) ,NORMAL_FNT,NORM_LINE,menu_item);
			LcdTextXY(x, y, NORMAL_FNT, NORM_LINE, menu_item);
			y++;
		}
	}
	//if (selectedMenuItem !=-1)
	//	MENU_printMenuItem(selectedMenuItem,1);
	LcdUpdate();
}

//-------------------------------- Print Menu Item -----------------------------    
void MENU_printMenuItem(unsigned char menuNumber, char style) {
	char menu_item[22];

	y = MENU_DEFAULT_Y - currentMenuOffset;
	if ((menuNumber - currentMenuOffset) < MENU_MAX_MENU_ITEMS_LCD) {
		sprintf(menu_item, "%d.%s", menuNumber + 1, currentMenu[menuNumber].menuTitle);
		//LcdTextXY(x,y +( i - currentMenuOffset) ,NORMAL_FNT,NORM_LINE,menu_item);
		if (style)
			LcdTextXY(x, menuNumber + y, NORMAL_FNT, INV_LINE, menu_item);
		else
			LcdTextXY(x, menuNumber + y, NORMAL_FNT, NORM_LINE, menu_item);
	}
	LcdUpdate();
}

///-----------------------------------------------------------------------------
unsigned char MENU_getMenuSize(const ST_MENUPOINT * menu) {
	unsigned char i;
	for (i = 0; i < MENU_MAX_MENU_ITEMS && menu[i].menuTitle; i++)
		;			// Count the items with title not NULL
	return i;
}

//------------------------------------------------------------------------------
void MENU_gtoDefaultMenu(void) {
	if (!gucAdminMode) {
		currentMenuNumber = 0;
		currentMenuLevel = 0;
		LcdErase();
		(currentMenu[currentMenuNumber].menuHandlerCallback)(0xFF);	//<- clear button		//( buttonPressed);  //<--- must be state function with key checking
		menuState = 2;
		op_access_level = 1;
	} else {
		MENU_init(rootMenu);
	}
}

//----------------------------  Menu Handler -----------------------------------
// * This function handles the events which comes from keyboard.
// * Before using this function, you should initialize the menu system. 

void MENU_handler(unsigned char buttonPressed) {
	char handler_result;

	//  ToDo for warnins - must save screen and menu state, then go to warnings state.
	//  After warning accepted - restore screen and state
	if (gucWarnings)
		menuState = 3;

	switch (menuState) {
	case 0:		// Login
		//------------------ Select menu Item ----------------
	case 1:		// Select

		if (buttonPressed == KEY_DN) {
			if ((selectedMenuItem < (currentMenuSize - 1))
					&& (op_access_level > currentMenu[selectedMenuItem + 1].access_level)) {
				if (selectedMenuItem - currentMenuOffset > MENU_MAX_MENU_ITEMS_LCD - 2) {
					// TODO: screen scroll
					currentMenuOffset++;
					//selectedMenuItem++;
					MENU_printMenu();
				}

				if (selectedMenuItem != -1)
					MENU_printMenuItem(selectedMenuItem, 0);
				selectedMenuItem++;
				MENU_printMenuItem(selectedMenuItem, 1);
			}

		} else if (buttonPressed == KEY_UP) {

			if (selectedMenuItem > 0) {
				if (currentMenuOffset > 0 && selectedMenuItem - currentMenuOffset == 0) {
					// TODO: screen scroll
					currentMenuOffset--;
					//selectedMenuItem--;
					MENU_printMenu();
				}

				MENU_printMenuItem(selectedMenuItem, 0);
				selectedMenuItem--;
				MENU_printMenuItem(selectedMenuItem, 1);
			}
		}

		if ((buttonPressed == KEY_TL) && (selectedMenuItem != -1)) {
			//---------  Selected menu == Button pressed ----------
			currentMenuNumber = selectedMenuItem;
			if ((currentMenuNumber < currentMenuSize) && ((op_access_level > currentMenu[currentMenuNumber].access_level))) {
				if (currentMenu[currentMenuNumber].menuHandlerCallback) {
					// Call callback function
					LcdErase();
					if ((currentMenu[currentMenuNumber].menuHandlerCallback)(0xFE)) { //<- clear button		//( buttonPressed);  //<--- must be state function with key checking
						menuState = 1;
						MENU_printMenu();
						MENU_printMenuItem(selectedMenuItem, 1);

					} else
						menuState = 2;
				} else if (currentMenu[currentMenuNumber].psubMenu) { // Enter in Sub menu If exists
					// Enter to submenu
					if (currentMenuLevel < MENU_MAX_LEVEL) {
						parentMenu[currentMenuLevel] = currentMenu;
						currentMenuLevel += 1;
						parentMenuTitle[currentMenuLevel] = currentMenu[currentMenuNumber].menuTitle;
						currentMenu = currentMenu[currentMenuNumber].psubMenu;
						currentMenuNumber = 0;
						selectedMenuItem = -1;
						currentMenuOffset = 0;
						currentMenuSize = MENU_getMenuSize(currentMenu);
						MENU_printMenu();
					} else {
						LcdTextXY(0, 0, NORMAL_FNT, NORM_LINE, (char*) "Too many menu!");
						//UART_printf ("%s(): internal error: too many menu levels!\r\n");
					}
				}
			}
		}
		if (buttonPressed == KEY_BACK) {
			//--------------- Go  Back ---------------
			// Leave submenu
			if (currentMenuLevel) {
				currentMenuLevel--;
				selectedMenuItem = -1;
				currentMenuOffset = 0;
				currentMenu = parentMenu[currentMenuLevel];
				currentMenuSize = MENU_getMenuSize(currentMenu);
				menuState = 1;
				MENU_printMenu();
			}
			else {
				//ROM_SysCtlReset();
				HAL_NVIC_SystemReset();
			}
		}

		break;
		// --------- Menu Item handler --------------
	case 2:
		// CallBack function
		if (handler_result = (currentMenu[currentMenuNumber].menuHandlerCallback)(buttonPressed)) {
			if ((currentMenu[currentMenuNumber].psubMenu) && (handler_result == 1)) {
				//  Modal call - if result of handler ==1 then go to sub menu
				parentMenu[currentMenuLevel] = currentMenu;
				currentMenuLevel += 1;
				parentMenuTitle[currentMenuLevel] = currentMenu[currentMenuNumber].menuTitle;
				currentMenu = currentMenu[currentMenuNumber].psubMenu;
				currentMenuNumber = 0;
				selectedMenuItem = -1;
				currentMenuSize = MENU_getMenuSize(currentMenu);
				MENU_printMenu();
				menuState = 1;
			} else {
				menuState = 1;
				MENU_printMenu();	// Restore the last menu, after finished callback function
				if (selectedMenuItem != -1)
					MENU_printMenuItem(selectedMenuItem, 1);
			}

		}

		break;
		//-------------- Non handeled warnings ----------------
	case 3:
		selectedMenuItem = gucWarnings;

		if (menuWarnings[selectedMenuItem].menuHandlerCallback) {
			//--------------  Callback function --------
			if ((menuWarnings[selectedMenuItem].menuHandlerCallback)(0xFF)) {		// Cal the handler of the event
				if (gucDefaultMenu) {
					MENU_gtoDefaultMenu();
				} else if (op_access_level > 0) {
					menuState = 1;
					selectedMenuItem = -1;
					MENU_printMenu();
				} else {
					// Warning is called after boot and has no logged operator yet
					MENU_init(rootMenu);
					menuState = 0;
				}
				gucWarnings = W_NOWARNING;
				gucEventFlag = 0;			// Clear flag - event handler is completed
			} else {
				menuState = 4;
			}
		} else {
			if (MessageBox(gucWarnings, buttonPressed)) {

				LcdErase();

				// ---------- No callback function - only warning ---------------
				if (gucDefaultMenu) {
					MENU_gtoDefaultMenu();
				} else if (op_access_level < 1) {
					MENU_init(rootMenu);
					menuState = 0;
				} else if (op_access_level > 0) {
					menuState = 1;
					selectedMenuItem = -1;
					currentMenuOffset = 0;
					MENU_printMenu();	// Restore the last menu, after finished callback function
				} else {
					// Warning is called after boot and has no logged operator yet
					MENU_init(rootMenu);
					menuState = 0;
				}
				selectedMenuItem = -1;
				gucEventFlag = 0;			// Clear flag - event handler is completed
			}
			gucWarnings = W_NOWARNING;
		}

		break;
		// ------------- Warnings handler -------------------
	case 4:
		if ((menuWarnings[selectedMenuItem].menuHandlerCallback)(buttonPressed)) {
			if (gucDefaultMenu) {
				MENU_gtoDefaultMenu();
			} else if (op_access_level < 1) {
				MENU_init(rootMenu);
				menuState = 0;
			} else {
				menuState = 1;
				selectedMenuItem = -1;
				currentMenuOffset = 0;
				MENU_printMenu();	// Restore the last menu, after finished callback function

			}
			gucEventFlag = 0;			// Clear flag - event handler is completed
			gucWarnings = W_NOWARNING;
		}
		break;
	}

}

