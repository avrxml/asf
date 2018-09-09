/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel sensors interface module.
 *
 * This module acts as an interface to all sensors.
 *
 *
 *                       The example is written for UC3 and EVK1100.
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


#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "shell.h"
#include "datalog.h"


//_____ M A C R O S ________________________________________________________

#define SENSOR_MSG_ALARM_ON     "alarm=on"CRLF
#define SENSOR_MSG_ALARM_OFF    "alarm=off"CRLF
#define SENSOR_MSG_CONFIG_SET   SHELL_MSG_CONFIG_SET
//_____ D E C L A R A T I O N S ____________________________________________

/*!
 * Start the sensor module.
 *
 * \return true upon success, else false.
 */
bool bsensor_start( void );

/*!
 * \brief Stop the sensor module resources.
 */
void v_sensor_stop( void );

/*!
 *  \brief Get a current sensor value.
 *
 *  \param pxLog a Log structure. The sensor id is in the id field of pxLog.
 *
 *  \return true upon success, false if error.
 */
bool b_sensor_get_value( xLogDef *pxLog );



/*!
 *  \brief The get sensor value command: get value(s) from a sensor.
 *         Takes two parameters, one of which is optional.
 *         The first parameter is the sensor's name, the optional parameter is
 *         the number of samples requested.
 *         Format: get_sensor_value sensorname [nbsamples], when nbsamples is
 *         given, the command should give an history of nbsamples-1 logs + the
 *         last value.
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1 or 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_cmd_get_value( eModId xModId, signed short FsNavId,
                                    int ac, signed portCHAR *av[],
                                    signed portCHAR **ppcStringReply );


/*!
 *  \brief The get sensor config command: get the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: get_sensor_config sensorname
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
eExecStatus e_sensor_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );


/*!
 *  \brief The set sensor config command: set the value of a config field of a sensor.
 *         Takes three parameters.
 *         The first parameter is the sensor's name, the second parameter is
 *         the config field name, the third parameter is the value.
 *         Format: set_sensor_config sensorname field=value
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 3.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );


/*!
 *  \brief The sensor help command: display the sensors available shell commands.
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
eExecStatus e_sensor_help( eModId xModId, signed short FsNavId,
                           int ac, signed portCHAR *av[],
                           signed portCHAR **ppcStringReply );

#endif // __SENSOR_H__
