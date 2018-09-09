/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP on Ethernet entry point.
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


