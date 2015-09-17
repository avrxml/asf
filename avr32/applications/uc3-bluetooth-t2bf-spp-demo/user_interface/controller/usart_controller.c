/**************************************************************************
 *
 * \file
 *
 * \brief Controller for USB CDC with Xplained board.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "controller.h"

/* The keys in keyboard to control the demo */
#define KEY_w 0x57
#define KEY_W 0x77
#define KEY_d 0x44
#define KEY_D 0x64
#define KEY_a 0x41
#define KEY_A 0x61
#define KEY_s 0x53
#define KEY_S 0x73

/*! flag set when key inputs. */
bool c_inquiry, c_select, c_connect, c_send;
static bool any_key_input = false;

void check_key_input()
{
	any_key_input = true;
}

/*!
 * \brief function to configure output.
 */
void controller_init(U32 fcpu_hz, U32 fhsb_hz, U32 fpbb_hz, U32 fpba_hz)
{

}

/*! \brief Refresh the controller.
 */
void controller_task()
{
	uint8_t ch;
	c_inquiry = false;
	c_select  = false;
	c_connect = false;
	c_send    = false;
        if(any_key_input){
		ch = getchar();   // get one input character
        	any_key_input = false;

		if((ch==KEY_w)||(ch==KEY_W)) {
			c_inquiry = true;
 		}
		else if ((ch==KEY_d)||(ch==KEY_D)) {
			c_select = true;
		}
		else if ((ch==KEY_a)||(ch==KEY_A)) {
			c_connect = true;
		}
		else if ((ch==KEY_s)||(ch==KEY_S)) {
                 	 c_send = true;
		}
        }
}

/* Check the input request for inquiry */
bool controller_inquiry(void)
{
	return     c_inquiry;
}
/* Check the input request for selecting */
bool controller_select(void)
{
	return     c_select;
}
/* Check the input request for connecting */
bool controller_connect(void)
{
	return     c_connect;
}
/* Check the input request for sending */
bool controller_send(void)
{
	return     c_send;
}
