//*****************************************************************************
//	Created on: 26/02/2017
//  Author: Adan Torralba
//
// Application Name     - Lcd i2c example
// Application Overview - Demostrates the use of the i2c_lcd library
//
//*****************************************************************************


#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"

// App Includes
#include "pinmux.h"
#include "i2c_lcd.h"

// Common interface includes
#include "common.h"
#include "gpio_if.h"
#include "uart_if.h"
#include "i2c_if.h"

#define APPLICATION_VERSION     "1.0"
#define APP_NAME                "Lcd i2c example"
#define SEC_DELAY(x)			MAP_UtilsDelay(x * (80000000 / 5));


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

unsigned long CurrentLux;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************



/****************************************************************************/
/*                      LOCAL FUNCTION DEFINITIONS                          */
/****************************************************************************/


//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{
    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t     CC3200 %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//****************************************************************************
//
//!
//!
//! \param none
//! 
//! This function  
//!    1.
//!
//! \return None.
//
//****************************************************************************
void main()
{
    long	lRetVal = -1;
    int 	iLoopCnt;

    //
    // Board Initialisation
    //
    BoardInit();
    
    //
    // Configure the pinmux settings for the peripherals exercised
    //
    PinMuxConfig();    

    //
    // UART Init
    //
    InitTerm();

    DisplayBanner(APP_NAME);

    //
    // I2C Init
    //
    lRetVal = I2C_IF_Open(I2C_MASTER_MODE_STD);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    Lcd_init(16, 2);
    Lcd_backlight(ENABLE);
    Lcd_displaycontrol(LCD_DISPLAYON,LCD_CURSOROFF,LCD_BLINKOFF);
    Lcd_home();
    Lcd_clear();

    unsigned char check[8] 	= 	{0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
    Lcd_createChar(0, check);

    Lcd_gotoxy(0,0);
    Lcd_Print("CC3200 Lcd I2C ");
    Lcd_send_byte(0,DATA);
    Lcd_gotoxy(0,1);
    Lcd_Print("FC-113 PCF8574T");
    SEC_DELAY(2);

    Lcd_clear();

    Lcd_gotoxy(1,0);
    Lcd_Print("Counter: ");
    while(1)
    {
        for(iLoopCnt = 0; iLoopCnt < 50; iLoopCnt++)
        {
            Lcd_gotoxy(11,0);
        	Lcd_Print("%2d",iLoopCnt);
        	 SEC_DELAY(0.3);
        }
    }

}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
