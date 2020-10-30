#ifndef MENU_FUNCTIONS_H_
#define MENU_FUNCTIONS_H_

//#include "../Drivers/ctime.h"
//#include "../Fonts/Icons.h"


//----------------------------------------------------------------------------------------------
//   Menu functions 
// 	Used with menu items structure - struct ST_MENUPOINT  defined ine menu.h
//	included in menuitems.c
//  there should be defined callback functions foe menues
//		nsigned char (*menuHandlerCallback) (unsigned char * arg );
//
//	author: Simeon Simeonov
//	copyright: bitElectronics L.t.d.
//	created: 24/10/2010
//
//-----------------------------------------------------------------------------------------------

#define NONMODAL	0
#define MODAL		1	


//------------------Properties of the Editboxes  ----------------
typedef struct  {
	unsigned char x_pos;
	unsigned char y_pos;
	unsigned char ucIntSize;
	unsigned char ucFractSize;
	unsigned char ucFont;
	unsigned char ucInit;
	char * pucValue;
	char * pucTitle;
	char **pucSelect ;
	char ucOptions;
	char ucKbdTable;
        unsigned int    uiMaxValue;
        unsigned int uiMinValue;
} S_EDITBOX;

//------------ Global definitions ---------------
extern S_EDITBOX property;

// ----------- Function prototypes --------------

unsigned char EditValue(unsigned char kbd_key, S_EDITBOX *property);		// pucInput - must be converted to pointer to Long
unsigned char EditString(unsigned char kbd_key, S_EDITBOX *property);		// pucInput - must be pointer to string
unsigned char DateBox(  unsigned char kbd_key, S_EDITBOX *property );
unsigned char DateTimeBox (unsigned char kbd_key);
unsigned char InputValue (unsigned char kbd_key,  S_EDITBOX *property);
unsigned char InputPassword (unsigned char kbd_key,  S_EDITBOX *property);
//unsigned char WarningBox(unsigned char kbd_key,  char* pucText, IMG_DEF* icon );
unsigned char MessageBox ( unsigned char warning_nr, unsigned char key);
void TextBox( char top, char left, char width, char font_type, char * text );

//----------- For testings -----------
unsigned char inputPrice( unsigned char kbd_key);
unsigned char inputQuantity( unsigned char kbd_key);
unsigned char inputPercent( unsigned char kbd_key);
unsigned char inputString( unsigned char kbd_key);
unsigned char SelectBox (unsigned char kbd_key,  S_EDITBOX *property);
unsigned char inputTime( unsigned char kbd_key) ;
unsigned char inputDate( unsigned char kbd_key) ;
unsigned char Login( unsigned char kbd_key);

void DrawBar(long pos, long max_size, long cell_count, long linepos );
void ClearBar(long linepos); 

#endif /*MENU_FUNCTIONS_H_*/
