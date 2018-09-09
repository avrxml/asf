/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel joystick sensor module.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
