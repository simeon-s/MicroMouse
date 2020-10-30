
//#define TARGET_IS_TEMPEST_RC1

#include <stdint.h>
#include <stdbool.h>

#include "lcd_SPI_drv.h"


//#include "oneshot_timer.h"
#include "main.h"		// for Systick timer
#include "common.h"


/* command function equates for S6B0724 LCD Display Controller */

#define LCD_DISP_OFF 		0xAE	/* turn LCD panel OFF */
#define LCD_DISP_ON			0xAF	/* turn LCD panel ON */
#define LCD_SET_LINE		0x40	/* set line for COM0 (6 lsbs = ST5:ST4:ST3:ST2:ST1:ST0) */
#define LCD_SET_PAGE		0xB0	/* set page address (4 lsbs = P3:P2:P1:P0) */
#define LCD_SET_COL_HI		0x10	/* set column address (4 lsbs = Y7:Y6:Y5:Y4) */
#define LCD_SET_COL_LO		0x00	/* set column address (4 lsbs = Y3:Y2:Y1:Y0) */
#define LCD_SET_ADC_NOR		0xA0	/* ADC set for normal direction */
#define LCD_SET_ADC_REV		0xA1	/* ADC set for reverse direction */
#define LCD_DISP_NOR		0xA6	/* normal pixel display */
#define LCD_DISP_REV		0xA7	/* reverse pixel display */
#define LCD_EON_OFF			0xA4	/* normal display mode */
#define LCD_EON_ON			0xA5	/* entire dsplay on */

#define LCD_SET_MODIFY		0xE0	/* set modfy read mode */
#define LCD_CLR_MODIFY		0xEE	/* clear modify read mode */
#define LCD_RESET			0xE2	/* soft reset command */
#define LCD_SET_SHL_NOR		0xC0	/* set COM direction normal */
#define LCD_SET_SHL_REV		0xC8	/* set COM direction reverse ( remapped) */

#define LCD_REF_VOLT_REG	0x81

#define LCD_CHARGE_EN		0x8D

#define LCD_ST_IND_MODE_0	0xAC	/* set static indicator mode to 0 */
#define LCD_ST_IND_MODE_1	0xAD	/* set static indicator mode to 1 */
#define LCD_ST_IND_REG		0x00	/* set the static indicator register (2 lsbs = S1:S0) */
#define LCD_NOP				0xE3	/* LCD controller NOP command */
#define LCD_TEST_CMD_1		0xF0	/* LCD Test Instruction 1.. do not use */
#define LCD_TEST_CMD_2		0x90	/* LCD Test Instruction 2.. do not use */

#define LCD_SET_COL			0x80
#define LCD_SET_PG			0x40




/* the LCD display image memory                        */
/* buffer arranged so page memory is sequential in RAM */
//static unsigned char xdata lcd_display_array[Y_BYTES][X_BYTES];
unsigned char lcd_display_array[Y_BYTES][X_BYTES];

/*  Golbal display variables   */
unsigned char   Y_lo;	// low row where are made changes
unsigned char   Y_hi;	// hi row where are nade chages 
unsigned char  UpdateLcd;			// display update need flag



/* 
**
** low level routine to send a byte value out the SPI
** to the LCD controller data register. entry argument
** is the data to output.
**
**	po_DC_DP - control line = 1 - data register, 0 - control register
*/


void LcdOutDat(char ub_Data) {
	HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, SET);	// select register for data port
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&ub_Data, 1, 10);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, SET);
} 

/* 
**
** low level routine to send a byte value out the serial bus
** to the LCD controller control register. entry argument is
** the command to output.
**
*/

void LcdOutCtl(char ub_Data) {
	HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, RESET);	// select register for command port
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&ub_Data, 1, 10);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, SET);
} 




 
/*--------------------------------------------------------------------------------------------------

  Name         :  LcdInit

  Description  :  Performs MCU SPI & LCD controller initialization.

  Argument(s)  :  None.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/

void LcdInit(void)
{
//	int i;


	/* initialize the port control lines to the LCD module */
//	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, SET);				/* set RST signal high off output */
	HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, RESET);				/* set the CD line low as output */
	Delay_us(10);

	/* reset the LCD controller chip */
//	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, RESET);			/* set the reset line low */
//	for(i=0; i<1000; i++) ;			/* delay for the reset time */
	gulTickCountDown = 10; while (gulTickCountDown);
