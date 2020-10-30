/*
 * File:        menu.h
 * Purpose:     Menu functions header
 * Author : Simeon Simeonov
 * Created:     2007-05-19 
 * Last modify: 2007-10-21 
 */
#ifndef __INCLUDE_MENU_H
#define __INCLUDE_MENU_H


//#define MENU_MAX_MENU_ITEMS_LCD     LCD_HEIGHT / FONT_HEIGHT
#define MENU_MAX_MENU_ITEMS_LCD     6


//#define NULL		0

typedef struct ST_MENUPOINT ST_MENUPOINT;

struct ST_MENUPOINT  {

	char access_level;
    char* menuTitle;           //Title of menu to display.
     // Sub-menu to invoke if menu is selected. 
     // You should set only one option of subMenu and menuHandlerCallback!
    const ST_MENUPOINT * psubMenu;
     // Function to call if menu is selected.
     // You should set only one option of subMenu OR menuHandlerCallback!
    unsigned char (*menuHandlerCallback) (unsigned char );
};


/// Maximum level of menu depth
#define MENU_MAX_LEVEL          8
/// Maximum number of menu items on a level
//#define MENU_MAX_MENU_ITEMS     255
#define MENU_MAX_MENU_ITEMS     10

#define MENU_DEFAULT_X          0
#define MENU_DEFAULT_Y          2	// start from first line - at line 0 - navigation bar




//----------------Warnings definitions ----------------------------

#define W_NOWARNING 	 0


extern char gucEventFlag; 				// events flag
extern char gucAdminMode;				// Disable  low level acounts
extern char gucDefaultMenu;				// Flag for sales mode

//-------------- Global Variables --------------------------------
extern unsigned char op_access_level;
extern unsigned char menuState;
extern unsigned char gucWarnings;
extern const ST_MENUPOINT* currentMenu;
extern unsigned char currentMenuNumber;

extern char warning_timer;

//---------------- Function prototypes --------------------------- 
void MENU_init (const ST_MENUPOINT* rootMenu);
void MENU_printMenu ();
void MENU_gtoDefaultMenu(void);

void MENU_handler (unsigned char  buttonPressed);



#endif // __INCLUDE_MENU_H
