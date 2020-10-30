/**************************************************************************
 *                                                                         *
 *	 FILE: LCD132x64.C                                                     *
 *   LCD Display Controller Interface Routines for use with Tian-ma        *
 *   132x64 Graphics module with onboard SSD1815 controller                *
 *                                                                         *
 *   Copyright (C) Techproject 2004                                        *
 *                                                                         *
 *									Written by:                            *
 *									Simeon Simeonov                        *
 *									                                       *
 *									                                       *
 *                                                                         *
 *   0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ                                  *    
 *	 abcdefghijklmnopqrstuvwxyz? ():.                                      *
 *	 ���������������������������������                                     *
 *	 �������������������������������                                       *
 *                                                                         * 
 ***************************************************************************/

//#pragma CODE

#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include "lcd_SPI_drv.h"
//#include ".Fonts/fonts.h"
#include "Glcd.h"


/* pixel level bit masks for display       */
/* this array is setup to map the order    */
/* of bits in a byte to the vertical order */
/* of bits at the LCD controller */
const unsigned char l_mask_array[8] =  { 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//const unsigned char const l_mask_array[8] = { 0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};


static unsigned char X_pos;
static unsigned char Y_pos;


//extern unsigned char xdata lcd_display_array[Y_BYTES][X_BYTES];
/* 
**
** routine to erase the LCD screen, This erases whole
** display memory of the S6B0724 LCD controller.
**
*/

void LcdErase(void)
{
	//memset( lcd_display_array,0xFF,sizeof(lcd_display_array));
	memset( lcd_display_array,0x00,sizeof(lcd_display_array));

    //  Reset current position.
	X_pos = 0;
	Y_pos = 0;

	Y_lo = 0;			// set update area to full screen;
	Y_hi = Y_BYTES -1;
    UpdateLcd = TRUE;
	LcdUpdate();
}





/***************************************************************
      X_pos - x position in points;
      Y_pos - y position in rows
**************************************************************/

void LcdGotoXY(unsigned char xpos, unsigned char ypos) {

	Y_pos = ypos;
	X_pos = xpos;

}


/*--------------------------------------------------------------------------------------------------

  Name         :  LcdChr

  Description  :  Displays a character at current cursor location and increment cursor location.

  Argument(s)  :  size -> Font size. See enum.
                  ch   -> Character to write.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/

void LcdChr ( unsigned char font_type, unsigned char style, unsigned char ch ) {

unsigned char glyph;
const unsigned char *glyph_ptr;
const unsigned char *font;
unsigned char font_width;
unsigned char font_height;
unsigned char column,y;						//column of grf display in pixels
unsigned char i;
	


	switch (font_type) {
		case 1:
			font_width = Font_1.charWidth;
			font_height = Font_1.charHeight;
			font = Font_1.FontTable;
		break;
		case 2:
			font_width = Font_2.charWidth;
			font_height = Font_2.charHeight;
			font = Font_2.FontTable;
		break;
		case 3:
			font_width = Font_3.charWidth;
			font_height = Font_3.charHeight;
			font = Font_3.FontTable;
		break;
		case 4:
			font_width = Font_4.charWidth;
			font_height = Font_4.charHeight;
			font = Font_4.FontTable;
		break;
	}

	column = X_pos;
	y = Y_pos; 
	
	if ( Y_lo > y) 
		Y_lo = y;
	
	glyph = ch;
	glyph -= ASCII_BOT;
	if ( font_height < 9)
		glyph_ptr = font + (glyph * font_width);		// font width = 6, 7x5 + 1 pixel interval
	else
		glyph_ptr = font + (glyph * font_width *2);		// font width  * 2 rows
	if ( ch >= ASCII_BOT ) {							// character 
		for (i=0; i<font_width; i++)
		{
			if ( style &(INVERSE | INV_LINE) ) {		// Inverse{
				lcd_display_array[y][column] = ~(*glyph_ptr);
				if (font_height >8) {
					glyph_ptr++;	
					lcd_display_array[y+1][column] = ~(*glyph_ptr);
				}   
			} else {  
				lcd_display_array[y][column] = *glyph_ptr;
				if (font_height >8) {
					glyph_ptr++;	
					lcd_display_array[y+1][column] = *glyph_ptr;
				}  
			}		
			glyph_ptr++;						// point to next column of the glyph
			column++;
		}
		if ( style & (INVERSE | INV_LINE) ) {   	// one pixel interval
			lcd_display_array[y][column] = 0xFF;
			if (font_height >8) {
				lcd_display_array[y+1][column] = 0xFF;	
			}
		} else { 		
			lcd_display_array[y][column] = 0x00;
			if (font_height >8) {
				lcd_display_array[y+1][column] = 0x00;	
			}
		}
	}
	column++;
	X_pos = column;
	Y_pos = y;
	
	if (font_height >8)
		y++;
	
	if ( Y_hi < y ) {
		Y_hi = y;
	}
	UpdateLcd = TRUE;
}






void LcdTextXY( unsigned char x, unsigned char y, unsigned char font_type, unsigned char style, char *str){
unsigned char glyph;
unsigned char const *glyph_ptr;
const unsigned char *font;
unsigned char font_width;
unsigned char font_height;
unsigned char column;				//, l_columns;						//column of grf display in pixels
unsigned char i;

	switch (font_type) {
		case 1:
			font_width = Font_1.charWidth;
			font_height = Font_1.charHeight;
			font = Font_1.FontTable;
		break;
		case 2:
			font_width = Font_2.charWidth;
			font_height = Font_2.charHeight;
			font = Font_2.FontTable;
		break;
		case 3:
			font_width = Font_3.charWidth;
			font_height = Font_3.charHeight;
			font = Font_3.FontTable;
		break;
		case 4:
			font_width = Font_4.charWidth;
			font_height = Font_4.charHeight;
			font = Font_4.FontTable;
		break;
	}
	
	//l_columns = X_BYTES/(font_width+1);
	if ( style & CENTERED )	
		column = (X_BYTES - (strlen(str)*(font_width+1))) /2;
	 else 
		column = x * (font_width+1);
		
	// If centered or inverse line - fill the beggining 		
	for (i=0; i<column; i++) {
		if ( style &(  INV_LINE) )	{
			lcd_display_array[y][i] = 0xFF;
			if (font_height >8) {
				lcd_display_array[y+1][i] = 0xFF;	
			}
		} 
		if ( style &(  NORM_LINE) ) {
			lcd_display_array[y][i] = 0x00;
			if (font_height >8) {
				lcd_display_array[y+1][i] = 0x00;	
			}
		}
	}
	
	if ( Y_lo > y) 
		Y_lo = y;
//	LcdOutCtl(LCD_SET_PG+y);				// set row
//	LcdOutCtl(LCD_SET_COL+column);			// set column
	while(*str != 0x00)	{
		glyph = (unsigned char)*str;
		glyph -= ASCII_BOT;
		if (font_height < 9)
			glyph_ptr = font + (glyph * font_width);		// font width = 6, 7x5 + 1 pixel interval
		else
			glyph_ptr = font + (glyph * font_width *2);		// font width  * 2 rows
			
		if ( *str >= ASCII_BOT ) {							// character 
			for (i=0; i<font_width; i++)
			{
				if ( style &(INVERSE | INV_LINE) ) {		// Inverse{
					lcd_display_array[y][column] = ~(*glyph_ptr);
					if (font_height >8) {
						glyph_ptr++;	
						lcd_display_array[y+1][column] = ~(*glyph_ptr);
					}   
				} else {  
					lcd_display_array[y][column] = *glyph_ptr;
					if (font_height >8) {
						glyph_ptr++;	
						lcd_display_array[y+1][column] = *glyph_ptr;
					}  
				}		
				glyph_ptr++;						// point to next column of the glyph
				column++;
			}
			if ( style & (INVERSE | INV_LINE) ) {   	// one pixel interval
				lcd_display_array[y][column] = 0xFF;
				if (font_height >8) {
					lcd_display_array[y+1][column] = 0xFF;	
				}
			} else { 		
				lcd_display_array[y][column] = 0x00;
				if (font_height >8) {
					lcd_display_array[y+1][column] = 0x00;	
				}
			}
		}
		column++;
		str++;										// point to next character in string
	}
	if (style & INV_LINE ){
		while (column <X_BYTES) {
			lcd_display_array[y][column] = 0xFF;
			if (font_height >8) {
				lcd_display_array[y+1][column] = 0xFF;	
			}
			column++;
		}
	} else if (style & NORM_LINE) {
		 while (column <X_BYTES) {
			lcd_display_array[y][column] = 0x00;
			if (font_height >8) {
				lcd_display_array[y+1][column] = 0x00;	
			}
			column++;
		}
	}
	
	X_pos = column;
	Y_pos = y;
	
	if ( font_height >8 )
			y++;
	
	if ( Y_hi < y ) {
		Y_hi = y;
	}
	
	UpdateLcd = TRUE;
}




void LcdPrint(unsigned char font_type, unsigned char style, unsigned char *str) {

	while(*str != 0x00)	{ 
		LcdChr ( font_type, style, *str );
		str++;
	}
}





/*-------------------------------------------------------------------------------
Draw a dot on the LCD
	LcdSetDoc (U8 u8Xaxis, U8 u8Yaxis)
		u8Xaxis = absciss
		u8Yaxis = ordinate
-------------------------------------------------------------------------------*/	
void LcdSetDot (unsigned char x_axis, unsigned char y_axis) {
	unsigned char bit_pos;
	unsigned char byte_offset;
	
	bit_pos = y_axis & 0x07;			/* get the bit offset into a byte */
  	byte_offset = y_axis >> 3;		    /* get the byte offset into x array */
	
	//lcd_display_array[byte_offset][x_axis] &= l_mask_array[bit_pos]; 	/* get the mask for this bit */
	lcd_display_array[byte_offset][x_axis] |= l_mask_array[bit_pos]; 	/* get the mask for this bit */

	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	UpdateLcd = TRUE;
}




/*
**
** Draws a line into display  memory starting at the X1, Y1
** going to the X2, Y2n. No runtime error checking is performed.
**
*/

void LcdLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char doted) {
	signed char   stepx, stepy;
	int  dx, dy, fraction;

	if (x2 > x1) { 
		dx = x2 - x1;
		stepx = 1;
	} else {
		dx = x1 - x2;
		stepx = -1;
	}
	if ( y2 > y1) {
		dy = y2 - y1;
	 	stepy = 1;
	} else {
	   dy = y1 - y2;
	 	stepy = -1;
	}
    
    dx <<= 1;
    dy <<= 1;

    LcdSetDot( x1, y1 );

    if ( dx > dy ) {
        fraction = dy - (dx >> 1);
        while ( x1 != x2 ) {
            if ( fraction >= 0 ) {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
			if ( ! (x1 & doted)) 
            	LcdSetDot( x1, y1);
        }
    } else {
        fraction = dx - (dy >> 1);
        while ( y1 != y2 ) {
            if ( fraction >= 0 ){
                x1 += stepx;
                fraction -= dy;
            } 
            y1 += stepy;
            fraction += dx;
			if ( !(y1 & doted) )
            	LcdSetDot( x1, y1 );
        }
    }

}




/*
**
** Draws a line into the display memory starting at left going to
** right, on the given row. No runtime error checking is performed.  
** It is assumed that left is less than right.
** dot_mask is a mask ,used for making dashed lines. 
** if dot_mask = 2,  2 pixels interva, 4 - for pixels interal.
**
*/

void LcdHLine(unsigned char left, unsigned char right,
		           unsigned char row, unsigned char dot_mask)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char mask;
	unsigned char col;
	
	bit_pos = row & 0x07;			/* get the bit offset into a byte */
  	byte_offset = row >> 3;		    /* get the byte offset into x array */
	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */
	
	for(col = left; col <= right; col++)\
	{
		if ( !(col & dot_mask) ) 
			//lcd_display_array[byte_offset][col] &= mask;
			lcd_display_array[byte_offset][col] |= mask;
	}
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	UpdateLcd = TRUE;
}


