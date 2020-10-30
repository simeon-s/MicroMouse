

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
//
//	All functions must be state machines with input keyboard key
//	and result :
//	0 - working - keep the current state
//	1 - ready with no errors
//	2..10 - error type
////
//
//
//
//-------------------------- USB keyboard events ---------------------------
//
// enum E_KEYBOARD_EVENTS {
// 	E_NULL	 = 0,
//	E_KEYPRESSED,
//	E_LINEGET,
//	E_BACKSPACE,
//	E_LEFT_ARROW,
//	E_RIGHT_ARROW,
//	E_UP_ARROW,
//	E_DOWN_ARROW,
//	E_F1_KEY,
//	E_KBD_CONNECTED,
//	E_KBD_DICSONNECTED,
//	E_UNKNOWNDEVICE
//};
//extern enum E_KEYBOARD_EVENTS g_eUSBKbdEvents;
//	g_eUSBKbdEvents = E_NULL;
//
//-----------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include <Rtcc.h>

#include "../Libraries/KBD_scan.h"
#include "../Libraries/Glcd.h"
#include "../Libraries/lcd_SPI_drv.h"
#include "../Language/menu_items.h"

#include "main.h"
#include "menu_functions.h"


//---------------------- Global definitions ----------------

S_EDITBOX property;

//---------------------------------------------------
#define ST_INIT		0
#define ST_INPUT	1
#define ST_EDIT		2

static char edit_state = ST_INIT;
static char edit_buff[40]; // edit buff is common for all input end edit functions and must be static
static unsigned char input_ptr;
//static unsigned char tmp_edit_buff[16];

//----------------------------------------------------------------------
//          EditValue function - Edit or input digital value
//   parameters:
//		S_EDITBOX
//	Return:
//		- uunsigned char return code
//			- 0 (false)  - working
//			- 1 (true)	 - Success - the input value is correct
//			- 2 (false)  - not sucsess or ESC(MOD ) key pressed
//----------------------------------------------------------------------

unsigned char EditValue(unsigned char kbd_key, S_EDITBOX *property) {
    unsigned int digit;
    char format_str[6];

    if (property->ucInit)
        edit_state = ST_INIT;

    //digit = *property->pucValue + (*(property->pucValue + 1) << 8);
    digit = *(unsigned int*)property->pucValue;
    switch (edit_state) {
        case ST_INIT:
            if (property->ucFractSize == 0)
                property->ucFractSize = 1;

            if (!property->ucFont || property->ucFont > BIG_FNT) {
                property->ucFont = NORMAL_FNT;
            }
            LcdBox(2, 4, 125, 60);
            LcdTextXY(0, 0, NORMAL_FNT, CENTERED, property->pucTitle);
            property->ucInit = 0;
            //input_ptr = 0;
            edit_state = ST_EDIT;
            break;
        case ST_EDIT:
            if (kbd_key == KEY_UP) {
                if (digit < property->uiMaxValue)
                    digit += property->ucFractSize;
            }

            if (kbd_key == KEY_DN) {
                if (digit > property->uiMinValue)
                    digit -= property->ucFractSize;
            }

            break;
    }
    // return new value in the buffer
    //*property->pucValue = digit;
    //*(property->pucValue + 1) = digit >> 8;
    *(unsigned int*)property->pucValue = digit;

    // --- Display the input string ---
    sprintf(format_str, "%%0%dd ", property->ucIntSize);
    sprintf((char*) edit_buff, format_str, digit);
    LcdTextXY(property->x_pos, property->y_pos, property->ucFont, INVERSE, edit_buff);
    LcdUpdate();

    //---------- Accept changes -------------
    if (kbd_key == KEY_TL) {
        edit_state = ST_INIT;
        return 1;
    }

    //------- Go back - exit without changes -------
    if (kbd_key == KEY_BACK) {
        edit_state = ST_INIT;
        return 2; // ESC  - go back without new valu in the buffer
    }

    return 0;
}




//------------------------------------------------------------------------------
//          Input Value  function - Digital input from keyboard
//			without displayng the value but *
//   parameters:
//		S_EDITBOX
//                char ucOptions = 0 - input digit
//                char ucOptions = 1 - input password
//	Return:
//		- uunsigned char return code
//			- 0 (false)  - working
//			- 1 (true)	 - Success - the input value is correct
//			- 2 (false)  - not sucsess or ESC(MOD ) key pressed
//		- Result is LONG in the S_EDIT
//-----------------------------------------------------------------------------

