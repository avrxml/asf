/**************************************************************************
 *
 * \file
 *
 * \brief Controller for USB CDC with Xplained board.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
