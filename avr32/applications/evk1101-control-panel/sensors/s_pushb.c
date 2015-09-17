/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel push buttons sensor module.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

//_____  I N C L U D E S ___________________________________________________
#include <string.h>
#include <stdio.h>

#include "compiler.h"
#include "s_pushb.h"
#include "gpio.h"
#include "FreeRTOS.h"


//_____ M A C R O S ________________________________________________________

/*! Pushbutton events. */
#define PUSHB_EVENT_PRESS      true
#define PUSHB_EVENT_RELEASE    false

#define SENSOR_PB1_CONFIG_FILE    "CFG/PB1.TXT"
#define SENSOR_PB2_CONFIG_FILE    "CFG/PB2.TXT"
#define SENSOR_PB3_CONFIG_FILE    "CFG/PB3.TXT"

#define PB1_POSITION              GPIO_PUSH_BUTTON_0
#define PB2_POSITION              GPIO_PUSH_BUTTON_1
#define PB3_POSITION              GPIO_PUSH_BUTTON_2

//_____ D E F I N I T I O N S ______________________________________________

/*! Pushbutton events string. */
const signed portCHAR *acpc_pushb_events[2] = {
   (signed portCHAR *)"RELEASE",
   (signed portCHAR *)"PRESS"
};

/*! Pushbutton events emulation. */
static bool b_pushb1 = PUSHB_EVENT_RELEASE;
static bool b_pushb2 = PUSHB_EVENT_RELEASE;
#if BOARD == EVK1100
static bool b_pushb3 = PUSHB_EVENT_RELEASE;
#endif


//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param buf char buffer in which the push button state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb1_get_value( char* buf )
{
   // input is pulled up, if 1 : input is not active
   if (gpio_get_pin_value(PB1_POSITION)) {
      b_pushb1 = PUSHB_EVENT_RELEASE;
   } else {
      b_pushb1 = PUSHB_EVENT_PRESS;
   }
   // Build the log string.
   sprintf( buf, "%s\r\n", (portCHAR *)acpc_pushb_events[b_pushb1]);
   return true;
}



/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param buf char buffer in which the push button state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb2_get_value( char* buf )
{
   // input is pulled up, if 1 : input is not active
   if (gpio_get_pin_value(PB2_POSITION)) {
      b_pushb2 = PUSHB_EVENT_RELEASE;
   } else {
      b_pushb2 = PUSHB_EVENT_PRESS;
   }
   // Build the log string.
   sprintf( buf, "%s\r\n", (portCHAR *)acpc_pushb_events[b_pushb2]);
   return true;
}



#if BOARD == EVK1100
/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param buf char buffer in which the push button state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb3_get_value( char* buf )
{
   // input is pulled up, if 1 : input is not active
   if (gpio_get_pin_value(PB3_POSITION)) {
      b_pushb3 = PUSHB_EVENT_RELEASE;
   } else {
      b_pushb3 = PUSHB_EVENT_PRESS;
   }
   // Build the log string.
   sprintf( buf, "%s\r\n", (portCHAR *)acpc_pushb_events[b_pushb3]);
   return true;
}
#endif
