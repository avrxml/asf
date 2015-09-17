/*****************************************************************************
 *
 * \file
 *
 * \brief Usart User Interface.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
