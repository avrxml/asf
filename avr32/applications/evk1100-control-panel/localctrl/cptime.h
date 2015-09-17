/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel local time module.
 * The local time module is in charge of the Control Panel local time management.
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


#ifndef __CPTIME_H__
#define __CPTIME_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "shell.h"
#include "datalog.h"

//_____ M A C R O S ________________________________________________________



//_____ D E C L A R A T I O N S ____________________________________________

/*
 * Scheduled cmds id.
 */
typedef enum
{
   CPTIME_SCHEDCMDID_SETLEDM1VAL,   // Set the LEDm1 value.
   CPTIME_SCHEDCMDID_SETLEDM2VAL,   // Set the LEDm2 value.
   CPTIME_SCHEDCMDID_SETLEDB1VAL,   // Set the LEDb1 value.
   CPTIME_SCHEDCMDID_SETLEDB2VAL,   // Set the LEDb2 value.
   CPTIME_SCHEDCMDID_SETLCDVAL,     // Set the LCD usermsg value.
} eSchedCmdId;


/*!
 * \brief Init the time module.
 */
void v_cptime_Init( void );


#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_cptime_trace( void );
#endif

/*!
 * \brief Get the current time in the "YYYYMMDDHHMMSSMS" string format.
 *
 * \param pcDate  Input/Output. Input allocated array. Output is the current time
 *                expressed in the "YYYYMMDDHHMMSSMS" string format.
 */
void v_cptime_GetDateInFatStringFormat( char *pcDate );

/*!
 * \brief Update the local time.
 *
 */
void v_cptime_UpdateLocalTime( void );


/*!
 *  \brief Record a scheduled command.
 *
 *  \param pcDate         Input. The date the cmd is scheduled at.
 *                        Format: seconds OR mm/dd/yy hh:mm:ss
 *  \param CmdId          Scheduled command id.
 *  \param pfScheduledCmd pointer on fct to be called at expiration date
 *  \param pvCmdParams    pointer on struct to give as param to pfScheduledCmd
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_RecordScheduledCmd( char *pcDate, int CmdId,
                                         void (*pfScheduledCmd)( int, void * ),
                                         void *pvCmdParams,
                                         signed portCHAR **ppcStringReply );


/*!
 * \brief Execute a scheduled command if expiration date is up.
 */
void v_cptime_ExecuteScheduledCmd( void );


/*! \brief The set time config command: set each config field value of the time module.
 *         Takes one parameter : field=value
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );


/*! \brief The get time config command: get the config fields value of the time module
 *         Takes no parameter
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_cptime_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );


#endif // __CPTIME_H__
