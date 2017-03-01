/*
 * i2c_lcd.h
 *
 *  Created on: 26/02/2017
 *      Author: Adan Torralba
 */
//*****************************************************************************
//
//! @{
//
//*****************************************************************************
// Standard includes
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "i2c_if.h"
#include "i2c_lcd.h"
#include "rom_map.h"
#include "uart_if.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define FAILURE                 -1
#define SUCCESS                 0
#define RET_IF_ERR(Func)        {int iRetVal = (Func); \
		if (SUCCESS != iRetVal) \
		return  iRetVal;}
#define DBG_PRINT               Report
#define US_DELAY(x)				MAP_UtilsDelay(x * (80 / 5));
#define MS_DELAY(x)				MAP_UtilsDelay(x * (80000 / 5));
#define SEC_DELAY(x)			MAP_UtilsDelay(x * (80000000 / 5));

extern int
I2C_IF_Write(unsigned char ucDevAddr,
		unsigned char *pucData,
		unsigned char ucLen,
		unsigned char ucStop);

extern void MAP_UtilsDelay(unsigned long ulCount);
//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//****************************************************************************

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//

//****************************************************************************
//
//! Initialize the LCD
//!
//! \param cols is the number of cols of the display
//! \param rows is the number of cols of the display
//!
//! This function
//!    1. Initialize the LCD
//!
//
//****************************************************************************

void Lcd_init(unsigned char cols, unsigned char rows) {
	_cols = cols;
	_rows = rows;
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to data sheet, we need at least 40ms after power rises above 2.7V
	// before sending commands.
	MS_DELAY(200);

	//first
	Lcd_send_command(0x03);
	MS_DELAY(5);

	//second
	Lcd_send_command(0x03);
	US_DELAY(100);

	//third
	Lcd_send_command(0x03);

	//Function set
	Lcd_send_command(LCD_RETURNHOME);
	Lcd_send_byte(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS,COMMAND);
	Lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF,COMMAND);
	Lcd_send_byte(LCD_CLEARDISPLAY,COMMAND);
	//Entry mode set
	Lcd_send_byte(LCD_ENTRYMODESET|LCD_ENTRYLEFT|LCD_ENTRYSHIFTDECREMENT, COMMAND);
}

//****************************************************************************
//
//! Clear the lcd
//!
//! \param None
//!
//! This function
//!    1. Clears the lcd screen
//!
//****************************************************************************

void Lcd_clear() {
	Lcd_send_byte(LCD_CLEARDISPLAY,COMMAND); // clear display, set cursor position to zero
	US_DELAY(2000);  // this command takes a long time!
}

//****************************************************************************
//
//! Lcd home
//!
//! \param None
//!
//! This function
//!    1. Sets the lcd into the (0,0) position
//!
//****************************************************************************

void Lcd_home() {
	Lcd_send_byte(LCD_RETURNHOME,COMMAND);  // set cursor position to zero
	US_DELAY(2000);  // this command takes a long time!
}

//****************************************************************************
//
//! Lcd entymode
//!
//! \param direction Select the direction of the lcd writing
//! 		Flags: LCD_ENTRYRIGHT, LCD_ENTRYLEFT
//! \param shiftdirection Select the shifting direction of the lcd
//! 		Flags: LCD_ENTRYSHIFTINCREMENT, LCD_ENTRYSHIFTDECREMENT
//!
//! This function
//!    1. Sets the lcd Entry mode
//!
//****************************************************************************
void Lcd_entymode(unsigned char direction, unsigned char shiftdirection) {
	Lcd_send_byte(LCD_ENTRYMODESET|direction|shiftdirection,COMMAND);
	US_DELAY(2000);  // this command takes a long time!
}


//****************************************************************************
//
//! Lcd Display Control
//!
//! \param display Turns the display on or off
//! 		Flags: LCD_DISPLAYON, LCD_DISPLAYOFF
//! \param cursor Turns the cursor on or off
//! 		Flags: LCD_CURSORON, LCD_CURSOROFF
//! \param blink Turns the blink on or off
//! 		Flags: LCD_BLINKON, LCD_BLINKOFF
//! This function
//!    1. Controls the lcd display
//!
//****************************************************************************
void Lcd_displaycontrol(unsigned char display, unsigned char cursor, unsigned char blink) {
	Lcd_send_byte(LCD_DISPLAYCONTROL|display|cursor|blink,COMMAND);
	US_DELAY(2000);  // this command takes a long time!
}