/*
**
** Draws a vertical line into display memory starting at the top
** going to the bottom in the given column. No runtime error checking 
** is performed. It is assumed that top is less than bottom and that 
** the column is in range.
** dot_mask is a bit mask ,used for making dashed lines. 
**
*/

void LcdVLine(unsigned char top, unsigned char bottom,
		           unsigned char column, unsigned char dot_mask)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;		   /* get starting bit offset into byte */
	byte_offset = top >> 3;		   /* get byte offset into y direction */
	y_bits = (bottom - top) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	while(y_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (y_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(y_bits > 7)			/* while there are at least 8 more bits to do */
			{
				if ( dot_mask)
					//lcd_display_array[byte_offset][column] = dot_mask; 
					lcd_display_array[byte_offset][column] = dot_mask;  
				else 	
					//lcd_display_array[byte_offset][column] = 0x00;
					lcd_display_array[byte_offset][column] = 0xFF;
				byte_offset++;
				y_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
			/* do each individual bit   */
			
			if ( !(remaining_bits & dot_mask ))
				//lcd_display_array[byte_offset][column] &= mask;
				lcd_display_array[byte_offset][column] |= mask;
			mask <<= 1;
			//mask |= 0x01;
			 mask &= 0xFE;

			y_bits--;
			remaining_bits--;
			if(remaining_bits == 0)
			{
				/* might have bust gotton byte aligned */
				/* so reset for beginning of a byte */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
			}
		}
	}
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	UpdateLcd = TRUE;
	
}


