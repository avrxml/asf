/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP on Ethernet entry point.
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

#ifndef ETHERNET_H
#define ETHERNET_H


#include "supervisor.h"
#include "shell.h"


/*!
 * \brief Create the vStartEthernetTask task.
 *
 */
void vStartEthernetTaskLauncher( unsigned portBASE_TYPE uxPriority );

/*!
 * \brief configure lwIP and MACB, start lwIP layer,
 *        start servers tasks through lwIP services.
 *
 */
portTASK_FUNCTION( vStartEthernetTask, pvParameters );

/*!
 * \brief Stop the Ethernet module resources.
 */
void v_ethernet_stopResources( void );

/*! \brief The get sensor config command: get the config fields value of a sensor.
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
eExecStatus e_ethernet_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );

/*! \brief The set sensor config command: set the value of a config field of a sensor.
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
eExecStatus e_ethernet_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );

/*!
 *  \brief The development only ip statck stats: display the TCP/IP stack stats on COM2.
 *         No parameters.
 *         Format: ipstat
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_ip_stat( eModId xModId, signed short FsNavId,
                       int ac, signed portCHAR *av[],
                       signed portCHAR **ppcStringReply );


/*! \brief get the current MAC address :
 *         formatted as follow :
 *              XX:XX:XX:XX:XX:XX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetMACAddr(portCHAR * pcConfig);

/*! \brief get the current IP address :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetIPAddr(portCHAR * pcConfig);

/*! \brief get the current Subnet mask :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetSubnetMask(portCHAR * pcConfig);

/*! \brief get the current GW address :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetGWAddr(portCHAR * pcConfig);



#endif