//****************************************************************************
//
//! Lcd cursor shift
//!
//! \param move Move the display or the cursor
//! 		Flags: LCD_DISPLAYMOVE, LCD_CURSORMOVE
//! \param direction the direction of the shift
//! 		Flags: LCD_MOVERIGHT, LCD_MOVELEFT
//! This function
//!    1. Shifts the cursor or the display
//!
//****************************************************************************
void Lcd_cursorshift(unsigned char move, unsigned char direction) {
	Lcd_send_byte(LCD_CURSORSHIFT|move|direction,COMMAND);
	US_DELAY(2000);  // this command takes a long time!
}

//****************************************************************************
//
//! Lcd (x,y) position
//!
//!			**************************
//!			**(0,0)			  (15,0)**
//!			**(0,1)			  (15,1)**
//!			**************************
//!
//! \param xCoor: Horizontal coordinate, starts in 0
//! \param yCoor: Vertical coordinate, starts in 0
//! This function
//!    1. Sets the cursor in the desired coordinate
//!
//****************************************************************************
void Lcd_gotoxy(unsigned char xCoor, unsigned char yCoor) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (yCoor > _rows) {
		yCoor = _rows - 1;    // we count rows starting w/0
	}
	Lcd_send_byte(LCD_SETDDRAMADDR | (xCoor + row_offsets[yCoor]),COMMAND);
}

//****************************************************************************
//
//! Fill the first 8 CGRAM locations with custom characters
//!
//! Examples;
//!		unsigned char bell[8]	 = 	{0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
//!		unsigned char note[8]  	= 	{0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
//!		unsigned char clock[8] 	= 	{0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
//!		unsigned char heart[8] 	= 	{0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
//!		unsigned char duck[8]  	= 	{0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
//!		unsigned char check[8] 	= 	{0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
//!		unsigned char cross[8] 	= 	{0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
//!		unsigned char retarrow[8] = {0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};
//!
//!		Lcd_createChar(0, bell);
//!		Lcd_createChar(1, note); so on...
//!
//!		To print the character:
//!		Lcd_send_byte(0,DATA);	//Prints bell character
//!		Lcd_send_byte(1,DATA); 	//Prints note character and so on...
//!
//! \param location: Location of the character (0-7)
//! \param charmap[]: Map of the custom character
//! This function
//!    1. Sends the custom character to the lcd CGRAM
//!
//****************************************************************************
void Lcd_createChar(unsigned char location, unsigned char charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	Lcd_send_byte(LCD_SETCGRAMADDR | (location << 3),COMMAND);
	int i;
	for (i = 0; i < 8; i++) {
		Lcd_send_byte(charmap[i],DATA);
	}
}


//****************************************************************************
//
//! Sends char by char the data string
//!
//! \param str: string pointer
//! This function
//!    1. Sends char by char the data string,
//!		  stops until it finds the NULL character
//!
//****************************************************************************

void Lcd_message(const char *str)
{
    if(str != NULL)
    {
        while(*str!='\0')
        {
        	Lcd_send_byte(*str++,DATA);
        }
    }
}


//****************************************************************************
//
//! Print Lcd string
//!
//! \param format is a pointer to the character string specifying the format in
//!           the following arguments need to be interpreted.
//! \param [variable number of] arguments according to the format in the first
//!         parameters
//! This function
//!    1. Prints a printf style string into the lcd
//!
//****************************************************************************

int Lcd_Print(const char *pcFormat, ...)
{
	int iRet = 0;

	char *pcBuff, *pcTemp;
	int iSize = 256;

	va_list list;
	pcBuff = (char*)malloc(iSize);
	if(pcBuff == NULL)
	{
		return -1;
	}
	while(1)
	{
		va_start(list,pcFormat);
		iRet = vsnprintf(pcBuff,iSize,pcFormat,list);
		va_end(list);
		if(iRet > -1 && iRet < iSize)
		{
			break;
		}
		else
		{
			iSize*=2;
			if((pcTemp=realloc(pcBuff,iSize))==NULL)
			{
				Message("Could not reallocate memory\n\r");
				iRet = -1;
				break;
			}
			else
			{
				pcBuff=pcTemp;
			}

		}
	}

	Lcd_message(pcBuff);
	free(pcBuff);

	return iRet;
}

