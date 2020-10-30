#ifndef _SPI_DRV_H
#define _SPI_DRV_H


#define SSD1815B		//Tian-ma 132x64 Graphics module with onboard SSD1815 controller
//#define PCD8544			// Nokia 3310 display with PCD8544 Philips comtroler 	

//#define SOFT_SPI		// Software SPI for lcd communication


/*****************************************************************
		LC Display is on SPI0  of the controller and SPI0
		have to be configure  in config.c 
******************************************************************/



#define CONTRAST			0x7F


#ifdef SSD1815B

/* LCD screen and bitmap image array consants */
//#define X_BYTES				132
#define X_BYTES				128
#define Y_BYTES	      		8
#define SCRN_LEFT			0
#define SCRN_TOP			0
//#define SCRN_RIGHT			131
#define SCRN_RIGHT			127
#define SCRN_BOTTOM			63


#else 

/* LCD screen and bitmap image array consants */
#define X_BYTES				84
#define Y_BYTES	      		6
#define SCRN_LEFT			0
#define SCRN_TOP			0
#define SCRN_RIGHT			83
#define SCRN_BOTTOM			47

#endif



/* the LCD display image memory                        */
/* buffer arranged so page memory is sequential in RAM */
//static unsigned char  lcd_display_array[Y_BYTES][X_BYTES];
extern unsigned char  lcd_display_array[Y_BYTES][X_BYTES];

/*  Golbal display variables   */
extern unsigned char   Y_lo;	// low row where are made changes
extern unsigned char   Y_hi;	// hi row where are nade chages 
extern unsigned char  UpdateLcd;			// display update need flag



void LcdInit(void);
void LcdOutDat(char dat);
void LcdOutCtl(char dat);
void LcdUpdate( void);
void LcdContrast ( unsigned char contrast );
void LcdTest(unsigned char pattern);
void LcdTurnOFF(void);

void Delay_1us ( void );
void Delay_us(unsigned short us);
void Delay_ms(unsigned short us);



#endif
