/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuators module.
 *
 * - Supported boards:   EVK1100
 *
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

#include "compiler.h"
#include "FreeRTOS.h"

#include "lcd.h"

#include "datalog.h"
#include "shell.h"
#include "board.h"

#include "actuator.h"
#include "mmi.h"
#include "cptime.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#ifndef MMILCD_ENABLE
/*! Error msg if the LCD is not used. */
const signed portCHAR *LCD_ERRMSG_UNAVAILABLE = (signed portCHAR *)"Error"CRLF"The LCD is not available."CRLF;
#endif

extern int sprintf(char *out, const char *format, ...);  // From printf-stdarg.c

//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Get the lcd current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_lcd_get_value( xLogDef *pxLog )
{
   // Build the log string.
   pxLog->pcStringLog = (char *)"Enter your text"CRLF;
   pxLog->pfFreeStringLog = NULL; // We use a constant

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}



/*!
 *  \brief Set the user msg area of the lcd.
 *         Takes up to four parameters
 *         the first parameter is the usrmsg parameter, the second parameter is its value,
 *         the third is the time parameter(optional), the fourth is the time value(optional).
 *         Format: set_actuator_value actuator=lcd usrmsg=msg [time=date]
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 2, at most 4.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        If the action is successful, no response string is output.
 *                        If the action failed, a response string is output.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_lcd_set_value( eModId xModId, int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   /* 1) Check the input. */
   //  i) Arguments: at least 2, at most 4.
   if( ( 2 > ac ) || ( 4 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  ii) Check that the 1st arg is 'usrmsg'.
   if (strcmp((char *)av[0] , "usrmsg"))
   {
     // Syntax error.
     *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
     return( SHELL_EXECSTATUS_OK_NO_FREE );
   }

   //  iii) If there is a third parameter, check that it is equal to "time" and
   // record a new scheduling.
   if( 4 == ac )
   {
      return( e_actuator_ScheduleCmdSet( CPTIME_SCHEDCMDID_SETLCDVAL, ac, av, ppcStringReply ) );
   }

   // Display the user msg.
#ifdef MMILCD_ENABLE
   vMMI_UserMessDisplay((portCHAR *)av[1]);
   return( SHELL_EXECSTATUS_OK );
#else
   *ppcStringReply = (signed portCHAR *)LCD_ERRMSG_UNAVAILABLE;
   return( SHELL_EXECSTATUS_OK_NO_FREE );
#endif
}