//****************************************************************************
//
//! Lcd backlight
//!
//! \param value: blacklight flag status
//! 		Flags: ENABLE, DISABLE
//!
//! This function
//!    1. Turns the lcd backlight on or off
//!
//****************************************************************************
void Lcd_backlight(unsigned char value) {
	if (value == ENABLE)
		_backlightval = LCD_BACKLIGHT;
	else
		_backlightval = 0;
	Lcd_WriteI2C(0);
}

//****************************************************************************
//
//! Send command
//!
//! \param value: Command to be sent
//!
//! This function
//!    1. Expands value into FC-113 pinout format
//!    2. Sends data to the PCF8574T
//!
//! \Note: This function is needed only in the initialization routine
//!		   main difference between Lcd_send_byte is that Lcd_send_command only
//!		   sends the low nibble.
//!
//****************************************************************************
void Lcd_send_command(unsigned char value) {
	unsigned char expand;
	expand = (value & 0x0F) <<4;
	// RW is set low, send value to D[7:4] and send RS value
	Lcd_WriteI2C(expand & ~Rw);
	//Set E to high
	Lcd_WriteI2C(expand & ~Rw | En);
	// enable pulse must be >230ns
	//US_DELAY(1);
	//Set E to low
	Lcd_WriteI2C(expand & ~Rw & ~En);
	// commands need > 1us to settle
	//set high RW
	Lcd_WriteI2C(expand & Rw & ~En);
	//US_DELAY(40);
}

//****************************************************************************
//
//! Send byte
//!
//! \param value: Command or data to be sent
//! \param mode: Select mode: COMMAND or DATA
//!
//! This function
//!    1. Separates value into high and low nibbles
//!    2. Expands the low nibble into FC-113 pinout format
//!    3. Sends expanded low nibble to the PCF8574T
//!    4. Expands the high nibble into FC-113 pinout format
//!    5. Sends expanded low nibble to the PCF8574T
//!
//****************************************************************************
void Lcd_send_byte(unsigned char value,unsigned char mode) {
	unsigned char expand[2];
	expand[1] = value & 0xF0; //high nibble
	expand[0] = (value & 0x0F) <<4; //low nibble

	int i = 2;
	do {
		// RW is set low, send value to D[7:4] and send RS value
		Lcd_WriteI2C(expand[i-1] & ~Rw | mode);
		//Set E to high
		Lcd_WriteI2C(expand[i-1] & ~Rw | En | mode);
		// enable pulse must be >230ns
		//US_DELAY(1);
		//Set E to low
		Lcd_WriteI2C(expand[i-1] & ~Rw & ~En | mode);
		// commands need > 1us to settle
		//set high RW
		Lcd_WriteI2C(expand[i-1] & Rw & ~En | mode);
		//US_DELAY(1);
		i--;
	}while(i > 0);
	//US_DELAY(40);
}

//****************************************************************************
//
//! Send i2c byte
//!
//! \param value: Data to be sent via i2c
//!
//! This function
//!    1. Saves the value and the backlight flag into a temp variable
//!    2. Delays the functor for approximate 120us
//!    3. Sends data to the PCF8574T
//!
//! \Note: the PCF8574T only needs the Address register, if acknowledged,
//!		   the Microcontrolled sends directly the data, no registers involved.
//!
//! \return i2c writing failure or success
//
//****************************************************************************
int Lcd_WriteI2C(unsigned char value) {
	unsigned char temp = (value | _backlightval);
	US_DELAY(120);
	if(I2C_IF_Write(LCDI2C_ADDRESS, &temp, 1, 1) == 0)
	{
		return SUCCESS;
	}
	else
	{
		DBG_PRINT("I2C write failed\n\r");
	}
    return FAILURE;
}
