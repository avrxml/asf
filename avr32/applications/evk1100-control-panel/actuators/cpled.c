/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuators module.
 *
 * - Supported boards:   EVK1100
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