//	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, SET);					/* release reset to back high */
	gulTickCountDown = 10; while (gulTickCountDown);


	LcdOutCtl( 0xA8);	/* */
	LcdOutCtl( 0x3F);	/*  */


	/* program the controller operational state */
	//LcdOutCtl(LCD_SET_ADC_NOR);	/* set ADC reverse */
	//LcdOutCtl(LCD_SET_SHL_NOR);	/* set SHL normal */

	LcdOutCtl(LCD_SET_ADC_REV);	/* set ADC reverse */
	LcdOutCtl(LCD_SET_SHL_REV);	/* set SHL normal */

	gulTickCountDown = 1000; while (gulTickCountDown);


	LcdOutCtl(LCD_REF_VOLT_REG);	/* set default reference voltage select */
	LcdOutCtl(CONTRAST);
//	for(i=0; i<1000; i++);			 	/* delay for power stabilize */
	
	LcdOutCtl(LCD_CHARGE_EN);
	LcdOutCtl(0x14);

	gulTickCountDown = 1000; while (gulTickCountDown);
	
	LcdOutCtl(LCD_DISP_ON);			/* put the display on */

	LcdOutCtl(LCD_SET_LINE+0);		/* set line for row 0 of display */

	LcdOutCtl(LCD_SET_PAGE+0);		/* set page 0 */
	LcdOutCtl(LCD_SET_COL_HI+0);	/* set column 0 */
	LcdOutCtl(LCD_SET_COL_LO+0);
} 


/*--------------------------------------------------------------------------------------------------

  Name         :  LcdContrast

  Description  :  Set display contrast.

  Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.

  Return value :  None.

  Notes        :  No change visible at ambient temperature.

--------------------------------------------------------------------------------------------------*/
void LcdContrast ( unsigned char contrast )
{
  	LcdOutCtl(LCD_REF_VOLT_REG);		 			// prime for the reference voltage
	LcdOutCtl(contrast);			// set default reference voltage select

}


/* ---------------------------------------------------------------------S 
**
** routine to display a test pattern on the LCD screen,
**
*/

const unsigned char testpat[4][8]={
				   {0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0},
				   {0xF0,0xF0,0xF0,0xF0,0x0F,0x0F,0x0F,0x0F},
	               {0xFF,0x81,0xBD,0xBD,0xBD,0xBD,0x81,0xFF},
				   {0x00,0x7E,0x42,0x42,0x42,0x42,0x7E,0x00}
				  };


void LcdTest(unsigned char pattern)	//fills all the display with pattern
{
	unsigned char p;
	unsigned char i;

	for(p=0; p<8; p++)
	{
		LcdOutCtl(LCD_SET_PAGE + p);		//
		LcdOutCtl(LCD_SET_COL_HI+0);	// set column 0 
		LcdOutCtl(LCD_SET_COL_LO+0);		// set column 
		for(i=0; i<X_BYTES; i++)
		{
			LcdOutDat(testpat[pattern][i%8]);
		}
	}
}



//--------------------- Display OFF --------------------
void LcdTurnOFF(void) {
	LcdOutCtl(LCD_DISP_OFF);			/* put the display off */	
}



/* -----------------------------------------------------
**
** Updates area of the display. Writes data from display 
** RAM to the lcd display controller.
** 
** Arguments Used:                                      
**    top     top line of area to update.         
**    bottom  bottom line of area to update.
** 
**  for all screen - lcd_update(SCRN_TOP,SCRN_BOTTOM);
*/

void LcdUpdate( void)
{
 	unsigned char x;
	unsigned char y;
	unsigned char *colptr;
	
	if ( UpdateLcd ) {
		for(y = Y_lo; y <= Y_hi; y++)
		{
			LcdOutCtl(LCD_SET_PAGE+y);	/* set page */
			LcdOutCtl(LCD_SET_COL_HI+0);	/* set column 0 */
			LcdOutCtl(LCD_SET_COL_LO);
	
			colptr = &lcd_display_array[y][0];
			for (x=0; x < X_BYTES; x++)
			{
				LcdOutDat( (*colptr++));
			}
		}
	}
	Y_lo = Y_BYTES;
	Y_hi = 0;
    UpdateLcd = 0;				//FALSE;
}





/**************************************************************/


#define	SYSCLK 		64000000
#define TIM_1US		((SYSCLK + 500000) / 1000000)


/*========================================
	Implementation of Public Functions
========================================*/
// Cycles:
//	call:	3 oder 4
//	djnz:	2/3
//	ret:	5
//	
void Delay_1us(void){
	register unsigned char i;
	i=((TIM_1US - 8) + 1) / 3;
	do{
		i--;
	}while(i);
}

void Delay_us(unsigned short us){
	do{
		Delay_1us();
	}while(--us);
}


void Delay_ms(unsigned short us){
	do{
		Delay_us(1000);
	}while(--us);
}