unsigned char InputValue(unsigned char kbd_key, S_EDITBOX *property) {
    unsigned char curr_ptr;
    unsigned short digit;
    char * str_ptr;

    if (property->ucInit)
        edit_state = ST_INIT;

    switch (edit_state) {
        case ST_INIT:
            if (property->ucFractSize == 0)
                property->ucFractSize = 1;

            sprintf((char*) edit_buff, "%d", *(short*) &property->pucValue);
            if (edit_buff[0]) edit_buff[0] = edit_buff[0] - '0';
            if (edit_buff[0]) edit_buff[1] = edit_buff[1] - '0';
            if (edit_buff[0]) edit_buff[2] = edit_buff[2] - '0';
            if (edit_buff[0]) edit_buff[3] = edit_buff[3] - '0';

            //*property->pucValue = 0;
            property->ucInit = 0;
            input_ptr = 0;
            edit_state = ST_INPUT;

            if (property->ucOptions == 1) {
                sprintf((char*) property->pucValue, "****");
                edit_buff[0] = 0;
                edit_buff[1] = 0;
                edit_buff[2] = 0;
                edit_buff[3] = 0;
            } else {

            }
            break;
        case ST_INPUT:

            if (kbd_key == KEY_TL) {
                if (property->ucOptions == 1)
                    *(property->pucValue + input_ptr) = '*';
                input_ptr++;
                //edit_buff[input_ptr] = 0;
                //*(property->pucValue + input_ptr) = 0;
            }

            if (kbd_key == KEY_UP) {
                if (edit_buff[input_ptr] < 9) {
                    edit_buff[input_ptr] += property->ucFractSize;
                    *(property->pucValue + input_ptr) = edit_buff[input_ptr] + '0';
                } //else
                //sound(1800, 300);
            }

            if (kbd_key == KEY_DN) {
                if (edit_buff[input_ptr] > 0) {
                    edit_buff[input_ptr] -= property->ucFractSize;
                    *(property->pucValue + input_ptr) = edit_buff[input_ptr] + '0';
                } //else
                //sound(1800, 300);
            }

            if (kbd_key == KEY_BACK) {
                if (input_ptr > 0) {
                    if (property->ucOptions == 1)
                        *(property->pucValue + input_ptr) = '*';
                    input_ptr--;
                    //edit_buff[input_ptr] = 0;
                    // *(property->pucValue + input_ptr) = 0;

                } else {
                    //sound(2150, 300);
                    digit = 0;
                    edit_state = ST_INIT;
                    return 2;
                }
            }
            break;
    }

    // --- Display the input string ---
    str_ptr = property->pucValue;
    LcdTextXY(property->x_pos, property->y_pos, NORMAL_FNT, NORMAL, str_ptr);
    LcdGotoXY((property->x_pos + input_ptr)*6, property->y_pos);
    LcdChr(NORMAL_FNT, INVERSE, *(property->pucValue + input_ptr)); // make the cursor
    //LcdChr(NORMAL_FNT, NORMAL, ' '); // Clear next character
    LcdUpdate();

    //---------- Accept changes -------------
    if ((kbd_key == KEY_TL) && (input_ptr == property->ucIntSize)) {
        curr_ptr = 0;
        digit = 0;
        while (curr_ptr < property->ucIntSize) {
            digit = digit * 10;
            digit += edit_buff[curr_ptr];
            curr_ptr++;
        }
        // return new value in the buffer
        *(unsigned short*) property->pucValue = digit;

        edit_state = ST_INIT;
        return 1;
    }
    return 0;
}





//------------------------------------------------------------------------------
//          SelectBox function - Selects from string values
//   parameters:
//		S_EDITBOX
//	Return:
//		- uunsigned char return code
//	
//-----------------------------------------------------------------------------
#define DISPLAY_OPTIONS		6 

unsigned char SelectBox(unsigned char kbd_key, S_EDITBOX *property) {
    static char selected;
    char option_nr, start_option, max_option;
    char y;
    long pos;


    if (kbd_key == 0xFF)
        edit_state = ST_INIT;

    switch (edit_state) {
        case ST_INIT:

            selected = *property->pucValue; // make inverse selected value
            edit_state = ST_EDIT;
            break;

        case ST_INPUT:
        case ST_EDIT:
            if ((kbd_key == KEY_DN)) {
                if (selected < property->ucOptions - 1)
                    selected++;

            } else if ((kbd_key == KEY_UP)) {
                if (selected > 0)
                    selected--;

            } else if (kbd_key < 10) {
                //sound(2150, 200);
            }

            break;
    }

    if ((property->ucOptions > DISPLAY_OPTIONS)) {
        if (selected < property->ucOptions)
            max_option = selected + 1;
        else
            max_option = selected;
        if (max_option >= DISPLAY_OPTIONS)
            start_option = max_option - DISPLAY_OPTIONS;
        else {
            start_option = 0;
            max_option = DISPLAY_OPTIONS;
        }

    } else {
        max_option = property->ucOptions;
        start_option = 0;
    }
    LcdErase();
    LcdBox(2, 4, 125, 60);
    LcdTextXY(0, 0, NORMAL_FNT, CENTERED, property->pucTitle);

    y = 0;
    for (option_nr = start_option; option_nr < max_option; option_nr++) {
        sprintf((char*) edit_buff, " %s ", property->pucSelect[option_nr]);
        edit_buff[20] = 0;
        if (selected == option_nr)
            LcdTextXY(property->x_pos, property->y_pos + y, NORMAL_FNT, INVERSE, edit_buff);
        else
            LcdTextXY(property->x_pos, property->y_pos + y, NORMAL_FNT, NORMAL, edit_buff);

        y++;
    }

    if (property->ucOptions > DISPLAY_OPTIONS) {
        pos = selected * 16;
        pos /= property->ucOptions;
        LcdScrollBar(pos);
    }

    LcdUpdate();


    //---------- Accept changes -------------
    if ((kbd_key == KEY_TL)) {
        *property->pucValue = selected;
        edit_state = ST_INIT;
        return 1;
    }

    //------- Go back - exit without changes -------
    if (kbd_key == KEY_BACK) {
        edit_state = ST_INIT;
        return 2;
    }

    return 0;

}

