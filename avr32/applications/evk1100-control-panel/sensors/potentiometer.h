/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel potentiometer sensor interface.
 *
 * - Supported boards:   EVK1100
 *
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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


#ifndef __POTENTIOMETER_H__
#define __POTENTIOMETER_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "shell.h"

#include "datalog.h"

//_____ M A C R O S ________________________________________________________



//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief Init the potentiometer sensor channel.
 *
 *  \return true upon success, false if error.
 */
bool b_potentiometer_init ( void );

/*!
 *  \brief Get the potentiometer sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_potentiometer_get_config( signed portCHAR **ppcStringReply );

/*!
 *  \brief Set the sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *  \param ac             Input. Number of args
 *  \param av             Input. pointer to args
 *
 *  \return the status of the command execution.
 */
eExecStatus e_potentiometer_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );


/*!
 *  \brief Get the current potentiometer value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_potentiometer_get_value( xLogDef *pxLog );

/*! Declaration for datalog use */
extern unsigned int ul_pot_lograte;

#endif // __POTENTIOMETER_H__
