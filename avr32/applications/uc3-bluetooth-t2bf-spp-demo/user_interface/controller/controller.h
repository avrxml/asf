/**************************************************************************
 *
 * \file
 *
 * \brief Controller interface
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "compiler.h"

/*! \name Constructor/Destructor
 */
//! \{
/*! \brief Initialization function of the controller interface.
 *
 * \param fcpu_hz CPU frequency.
 * \param fhsb_hz HSB frequency.
 * \param fpbb_hz PBB frequency.
 * \param fpba_hz PBA frequency.
 */
void controller_init(U32 fcpu_hz, U32 fhsb_hz, U32 fpbb_hz, U32 fpba_hz);

/*! \brief Refresh the controller.
 */
void controller_task(void);

/*! \brief Shutdown the controller.
 */
void controller_shutdown(void);
// \}

/*! \brief Reset the key state.
 */
void controller_clear(void);

/*! \name Navigation view
 */
//! \{
bool controller_inquiry(void);
bool controller_select(void);
bool controller_connect(void);
bool controller_send(void);
// \}

#endif // _CONTROLLER_H_
