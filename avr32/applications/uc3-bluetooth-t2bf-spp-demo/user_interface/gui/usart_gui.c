/*****************************************************************************
 *
 * \file
 *
 * \brief Usart User Interface.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdio.h>
#include "board.h"
#include "gui.h"
#include "gpio.h"
#include "delay.h"
//#include "et024006dhu.h"


/*
 *! flag set when joystick display starts to signal main function to clear this
 * display .
 */
unsigned short display = 0;

/*!
 * \brief function to display default message
 */
static void gui_print_default_message(void)
{
  // Display default message.
	printf("\x1B[5;1H");
	printf("            ATMEL AVR UC3 \r\n");
	printf("          Cybercom Bluetooth \r\n");
	printf("             SPP Lite Demo   \r\n");
	printf("           Operation command:    \r\n");
	printf("   W: Inquiry;            D: Select device;  \r\n");
	printf("   S: Connect/Disconnect; A: Send data \r\n");
	printf("                                           \r");
	printf("          ");
}

void gui_init(U32 fcpu_hz, U32 fhsb_hz, U32 fpbb_hz, U32 fpba_hz)
{

}

void gui_callback( TString textToDisplay )
{
	display = 1;
	gui_print_default_message();
	printf(textToDisplay);
}
