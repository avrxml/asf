/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel pushbuttons sensor interface.
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


#ifndef __PUSHB_H__
#define __PUSHB_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "datalog.h"

//_____ M A C R O S ________________________________________________________



//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Init the push button 1 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb1_init ( void );

/*!
 * \brief Stop the push button 1 sensor.
 */
void v_pushb1_stop( void );


/*!
 *  \brief Init the push button 2 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb2_init ( void );

/*!
 * \brief Stop the push button 2 sensor.
 */
void v_pushb2_stop( void );


/*!
 *  \brief Init the push button 3 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb3_init ( void );

/*!
 * \brief Stop the push button 1 sensor.
 */
void v_pushb3_stop( void );


/*!
 *  \brief Get the pushbutton1 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb1_get_config( signed portCHAR **ppcStringReply );


/*!
 *  \brief Get the pushbutton2 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb2_get_config( signed portCHAR **ppcStringReply );


/*!
 *  \brief Get the pushbutton3 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb3_get_config( signed portCHAR **ppcStringReply );


/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb1_get_value( xLogDef *pxLog );

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb2_get_value( xLogDef *pxLog );

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb3_get_value( xLogDef *pxLog );

/*!
 *  \brief Set the sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *  \param ac             Input. Number of args
 *  \param av             Input. pointer to args
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb1_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );

/*!
 *  \brief Set the sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *  \param ac             Input. Number of args
 *  \param av             Input. pointer to args
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb2_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );

/*!
 *  \brief Set the sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *  \param ac             Input. Number of args
 *  \param av             Input. pointer to args
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb3_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );

#endif // __PUSHB_H__
