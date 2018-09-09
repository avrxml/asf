/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuator interface.
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
