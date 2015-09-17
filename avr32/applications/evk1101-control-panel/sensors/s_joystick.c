/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel joystick sensor module.
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
#include "s_joystick.h"
#include "joystick.h"
#include "FreeRTOS.h"


//_____ M A C R O S ________________________________________________________

/*! Max number of events on joystick. */
#define JS_NB_EVENT         32

/*! Mask for events on joystick. */
#define JS_EVENT_PUSH          0x01
#define JS_EVENT_LEFT          0x02
#define JS_EVENT_RIGHT         0x04
#define JS_EVENT_UP            0x08
#define JS_EVENT_DOWN          0x10

//_____ D E F I N I T I O N S ______________________________________________

/*! Joystick events string, regarding bit field value. */
const signed portCHAR *acpc_js_events[JS_NB_EVENT] = {
   (signed portCHAR *)"RELEASE",(signed portCHAR *)"PRESS",(signed portCHAR *)"LEFT",(signed portCHAR *)"LEFT/PRESS",
   (signed portCHAR *)"RIGHT",(signed portCHAR *)"RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"UP",(signed portCHAR *)"UP/PRESS",(signed portCHAR *)"UP/LEFT",(signed portCHAR *)"UP/LEFT/PRESS",
   (signed portCHAR *)"UP/RIGHT",(signed portCHAR *)"UP/RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"DOWN",(signed portCHAR *)"DOWN/PRESS",(signed portCHAR *)"DOWN/LEFT",(signed portCHAR *)"DOWN/LEFT/PRESS",
   (signed portCHAR *)"DOWN/RIGHT",(signed portCHAR *)"DOWN/RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",
};

/*! Joystick events. */
static unsigned short x_joystick = 0;


//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Get the current joystick state.
 *
 *  \param buf char buffer in which the joystick state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_joystick_get_value( char* buf )
{
    // input is pulled up, if 1 : input is not active
    if ( !is_joystick_up() )  { Clr_bits(x_joystick, JS_EVENT_UP); }
    else                      { Set_bits(x_joystick, JS_EVENT_UP); }

    if ( !is_joystick_down() ) { Clr_bits(x_joystick, JS_EVENT_DOWN); }
    else                       { Set_bits(x_joystick, JS_EVENT_DOWN); }

    if ( !is_joystick_left() )  { Clr_bits(x_joystick, JS_EVENT_LEFT); }
    else                        { Set_bits(x_joystick, JS_EVENT_LEFT); }

    if ( !is_joystick_right() )  { Clr_bits(x_joystick, JS_EVENT_RIGHT); }
    else                         { Set_bits(x_joystick, JS_EVENT_RIGHT); }

    if ( !is_joystick_pressed() ) { Clr_bits(x_joystick, JS_EVENT_PUSH); }
    else                          { Set_bits(x_joystick, JS_EVENT_PUSH); }

   // Build the log string.
   sprintf( buf, "%s\r\n", (portCHAR *)acpc_js_events[x_joystick]);
   return true;
}
