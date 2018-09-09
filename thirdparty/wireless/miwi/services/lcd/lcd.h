/**
* \file  lcd.h
*
* \brief LCD Abstraction for MiWi Protocol interface
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

#ifndef __LCDBLOCKING_H
#define __LCDBLOCKING_H


extern char LCDText[16*4+1];
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
void LCD_Initialize(void);

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
void LCD_Update(void);

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
void LCD_Erase(void);

/*********************************************************************
* Function: void LCDDisplay( char * message, uint8_t value, bool delay  )
*
* Overview: Displays the message on the LCD
*
* PreCondition: None
*
* Input:  message, variable if needed and delay
*
* Output: Prints the message on the LCD
*
********************************************************************/
void LCDDisplay(char * message, uint8_t value, bool delay);

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
* Note:            
********************************************************************/
void LCDTRXCount(uint8_t, uint8_t);
    
#endif
