/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel actuators interface module.
 *
 * This module acts as an interface to all actuators.
 *
 *
 *                       The example is written for UC3 and EVK1100.
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
 * Brief description of the module:
 * This module acts as an interface to all actuators.
 *
 * Detailed description of the module:
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>

#include "compiler.h"
#include "FreeRTOS.h"

#include "shell.h"
#include "datalog.h"
#include "cpled.h"
#include "lcd.h"

#include "tracedump.h"
#include "cptime.h"
#include "actuator.h"

//_____ M A C R O S ________________________________________________________

/*! Max number of actuators. */
#define ACTUATOR_MAXNB_ACTUATORS   (DATALOG_ID_LCD - DATALOG_ID_LEDB1 +1)

#define ACTUATOR_LIST  "{ledb1,ledb2,ledm1,ledm2,lcd}"

//_____ D E F I N I T I O N S ______________________________________________

/*! An actuator register cell structure. */
typedef struct ActuatorReg {
   // Fct ptr to the actual actuator get value function.
   bool (*pfGetActuatorValue)( xLogDef *pxLog );
   // Fct ptr to the actual actuator set value function.
   eExecStatus (*pfSetActuatorValue)( eModId xModId, int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply );
}xActuatorReg;

xActuatorReg axActuatorsRegistry[ACTUATOR_MAXNB_ACTUATORS] = {
   { b_ledb1_get_value, e_ledb1_set_value } ,
   { b_ledb2_get_value, e_ledb2_set_value } ,
   { b_ledm1_get_value, e_ledm1_set_value } ,
   { b_ledm2_get_value, e_ledm2_set_value } ,
   { b_lcd_get_value, e_lcd_set_value }
};

/*!
 * The params given to an actuator scheduled command.
 */
typedef struct st_scheduled_param {
   int ac;
   signed portCHAR *av[4];
}Scheduled_Cmd_Params;

const signed portCHAR *const ACTUATOR_MSG_HELP            = (signed portCHAR *)"\
"CRLF"set_actuator_value actuator={ledb1,ledb2} green=value red=value [time=date]"CRLF"\
set_actuator_value actuator={ledm1,ledm2} state={on,off} [time=date]"CRLF"\
set_actuator_value actuator=lcd usrmsg=\"string\" [time=date]"CRLF"\
get_actuator_value actuator="ACTUATOR_LIST" : display the current value of an actuator"CRLF;

/*! Error msg upon get_actuator_value syntax error. */
const signed portCHAR *const ACTUATOR_ERRMSG_GETVAL_SYNTAXERROR   = (signed portCHAR *)"Error"CRLF"Usage: get_actuator_value actuator=actuatorname"CRLF;

/*! Error msg upon unreferenced actuator error. */
const signed portCHAR *const ACTUATOR_ERRMSG_UNREFACTUATOR   = (signed portCHAR *)"Error"CRLF"Unreferenced actuator name"CRLF;

/*! Error msg upon get_actuator_value actuator error. */
const signed portCHAR *const ACTUATOR_ERRMSG_GETVAL_FAIL          = (signed portCHAR *)"Error"CRLF"Actuator failed to deliver a value."CRLF;

/*! Error msg upon set_actuator_value syntax error. */
const signed portCHAR *const ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR   = (signed portCHAR *)"Error"CRLF"Usage: set_actuator_value actuator=actuatorname fields..."CRLF;

//_____ D E C L A R A T I O N S ____________________________________________

/*  \brief The get actuator value command: get value(s) from an actuator.
 *         Format: get_actuator_value actuator=actuatorname
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_actuator_cmd_get_value( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   int       i;
   xLogDef   *pxLog;


   /* 0) If the way to reply is unavailable, it's no use to continue. */
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   /* 1) Check the input. */
   //  i) Two arguments exactly.
   if( 2 != ac )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_GETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   // No need to check that av[0] == actuator. We actually use the "actuator=name"
   // format just to comply with the cgi calls rules.

   //  ii) Identify the actuator.
   for( i=DATALOG_ID_LEDB1;
        i<=DATALOG_ID_LCD && strcmp( (char *)av[1], acLogSourceName[i] );
        i++ );
   i -= DATALOG_ID_LEDB1; // Scale the index to fit for the actuator only array.

   if( ACTUATOR_MAXNB_ACTUATORS == i )
   {   // Unreferenced actuator.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_UNREFACTUATOR;
      return( SHELL_EXECSTATUS_KO );
   }

   /* 3) Perform the command. */
   // Alloc and init a log.
   pxLog = pxdatalog_log_alloc_init( DATALOG_ALLOC_DYNAMIC );
   if( NULL == pxLog )
   {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }

   // Init the id field of the log.
   pxLog->id = i+DATALOG_ID_LEDB1;

   // Get the value.
   if( false == axActuatorsRegistry[i].pfGetActuatorValue( pxLog ) )
   {
      vdatalog_log_free( pxLog );
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_GETVAL_FAIL;
      return( SHELL_EXECSTATUS_KO );
   }

   /* 4) Build the reply. */
   // Alloc space for the reply.
   *ppcStringReply = (signed portCHAR *)pvPortMalloc( DATALOG_LOG_MAXSIZE );
   if( NULL == *ppcStringReply )
   {
      vdatalog_log_free( pxLog );
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }

   // Build the string.
   vdatalog_make_logstring( pxLog, *ppcStringReply );

   // Free the log.
   vdatalog_log_free( pxLog );

   return( SHELL_EXECSTATUS_OK );
}


