/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel leds actuator interface.
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


#ifndef __CPLED_H__
#define __CPLED_H__

#include "compiler.h"
#include "FreeRTOS.h"


//_____ M A C R O S ________________________________________________________



//_____ D E C L A R A T I O N S ____________________________________________


/*!
 *  \brief Get the ledm1 current value.
 *
 *  \param buf char buffer in which the ledm1 state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm1_get_value( char* buf );


/*!
 *  \brief Get the ledm2 current value.
 *
 *  \param buf char buffer in which the ledm2 state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm2_get_value( char* buf );


/*!
 *  \brief Get the ledm3 current value.
 *
 *  \param buf char buffer in which the ledm3 state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm3_get_value( char* buf );


/*!
 *  \brief Get the ledm4 current value.
 *
 *  \param buf char buffer in which the ledm4 state is stored.
 *
 *  \return true upon success, false if error.
 */
bool b_ledm4_get_value( char* buf );


/*  \brief Set the value of the monocolor led1.
 *
 *  \param cmd            Input. The argument vector.
 *  \param buf            Output. The response string.
 *
 *  \return the status of the command execution.
 */
bool e_ledm1_set_value( char* cmd, char* buf );


/*  \brief Set the value of the monocolor led2.
 *
 *  \param cmd            Input. The argument vector.
 *  \param buf            Output. The response string.
 *
 *  \return the status of the command execution.
 */
bool e_ledm2_set_value( char* cmd, char* buf );


/*  \brief Set the value of the monocolor led3.
 *
 *  \param cmd            Input. The argument vector.
 *  \param buf            Output. The response string.
 *
 *  \return the status of the command execution.
 */
bool e_ledm3_set_value( char* cmd, char* buf );


/*  \brief Set the value of the monocolor led4.
 *
 *  \param cmd            Input. The argument vector.
 *  \param buf            Output. The response string.
 *
 *  \return the status of the command execution.
 */
bool e_ledm4_set_value( char* cmd, char* buf );


/*  \brief Set the intensity of the LED which are On to a specific value.
 *
 *  \param intensity      Input. The LED intensity (0% to 100%)
 *
 *  \return the status of the command execution.
 */
bool e_ledm_refresh_intensity( U32 intensity );

#endif // __CPLED_H__
