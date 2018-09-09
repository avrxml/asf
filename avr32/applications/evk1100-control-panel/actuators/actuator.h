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


#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "cptime.h"

//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

/*! Error msg upon set_actuator_value syntax error. */
extern const signed portCHAR *const ACTUATOR_ERRMSG_SETVAL_SYNTAXERROR;

/*  \brief The get actuator value command: get value(s) from an actuator.
 *         Takes one parameter, the actuator name.
 *         Format: get_actuator_value actuator
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
eExecStatus e_actuator_cmd_get_value( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply );


/*  \brief The set actuator value command: set the value(s) or state(s) of an actuator.
 *         Takes up to seven parameters
 *         The first parameter is the actuator's name, the second parameter is
 *         a config field name, the third parameter is its value, the fourth is
 *         another config field name(optional), the fifth is its value(optional),
 *         the sixth is the time parameter(optional), the seventh is the time
 *         value(optional).
 *         Format: set_actuator_value actuatorname field1=value [field2=value] [time=date]
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be
 *                        at least 3, at most 7.
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
                                      signed portCHAR **ppcStringReply );


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
                                       signed portCHAR **ppcStringReply );


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
                             signed portCHAR **ppcStringReply );


/*!
 *  \brief Execute a scheduled command.
 *
 *  \param CmdId Input. The set cmd identifier.
 *  \param pxCmdParams Input. The parameters (ac,*av[]) for the cmd.
 */
void vExecScheduledSet( int CmdId, void *pxCmdParams );

#endif // __ACTUATOR_H__