/*  \brief The set actuator value command: set the value(s) or state(s) of an actuator.
 *         Takes up to eight parameters
 *         The first parameter is the cte actuator indicator, the second is the actuator's name,
 *         the third parameter is a field name, the fourth parameter is its value, the fifth is
 *         another field name(optional), the sixth is its value(optional),
 *         the seventh is the time parameter(optional), the eighth is the time value(optional).
 *         Format: set_actuator_value actuator=actuatorname field1=value [field2=value] [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 4, at most 8.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_actuator_cmd_set_value( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   int i;


   /* 0) If the way to reply is unavailable, it's no use to continue. */
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   /* 1) Check the input. */
   //  i) Arguments: at least 4, at most 8.
   if( ( 4 > ac ) || ( 8 < ac ) )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   // No need to check that av[0] == actuator. We actually use the "actuator=name"
   // format just to comply with the cgi calls rules.

   //  ii) Identify the actuator.
   for( i=DATALOG_ID_LEDB1;
        i<=DATALOG_ID_LCD && strcmp( (char *)av[1], acLogSourceName[i] );
        i++ );
   i -= DATALOG_ID_LEDB1; // Scale the index to fit for the actuator only array.

   if( ACTUATOR_MAXNB_ACTUATORS == i )
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_UNREFACTUATOR;
      return( SHELL_EXECSTATUS_KO );
   }

   /* 3) Perform the command. */
   return( axActuatorsRegistry[i].pfSetActuatorValue( xModId, ac-2, &(av[2]), ppcStringReply ) );
}


/*!
 *  \brief Execute a scheduled command.
 *
 *  \param CmdId Input. The set cmd identifier.
 *  \param pxCmdParams Input. The parameters (ac,*av[]) for the cmd.
 */
void vExecScheduledSet( int CmdId, void *pxCmdParams )
{
   Scheduled_Cmd_Params   *pxActuatorCmdParams;
   int                    i;


   // Cast pxCmdParams to the appropriate struct.
   pxActuatorCmdParams = (Scheduled_Cmd_Params *)pxCmdParams;
   switch( CmdId )
   {
      case CPTIME_SCHEDCMDID_SETLEDM1VAL:
         e_ledm1_set_value( SYS_MODID_NONE, pxActuatorCmdParams->ac, pxActuatorCmdParams->av, NULL );
         break;

      case CPTIME_SCHEDCMDID_SETLEDM2VAL:
         e_ledm2_set_value( SYS_MODID_NONE, pxActuatorCmdParams->ac, pxActuatorCmdParams->av, NULL );
         break;

      case CPTIME_SCHEDCMDID_SETLEDB1VAL:
         e_ledb1_set_value( SYS_MODID_NONE, pxActuatorCmdParams->ac, pxActuatorCmdParams->av, NULL );
         break;

      case CPTIME_SCHEDCMDID_SETLEDB2VAL:
         e_ledb2_set_value( SYS_MODID_NONE, pxActuatorCmdParams->ac, pxActuatorCmdParams->av, NULL );
         break;

      case CPTIME_SCHEDCMDID_SETLCDVAL:
         e_lcd_set_value( SYS_MODID_NONE, pxActuatorCmdParams->ac, pxActuatorCmdParams->av, NULL );
         break;
   }
   // Free the string params and the params struct.
   for( i=0; i<pxActuatorCmdParams->ac; i++ )
   {
      vPortFree( pxActuatorCmdParams->av[i] );
   }
   vPortFree( pxActuatorCmdParams );
}


/*!
 *  \brief Schedule an actuator set for later.
 *
 *  \param CmdId          Input. The set actuator cmd identifier.
 *  \param ac             Input. The argument counter.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        If the action is successful, no response string is output.
 *                        If the action failed, a response string is output.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_actuator_ScheduleCmdSet( eSchedCmdId CmdId,
                                       int ac, signed portCHAR *av[],
                                       signed portCHAR **ppcStringReply )
{
   int                    i,j;
   Scheduled_Cmd_Params   *pxCmdParams;


   // Check that the before-last param is "time".
   if( strcmp( (char *)av[ac-2], "time" ) )
   {
      *ppcStringReply = (signed portCHAR *)ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }

   // Alloc and fill a param structure that contains the parameters that will
   // be given to the scheduled function when it will execute.
   pxCmdParams = pvPortMalloc( sizeof(Scheduled_Cmd_Params) );
   if( NULL == pxCmdParams )
   {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }

   // Alloc the params.
   pxCmdParams->ac = ac-2;
   for( i=0; i<pxCmdParams->ac; i++ )
   {
      pxCmdParams->av[i] = pvPortMalloc( strlen( (char *)av[i] ) +1 );
      if( NULL == pxCmdParams->av[i] )
      {   // An alloc failed => free previous alloc.
         for(j=0; j<i; j++)
         {
            vPortFree( pxCmdParams->av[j] );
         }
         vPortFree( pxCmdParams );
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
         return( SHELL_EXECSTATUS_KO );
      }
   }

   // Copy the params.
   for( i=0; i<pxCmdParams->ac; i++ )
   {
      strcpy( (char *)pxCmdParams->av[i], (char *)av[i] );
   }

   // Record the cmd.
   return( e_cptime_RecordScheduledCmd( (char *)av[ac-1],
                                        CmdId,
                                        vExecScheduledSet,
                                        pxCmdParams,
                                        ppcStringReply) );
}


/*  \brief The actuator command: display the actuators available shell commands.
 *         Format: help
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Ignored.
 *  \param av             Ignored.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_actuator_help( eModId xModId, signed short FsNavId,
                             int ac, signed portCHAR *av[],
                             signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Perform the command.
   *ppcStringReply = (signed portCHAR *)ACTUATOR_MSG_HELP;

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}
