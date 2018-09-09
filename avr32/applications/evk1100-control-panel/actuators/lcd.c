/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuators module.
 *
 * - Supported boards:   EVK1100
 *
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
