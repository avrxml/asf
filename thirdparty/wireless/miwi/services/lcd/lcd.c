/**
* \file  lcd.c
*
* \brief LCD Abstraction for MiWi Protocol implementation
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#include "system.h"
#include "lcd.h"
#include "delay.h"
#include "port.h"
#include "board.h"
#include "asf.h"

#if defined (ENABLE_LCD)

#include "gfx_mono.h"
#include "gfx_mono_spinctrl.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// LCDText is a 32 byte shadow of the LCD text.  Write to it and 
// then call LCDUpdate() to copy the string into the LCD module.
char LCDText[16*4+1];
      
/*********************************************************************
* Function:         void LCD_Initialize(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is configured
*
* Overview:         Configure the LCD
*
* Note:             None
********************************************************************/
void LCD_Initialize(void)
{
	gfx_mono_init();
}    
/*********************************************************************
* Function:         void LCD_Erase(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD display cleared
*
* Overview:         Clears LCD display
*
* Note:             None
********************************************************************/
void LCD_Erase(void)
{
    /* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT,
	GFX_PIXEL_CLR);
}
/*********************************************************************
* Function:         void LCD_Update(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is updated with message
*
* Overview:         LCD displays message
*
* Note:             None
********************************************************************/
void LCD_Update(void)
{
	gfx_mono_draw_string(LCDText, 0, 0, &sysfont);
}

/*********************************************************************
 * Function:        void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           txCount - the total number of transmitted messages
 *                  rxCount - the total number of received messages
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the total numbers of TX and
 *                  RX messages on the LCD, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
{
	/* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT, 
	GFX_PIXEL_CLR);
    snprintf(LCDText, sizeof(LCDText), "TX Messages: %d \nRX Messages: %d", txCount, rxCount);
    gfx_mono_draw_string(LCDText, 0, 0, &sysfont); 
	delay_ms(200);
}



/*********************************************************************
 * Function:        void LCDDisplay(char *text, uint8_t value, bool delay)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           text - text message to be displayed on LCD
 *                  value - the text message allows up to one byte
 *                          of variable in the message
 *                  delay - whether need to display the message for
 *                          2 second without change
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the text message on the LCD,
 *                  including up to one uint8_t variable, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDDisplay(char *text, uint8_t value, bool delay)
{
	/* Clear screen */	
	gfx_mono_draw_filled_rect(0, 0, GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT,
			GFX_PIXEL_CLR);    

    snprintf(LCDText, sizeof(LCDText), text);

    if (value)
    {
		uint8_t char_count = 0;
		do {
           ++char_count;
		} while (*(++text));
		
	    snprintf(LCDText+char_count, sizeof(LCDText), "%d", value);
	}
	    
	gfx_mono_draw_string(LCDText, 0, 0, &sysfont);
	
	if(delay)
	{
	  delay_s(2);
	}
}

#endif