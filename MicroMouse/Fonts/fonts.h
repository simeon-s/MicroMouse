/********************************************************************************
	NAME 		: FONT.H
	
	EXTENDED NAME	: Fonts for graphic LCD based on KS0108 or HD61202
	
	LAYER		: Application
				
********************************************************************************/

/*******************************************************************************/
/* 			     DECLARATIONS / DEFINITIONS			       */
/*******************************************************************************/

#ifndef __FONTS_AND_IMAGES__
#define __FONTS_AND_IMAGES__


//      Font types  
#define	NORMAL_FNT		1
#define	BOLD_FNT		2
#define BIG_DIGITS		4

#define BIG_FNT			3
#define	PRINTER_FNT		3
#define	PRINTER_DBL		4


//		Font style definitions - bit mask for style variable
#define NORMAL			0x00
#define NORM_LINE		0x01
#define INVERSE			0x02
#define CENTERED		0x04
#define INV_LINE		0x08
#define RIGHT_ALIGN		0x10
//#define INV_LINE		8


/* EXTERN Function Prototype(s) */

/* Extern definitions */

typedef struct FONT_DEF 
{
	unsigned char charWidth;     		/* Character width for storage         */
	unsigned char charHeight;  			/* Character height for storage        */
	const unsigned char *FontTable;		/* Font table start address in memory  */
} FONT_DEF;

extern const FONT_DEF Font_1;
extern const FONT_DEF Font_2;
extern const FONT_DEF Font_3;
extern const FONT_DEF Font_4;

#define ASCII_BOT 32		


#ifndef FONTGEN_ITYPES
#define FONTGEN_ITYPES
typedef struct IMG_DEF {
   unsigned short width;     /* Image width */
   unsigned char height;    /* Image height*/
   unsigned char const *char_table;         /* Image table start address in memory  */
} IMG_DEF;
#endif


/* Global variables */

extern const unsigned char FontSystem5x8[];
extern const unsigned char FontSystem7x8[];
extern const unsigned char PrinterFontSystem10x11[];
extern const unsigned char BigDigitsFontSystem10x12[];



#endif