/*
**
** Clears a line into the display memory starting at left going to
** right, on the given row. No runtime error checking is performed.  
** It is assumed that left is less than right.
**
*/

void LcdClrHLine(unsigned char left, unsigned char right,
		               unsigned char row)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char mask;
	unsigned char col;
	
	bit_pos = row & 0x07;			/* get the bit offset into a byte */
	byte_offset = row >> 3;		    /* get the byte offset into x array */
	mask = l_mask_array[bit_pos]; 	/* get the mask for this bit */
	for(col = left; col <= right; col++)
	{
		lcd_display_array[byte_offset][col] &= ~mask;
	}
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	UpdateLcd = TRUE;
}
	

/*
**
** Clears a vertical line into display memory starting at the top
** going to the bottom in the given column. No runtime error checking 
** is performed. It is assumed that top is less than bottom and that 
** the column is in range.
**
*/

void LcdClrVLine(unsigned char top, unsigned char bottom,
		               unsigned char column)
{
	unsigned char bit_pos;
	unsigned char byte_offset;
	unsigned char y_bits;
	unsigned char remaining_bits;
	unsigned char mask;

	bit_pos = top & 0x07;		   /* get starting bit offset into byte */

	byte_offset = top >> 3;		   /* get byte offset into y direction */
	y_bits = (bottom - top) + 1;   /* get length in the x direction to write */
	remaining_bits = 8 - bit_pos;  /* number of bits left in byte */
	mask = l_mask_array[bit_pos];  /* get mask for this bit */
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	while(y_bits)				   /* while there are still bits to write */
	{
		if((remaining_bits == 8) && (y_bits > 7))
		{
			/* here if we are byte aligned and have at least 1 byte to write */
			/* do the entire byte at once instead of bit by bit */
			while(y_bits > 7)			/* while there are at least 8 more bits to do */
			{
				lcd_display_array[byte_offset][column] = 0x00;
				byte_offset++;
				y_bits -= 8;
			}
		}
		else
		{
      		/* we are not byte aligned or an entire byte does not need written */
			/* do each individual bit                                          */
			lcd_display_array[byte_offset][column] &= ~mask;
			if(l_mask_array[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			y_bits--;
			remaining_bits--;
			if(remaining_bits == 0)
			{
				/* might have bust gotton byte aligned */
				/* so reset for beginning of a byte */
				remaining_bits = 8;
				byte_offset++;
				mask = l_mask_array[0];
			}
		}
	}
	if ( Y_hi < byte_offset ) 
		Y_hi = byte_offset;
	if ( Y_lo > byte_offset) 
		Y_lo = byte_offset;
	UpdateLcd = TRUE;
}


/*
**
** 	Draws a box in display memory starting at the left/top and going
**  to the right/bottom. No runtime error checking is performed.
**  It is assumed that left is less than right and that top is less 
**  than bottom.
** 
*/

void LcdBox(unsigned char left, unsigned char top,
			 unsigned char right, unsigned char bottom)
{
	/* to draw a box requires two vertical lines */
	LcdVLine(top,bottom,left,0);
	LcdVLine(top,bottom,right,0);
	//LcdVLine(top+1,bottom+1,right+1,0);
	
	/* and two horizonal lines */
	LcdHLine(left,right,top,0);
	LcdHLine(left,right,bottom,0);
	//LcdHLine(left+1,right+1,bottom+1,0);
}

/*
**
** Clears a box in display memory starting at the Top left and going
** to the bottom right. No runtime error checking is performed and
** it is assumed that Left is less than Right and that Top is less 
** than Bottom.
**
*/

void LcdClrBox(unsigned char left, unsigned char top,
             unsigned char right, unsigned char bottom)
{
	/* to undraw the box undraw the two vertical lines */
	LcdClrVLine(top,bottom,left);
	LcdClrVLine(top,bottom,right);
	
	/* and the two horizonal lines that comprise it */
	LcdClrHLine(left,right,top);
	LcdClrHLine(left,right,bottom);
}



/******************************************************************
				ScrollBar
	At the right position  of the display.
	5 pixels width, whole display height
	pos - relevant position
	max pos value = display rows * 2  ( Y_BYTES * 2 )

*******************************************************************/

const unsigned char arrows[] = {
	0xF4, 0x16, 0x17, 0x16, 0xF4,
	0xF4, 0xF6, 0xF7, 0xF6, 0xF4,
	0xFF, 0x00, 0x00, 0x00, 0xFF,
	0x2F, 0x68, 0xE8, 0x68, 0x2F,
	0x2F, 0x6F, 0xEF, 0x6F, 0x2F
};


void LcdScrollBar (unsigned char pos) {
unsigned char i,j;

	Y_hi = Y_BYTES - 1;
	Y_lo = 0;
	
	for (i=0; i < Y_BYTES; i++) {
		switch (i) {
		case 0:
			for (j = 0; j<5; j++) {
				if ( (pos>>1) != 0) 
					lcd_display_array[i][X_BYTES -5+ j] = arrows[j];
				else 
					lcd_display_array[i][X_BYTES -5+ j] = arrows[5 + j];	
			}
		break;
		case Y_BYTES -1:
			for (j = 0; j<5; j++) {
				if ( (pos>>1) != Y_BYTES -1)
					lcd_display_array[i][X_BYTES -5+ j] = arrows[15+j];
				else 
					lcd_display_array[i][X_BYTES -5+ j] = arrows[20+j];	
			}
		break;
		default:
			for (j = 0; j<5; j++) {
				lcd_display_array[i][X_BYTES -5+ j] = arrows[10+j];
				if (i == (pos>>1)) {
					if ( pos & 1) {
						lcd_display_array[i][X_BYTES -5+ j] |= 0xF0;
					}else {	
						lcd_display_array[i][X_BYTES -5+ j] |= 0x0F; 
					}
				}
			}		
		break;
		}
	}
	UpdateLcd = TRUE;
}



//-------------------------------------------------------------------------------
//                 GM signal quality
//-------------------------------------------------------------------------------
const unsigned char antenna[] = {0x01,0x02,0x7F,0x02,0x01};
const unsigned char sig_quality[]= { 0x60,0x70,0x78,0x7C,0x7E,};

//---------
void LcdAntena(unsigned char row, unsigned char column, unsigned char signal) {
unsigned char i;
unsigned char j;
	for (i=0; i<5; i++) {
		lcd_display_array[row][column+i] |= antenna[i];
	}
	
/*	if (signal >6)
		signal-=6;
	else
	 signal = 0;
*/
	i = column + 4;
	j =0;
	while (signal > 6) {
		lcd_display_array[row][i] |= sig_quality[j];
		lcd_display_array[row][i+1] |= sig_quality[j];
		i +=3;
		j++;
		signal -=6;
	}
}

/*-------------------------------------------------------------------------------
Draw a circle on the LCD
	GLCD_Circle (U8 u8CenterX, U8 u8CenterY, U8 u8Radius)
		u8CenterX = Center absciss (in pixels)
		u8CenterY = Center ordinate (in pixels) 
		u8Radius  = Radius (in pixels)
-------------------------------------------------------------------------------*/
void LcdCircle (unsigned char centerX, unsigned char centerY, unsigned char radius) {
	int tswitch=0;
	unsigned char y=0, x=0;
	unsigned char u8d;

	u8d = centerY - centerX;
	y = radius;
	tswitch = 3 - 2 * radius;
	while (x <= y) 
	{
		LcdSetDot(centerX + x, centerY + y);
		LcdSetDot(centerX + x, centerY - y);
		
		LcdSetDot(centerX - x, centerY + y);
		LcdSetDot(centerX - x, centerY - y);
		
		LcdSetDot(centerY + y - u8d, centerY + x);
		LcdSetDot(centerY + y - u8d, centerY - x);
		LcdSetDot(centerY - y - u8d, centerY + x); 
		LcdSetDot(centerY - y - u8d, centerY - x);

		if (tswitch < 0) 
			tswitch += (4 * x + 6);
		else 
		{
			tswitch += (4 * (x - y) + 10);
			y--;
		}
	
		x++;
	}
}


/********************************************************************
	Puts icon on the display
	Icon dimensions - 16 x 16 pixels
	style - normal or inverse  (0 - 1);

**********************************************************************/
/*
void LcdIcon( unsigned char x, unsigned char y, unsigned char icon) {
unsigned char i;
const unsigned char *pic;

	if ( Y_hi < (y + 1) ) 
		Y_hi = y+1;
	if ( Y_lo > y) 
		Y_lo = y;
	
	pic = icons +(icon*32);
	for (i=0; i<16; i++) {
		lcd_display_array[y][x+i] = *(pic+i);
		//lcd_display_array[y][x+i] = ~(*(pic+i));
	}
	y++;
	for (i=0; i<16; i++) {
		lcd_display_array[y][x+i] = *(pic+i+16);
		//lcd_display_array[y][x+i] = ~(*(pic+i+16));
	}	
	Y_pos = y;
	X_pos = x+i+1;
	Y_hi = y;
	UpdateLcd = TRUE;
}
		*/



void LcdBitmap( unsigned char x_pos, unsigned char y_pos,IMG_DEF  * bitmap) {
unsigned long i,j;
unsigned char rows_nr;
unsigned char const * pixel;

	if ( Y_hi < (y_pos + 1) ) 
		Y_hi = y_pos+1;
	if ( Y_lo > y_pos) 
		Y_lo = y_pos;

	
	rows_nr = bitmap->height / 8;
	pixel = (unsigned char *)bitmap->char_table;
	//rows_nr = 5;
	
	for (i=x_pos; i< bitmap->width + x_pos; i++) {
	//for (i=x_pos; i< ( 60+ x_pos ); i++) {
		for ( j=rows_nr; j >0; j--) {
			//prn_buff[j-1][i] = *(bitmap->char_table);
			//(bitmap->char_table)++;
			lcd_display_array[j-1 + y_pos][i] = *pixel;
			pixel++;
		}
	}
	
	
	Y_pos = y_pos ;
	X_pos = x_pos+i+1;
	Y_hi = y_pos  + rows_nr;
	if (Y_hi >= Y_BYTES)
		 Y_hi = Y_BYTES-1;
	UpdateLcd = 1;
}
		





/*	lcd_invert_row - function , used in menu function
**	and realise menu pointer to menu item
**  Thats is useful when selecting, without redrow wholl menu
**	invert curernt row, and row where marker go ( UP or DOWN )
**
**	Arguments Used:  
**		row		the row wich pixels,  be inverted
*/

void LcdInvertRow( unsigned char row ){
unsigned char i;
	for (i=0; i<X_BYTES; i++)
		lcd_display_array[row][i] ^= 0xFF;
	if ( Y_hi < row ) 
		Y_hi = row;
	if ( Y_lo > row) 
		Y_lo = row;
	UpdateLcd = TRUE;
}

/* ----------------------------------------------------------- */

void LcdInvertArea(unsigned char left, unsigned char top,
			 		unsigned char right, unsigned char bottom){
unsigned char i;
unsigned char j;
	for (j=top; j<=bottom; j++) {
		for (i=left; i<=right; i++)  	
			lcd_display_array[j][i] ^= 0xFF;
	}
	
	if ( Y_hi < bottom ) 
		Y_hi = bottom;
	if ( Y_lo > top) 
		Y_lo = top;
	LcdUpdate();
	UpdateLcd = TRUE;
}



/* -------------------------------------------- */
/*          Invert Screen 	                    */


void LcdInvertScr( void ){
unsigned char i,j;
	
	for (j=0; j<Y_BYTES; j++) {
		for (i=0; i<X_BYTES; i++)  	
			lcd_display_array[j][i] ^= 0xFF;
	}
	Y_lo = 0;			// set update area to full screen;
	Y_hi = Y_BYTES -1;
    UpdateLcd = TRUE;
	LcdUpdate();
}


//-----------------------------------------------


