# CC3200-LCD-I2C
LCD Library for the  FC-133 module using the PCF8574T GPIO expander
This Library is partialy based in the fdebrabander Arduino-LiquidCrystal-I2C-library

# Usage
Just copy the i2c_lcd.h and i2c_lcd.c into your workspace and its done!

# Note
Keep in mind that most Lcd use a  5v supply, the CC3200 uses a 3.3v supply so a level shifter is required. 
Some info is avaliable at: http://www.nxp.com/documents/application_note/AN10441.pdf
I used the 2n7000 for my level shifter as shown in the document's diagram
![alt tag](https://raw.githubusercontent.com/Torr250/CC3200-LCD-I2C/master/cc3200%20lcd.jpg)
