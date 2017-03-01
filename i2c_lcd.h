/*
 * i2c_lcd.h
 *
 *  Created on: 26/02/2017
 *      Author: Adan Torralba
 *
 *
 *      PCF8574T pin diagram:
 *
 *      D7  - pin12	/P7
 *      D6  - pin11	/P6
 *      D5  - pin10	/P5
 *      D4  - pin9 	/P4
 *      LED - pin7	/P3
 *      E   - pin6	/P2
 *      RW  - pin5	/P1
 *      RS  - pin4	/P0
 *
 *      7 | 6 | 5 | 4 |  3  | 2 | 1  | 0  |
 *      D7| D6| D5| D4| LED | E | RW | RS |
 *
 */

#ifndef I2C_LCD_H_
#define I2C_LCD_H_
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif
//*****************************************************************************
// TSL256 Device I2C address
//*****************************************************************************
#define  LCDI2C_ADDRESS		0x27

//*****************************************************************************
// Commands
//*****************************************************************************
#define LCD_CLEARDISPLAY 	0x01
#define LCD_RETURNHOME 		0x02
#define LCD_ENTRYMODESET 	0x04
#define LCD_DISPLAYCONTROL 	0x08
#define LCD_CURSORSHIFT 	0x10
#define LCD_FUNCTIONSET 	0x20
#define LCD_SETCGRAMADDR 	0x40
#define LCD_SETDDRAMADDR 	0x80

//*****************************************************************************
// Flags for display entry mode
//*****************************************************************************
#define LCD_ENTRYRIGHT 	0x00
#define LCD_ENTRYLEFT 	0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

//*****************************************************************************
// Flags for display on/off control
//*****************************************************************************
#define LCD_DISPLAYON 	0x04
#define LCD_DISPLAYOFF 	0x00
#define LCD_CURSORON 	0x02
#define LCD_CURSOROFF 	0x00
#define LCD_BLINKON 	0x01
#define LCD_BLINKOFF	0x00

//*****************************************************************************
// Flags for display/cursor shift
//*****************************************************************************
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 	0x00
#define LCD_MOVERIGHT 	0x04
#define LCD_MOVELEFT 	0x00

//*****************************************************************************
// flags for function set
//*****************************************************************************
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 	 0x08
#define LCD_1LINE 	 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

//*****************************************************************************
// flags for control
//*****************************************************************************
#define LCD_BACKLIGHT 	0x08 // Enable bit b00000000
#define En 0x04  // Enable bit b00000100
#define Rw 0x02  // Read/Write bit b00000010
#define Rs 0x01  // Register select bit b00000001

//*****************************************************************************
// Send byte mode
//*****************************************************************************
#define COMMAND 0x00	//Send byte command mode
#define DATA 	0x01	//Send byte data mode

#define ENABLE 	0x01
#define DISABLE 0x00

//*****************************************************************************
// API Variables
//*****************************************************************************
unsigned char _backlightval;
unsigned char _cols;
unsigned char _rows;

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
	void Lcd_init(unsigned char cols, unsigned char rows);
/********** high level commands*/
	void Lcd_clear();
	void Lcd_home();
	void Lcd_entymode(unsigned char direction, unsigned char shiftdirection);
	void Lcd_displaycontrol(unsigned char display, unsigned char cursor, unsigned char blink);
	void Lcd_cursorshift(unsigned char move, unsigned char direction);
	void Lcd_gotoxy(unsigned char xCoor, unsigned char yCoor);
	void Lcd_createChar(unsigned char location, unsigned char charmap[]);
	void Lcd_backlight(unsigned char value);
	int  Lcd_Print(const char *pcFormat, ...);
	void Lcd_message(const char *str);

/************ low level data pushing commands **********/
	void Lcd_send_command(unsigned char value);
	void Lcd_send_byte(unsigned char value, unsigned char mode);
	int Lcd_WriteI2C(unsigned char _data);
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  __I2C_LCD_H___

