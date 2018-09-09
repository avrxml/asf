/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel push buttons sensor module.
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
