/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuators module.
 *
 * - Supported boards:   EVK1100
 *
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


//_____  I N C L U D E S ___________________________________________________

#include <string.h>

#include "compiler.h"
#include "FreeRTOS.h"

#include "datalog.h"
#include "shell.h"
#include "board.h"
#include "supervisor.h"
#include "cptime.h"
#include "actuator.h"
#include "cpled.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

const signed portCHAR *const CPLED_OFF_STR = (signed portCHAR *)"off";
const signed portCHAR *const CPLED_ON_STR = (signed portCHAR *)"on";

extern int sprintf(char *out, const char *format, ...);  // From printf-stdarg.c

//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Get the ledm1 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm1_get_value( xLogDef *pxLog )
{
   // Build the log string.
   if( LED_Test( LEDM1 ) )
      pxLog->pcStringLog = (char *)CPLED_ON_STR;
   else
      pxLog->pcStringLog = (char *)CPLED_OFF_STR;
   pxLog->pfFreeStringLog = NULL; // We use a constant

   return( true );
}


/*!
 *  \brief Get the ledm2 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm2_get_value( xLogDef *pxLog )
{
   // Build the log string.
   if( LED_Test( LEDM2 ) )
      pxLog->pcStringLog = (char *)CPLED_ON_STR;
   else
      pxLog->pcStringLog = (char *)CPLED_OFF_STR;
   pxLog->pfFreeStringLog = NULL; // We use a constant

   return( true );
}


/*!
 *  \brief Get the ledb1 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledb1_get_value( xLogDef *pxLog )
{
   // Alloc memory for the log string.
   pxLog->pcStringLog = pvPortMalloc( 8*sizeof( char ) );
   if( NULL == pxLog->pcStringLog )
   {
      return( false );
   }
   pxLog->pfFreeStringLog = vPortFree; // Because pvPortMalloc() was used to
                                       // alloc the log string.
   // Build the log string.
   sprintf( pxLog->pcStringLog, "%d,%d", LED_Get_Intensity( LEDB1G ),
            LED_Get_Intensity( LEDB1R ) );

   return( true );
}


/*!
 *  \brief Get the ledb2 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledb2_get_value( xLogDef *pxLog )
{
   // Alloc memory for the log string.
   pxLog->pcStringLog = pvPortMalloc( 8*sizeof( char ) );
   if( NULL == pxLog->pcStringLog )
   {
      return( false );
   }
   pxLog->pfFreeStringLog = vPortFree; // Because pvPortMalloc() was used to
                                       // alloc the log string.
   // Build the log string.
   sprintf( pxLog->pcStringLog, "%d,%d", LED_Get_Intensity( LEDB2G ),
            LED_Get_Intensity( LEDB2R ) );

   return( true );
}


/*  \brief The set monocolor led1 value command: set the value of the monocolor led1.
 *         Takes up to four parameters
 *         the first parameter is the state parameter, the second parameter is its value(on/off),
 *         the third is the time parameter(optional), the fourth is the time value(optional).
 *         Format: set_actuator_value actuator=actuatorname state=value [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 2, at most 4.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_ledm1_set_value( eModId xModId, int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   /* 1) Check the input. */
   //  i) Arguments: at least 2, at most 4.
   if( ( 2 > ac ) || ( 4 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  ii) Check that the 1st arg is state.
   if( strcmp( (char *)av[0], "state" ) )
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  iii) If there is a third parameter, check that it is equal to "time" and
   // record a new scheduling.
   if( 4 == ac )
   {
      return( e_actuator_ScheduleCmdSet( CPTIME_SCHEDCMDID_SETLEDM1VAL, ac, av, ppcStringReply ) );
   }

   if( !strcmp( (char *)av[1], (char *)CPLED_ON_STR ) )
   {
      LED_On( LEDM1 );
   }
   else if( !strcmp( (char *)av[1], (char *)CPLED_OFF_STR ) )
   {
      LED_Off( LEDM1 );
   }
   else
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   return( SHELL_EXECSTATUS_OK );
}