/*****************************************************************************
 **		Function   MessagetBox
 **
 **   function MessageBox displayes messages on the display
 **
 **   Argumebts:
 **      type:   MODAL , NONMODAL. If MODAL with the message wil be diplaied ESC and OK
 **              and user can select with buttons "ESC  and "OK"
 **      message - message number described in menu.h
 **		key - keyboard ley pressed
 **
 **      returns 1 if pressed OK , 2 - ESC(CL),  0 - no key pressed
 **
 *******************************************************************************/

unsigned char MessageBox(unsigned char warning_nr, unsigned char key) {
    static unsigned char mb_state = ST_INIT;
    char msg[16];
    const char * message;
    unsigned char idx;
    unsigned char row;
    char result;

    result = 0;
    if (key == 0xFE)
        mb_state = ST_INIT;

    switch (mb_state) {
        case ST_INIT:
            // for debug only
            LcdErase();
            row = 4;
            message = menuWarnings[warning_nr].menuTitle;
            do {
                idx = 0;
                while ((*message != 0x00) && (idx < 14)) {
                    if (*message == '\n') {
                        message++;
                        break;
                    }
                    msg[idx] = *message;
                    message++;
                    idx++;
                }
                msg[idx] = 0;
                LcdTextXY(0, row, BOLD_FNT, CENTERED, msg);
                row++;

            } while (*message != 0x00);

            //LcdIcon(52, 1, NORMAL, (FONT_DEF*)&icons, ')'); // "!" sign
            LcdBox(2, 2, SCRN_RIGHT - 2, SCRN_BOTTOM - 2);

            LcdUpdate();
            mb_state = ST_INPUT;
            //sound(2150, 300);
            break;

        case ST_INPUT:
            //-------- Nothing there, wait for CL ----------
            break;
    }


    if (key == KEY_TL) {
        mb_state = ST_INIT;
        result = 1;
    }

    if  (key == KEY_BACK) { //||  ( g_eUSBKbdEvents == E_ESC_KEY ) ) {
        mb_state = ST_INIT;
        result = 2;
    }

    return result;
}

/***********************************************************************
 * 
 * 
 * 
 * 
 ***********************************************************************/

void TextBox(char top, char left, char width, char font_type, char * text) {
    char  msg[20];
    unsigned char idx;
    unsigned char row;

    LcdErase();
    LcdBox(2, 4, 125, 60);
    //LcdBox(2,2,SCRN_RIGHT-2 , SCRN_BOTTOM-2);

    row = top;
    do {
        idx = 0;
        while ((*text != 0x00) && (idx < width)) {
            if (*text == '\n') {
                text++;
                break;
            }
            msg[idx] = *text;
            text++;
            idx++;
        }
        msg[idx] = 0;
        LcdTextXY(1, row, font_type, CENTERED, msg);
        row++;

    } while ((*text != 0x00) && (row < 8));
    LcdUpdate();
}




//-------------------------------------------------------------
// 	Braw bar Function
//	requires current position, max size/elements, count of displays' 
//	chars and line where bar will be drawn
//--------------------------------------------------------------

void DrawBar(long pos, long max_size, long cell_count, long linepos) {
    div_t fractpart;
    long step;

    fractpart = div(pos * cell_count, max_size);

    for (step = 0; step < fractpart.quot; step++)
        LcdTextXY(step + ((21 - cell_count) / 2), linepos, NORMAL_FNT, INVERSE, ( char*) " ");

    LcdUpdate();
}

void ClearBar(long linepos) {
    LcdTextXY(1, linepos, NORMAL_FNT, NORMAL, ( char*) "                  ");
    LcdUpdate();
}

