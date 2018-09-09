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