/*!
 *  \brief The set monocolor led2 value command: set the value of the monocolor led2.
 *         Takes up to four parameters
 *         the first parameter is the state parameter, the second parameter is its value(on/off),
 *         the third is the time parameter(optional), the fourth is the time value(optional).
 *         Format: set_actuator_value actuator=actuatorname state=value [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 2, at most 4.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_ledm2_set_value( eModId xModId, int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   /* 1) Check the input. */
   //  i) Arguments: at least 2, at most 4.
   if( ( 2 > ac ) || ( 4 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  ii) Check that the 1st arg is state.
   if( strcmp( (char *)av[0], "state" ) )
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  iii) If there is a third parameter, check that it is equal to "time" and
   // record a new scheduling.
   if( 4 == ac )
   {
      return( e_actuator_ScheduleCmdSet( CPTIME_SCHEDCMDID_SETLEDM2VAL, ac, av, ppcStringReply ) );
   }

   if( !strcmp( (char *)av[1], (char *)CPLED_ON_STR ) )
   {
      LED_On( LEDM2 );
   }
   else if( !strcmp( (char *)av[1], (char *)CPLED_OFF_STR ) )
   {
      LED_Off( LEDM2 );
   }
   else
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   return( SHELL_EXECSTATUS_OK );
}



/*!
 *  \brief The set bicolor ledb1 value command: set the value of the bicolor ledb1.
 *         Takes up to six parameters
 *         the first parameter is the green parameter, the second parameter is its value in [0,255],
 *         the third parameter is the red parameter, the fourth parameter is its value in [0,255],
 *         the fifth is the time parameter(optional), the sixth is the time value(optional) expressed in seconds.
 *         Format: set_actuator_value actuator=actuatorname green=value red=value [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 4, at most 6.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_ledb1_set_value( eModId xModId, int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   /* 1) Check the input. */
   //  i) Arguments: at least 4, at most 6.
   if( ( 4 > ac ) || ( 6 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  ii) Check that the 1st arg is 'green' and that the 3rd is 'red'.
   if( strcmp( (char *)av[0], "green" ) || strcmp( (char *)av[2], "red" ))
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  iii) If there is a fifth parameter, check that it is equal to "time" and
   // record a new scheduling.
   if( 6 == ac )
   {
      return( e_actuator_ScheduleCmdSet( CPTIME_SCHEDCMDID_SETLEDB1VAL, ac, av, ppcStringReply ) );
   }

   /* 2) Set the val. */
   LED_Set_Intensity( LEDB1G, atoi( (char *)av[1] ) );
   LED_Set_Intensity( LEDB1R, atoi( (char *)av[3] ) );

   return( SHELL_EXECSTATUS_OK );
}


/*!
 *  \brief The set bicolor ledb2 value command: set the value of the bicolor ledb2.
 *         Takes up to six parameters
 *         the first parameter is the green parameter, the second parameter is its value in [0,255],
 *         the third parameter is the red parameter, the fourth parameter is its value in [0,255],
 *         the fifth is the time parameter(optional), the sixth is the time value(optional) expressed in seconds.
 *         Format: set_actuator_value actuator=actuatorname green=value red=value [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 4, at most 6.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_ledb2_set_value( eModId xModId, int ac, signed portCHAR *av[],
                               signed portCHAR **ppcStringReply )
{
   /* 1) Check the input. */
   //  i) Arguments: at least 4, at most 6.
   if( ( 4 > ac ) || ( 6 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  ii) Check that the 1st arg is 'green' and that the 3rd is 'red'.
   if( strcmp( (char *)av[0], "green" ) || strcmp( (char *)av[2], "red" ))
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   //  iii) If there is a fifth parameter, check that it is equal to "time" and
   // record a new scheduling.
   if( 6 == ac )
   {
      return( e_actuator_ScheduleCmdSet( CPTIME_SCHEDCMDID_SETLEDB2VAL, ac, av, ppcStringReply ) );
   }

   /* 2) Set the val. */
   LED_Set_Intensity( LEDB2G, atoi( (char *)av[1] ) );
   LED_Set_Intensity( LEDB2R, atoi( (char *)av[3] ) );

   return( SHELL_EXECSTATUS_OK );
}
