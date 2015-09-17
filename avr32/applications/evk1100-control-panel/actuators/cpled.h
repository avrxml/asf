/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuator interface.
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


#ifndef __CPLED_H__
#define __CPLED_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "datalog.h"


//_____ M A C R O S ________________________________________________________

#ifdef EVK1100_REVA
#  define LEDM1   LED_BI0_GREEN
#  define LEDM2   LED_BI0_RED
#  define LEDB1G  LED_MONO0_GREEN
#  define LEDB1R  LED_MONO1_GREEN
#  define LEDB2G  LED_MONO2_GREEN
#  define LEDB2R  LED_MONO3_GREEN
#else
#  define LEDM1   LED_MONO0_GREEN
#  define LEDM2   LED_MONO1_GREEN
#  define LEDB1G  LED_BI0_GREEN
#  define LEDB1R  LED_BI0_RED
#  define LEDB2G  LED_BI1_GREEN
#  define LEDB2R  LED_BI1_RED
#endif


//_____ D E C L A R A T I O N S ____________________________________________


/*!
 *  \brief Get the ledm1 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm1_get_value( xLogDef *pxLog );


/*!
 *  \brief Get the ledm2 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm2_get_value( xLogDef *pxLog );


/*!
 *  \brief Get the ledb1 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledb1_get_value( xLogDef *pxLog );


/*!
 *  \brief Get the ledb2 current value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_ledb2_get_value( xLogDef *pxLog );


/*  \brief TODO The set actuator value command: set the value(s) or state(s) of an actuator.
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
                               signed portCHAR **ppcStringReply );



/*  \brief TODO The set actuator value command: set the value(s) or state(s) of an actuator.
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
                               signed portCHAR **ppcStringReply );



/*  \brief TODO The set actuator value command: set the value(s) or state(s) of an actuator.
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
                               signed portCHAR **ppcStringReply );


/*  \brief TODO The set actuator value command: set the value(s) or state(s) of an actuator.
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
                               signed portCHAR **ppcStringReply );


#endif // __CPLED_H__
