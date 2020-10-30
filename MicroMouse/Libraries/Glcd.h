#ifndef _GLCD_H
#define _GLCD_H

#define TRUE 	1
#define FALSE 	0

#include "../Fonts/fonts.h"


/* LCD function prototype list */
void LcdErase(void);
void LcdLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char doted);
void LcdHLine(unsigned char left, unsigned char right, unsigned char row, unsigned char dot_mask);
void LcdVLine(unsigned char top, unsigned char bottom, unsigned char column, unsigned char dot_mask);
void LcdClrHLine(unsigned char left, unsigned char right, unsigned char row);
void LcdClrVLine(unsigned char left, unsigned char right, unsigned char row);
void LcdBox(unsigned char left, unsigned char top, unsigned char right, unsigned char bottom);
void LcdClrBox(unsigned char left, unsigned char top, unsigned char right, unsigned char bottom);
void LcdGotoXY(unsigned char xpos, unsigned char ypos);
void LcdChr ( unsigned char font_type, unsigned char style, unsigned char ch );
void LcdPrint(unsigned char font_type, unsigned char style, unsigned char *str);
void LcdTextXY( unsigned char x, unsigned char y, unsigned char font_type, unsigned char style, char *str);
void LcdInvertRow( unsigned char row );
void LcdInvertArea(unsigned char left, unsigned char top,unsigned char right, unsigned char bottom);
void LcdInvertScr( void );
void LcdSetDot (unsigned char x_axis, unsigned char y_axis);
void LcdCircle (unsigned char centerX, unsigned char centerY, unsigned char radius);
void LcdIcon( unsigned char x, unsigned char y, unsigned char icon);
void LcdBitmap( unsigned char x_pos, unsigned char y_pos,IMG_DEF  * bitmap);
void LcdScrollBar (unsigned char pos);
void LcdAntena(unsigned char row, unsigned char column, unsigned char signal);
void LcdLeds (unsigned char rbLeds);

#endif
