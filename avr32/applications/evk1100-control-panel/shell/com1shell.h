/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel COM1 shell module.
 *
 * This module manages a command shell on COM1.
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

#ifndef COM1SHELL_H
#define COM1SHELL_H

#include "FreeRTOS.h"
#include "fs_com.h"
#include "supervisor.h"


/*!
 * Start the COM1 shell module.
 * \param uxPriority The priority base of the COM1 shell tasks.
 */
void vStartCom1Shell( unsigned portBASE_TYPE uxPriority );

/*!
 * \brief Stop the com1shell module resources.
 */
void v_com1shell_stopResources( void );

/*!
 * Return the status of the COM1 shell module.
 * \return one of the eStatus enum value.
 */
eStatus xCom1Shell_GetStatus( void );


/*!
 *  \brief Print a string to the COM1 port.
 *
 *  \param pcString Input. The string to print.
 */
void vcom1shell_PrintMsg(const signed portCHAR *pcString);

/*!
 *  \brief Put a char to the COM1 port.
 *
 * \param cByte Input. The char to print.
 */
void vcom1shell_PutChar(signed portCHAR cByte);

/*!
 *  \brief Get a char from the COM1 port.
 *
 * \param pcByte     Output. The char received from the COM1 port.
 */
signed portBASE_TYPE com1shell_GetChar(signed portCHAR *pcByte);

/*!
 * \brief Mount default local drive 'a:'
 */
void v_com1shell_mount_local_drive( void );

#endif
