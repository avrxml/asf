/**
 * \file
 *
 * \brief Application CDC Definitions
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CDC_H_INCLUDED
#define CDC_H_INCLUDED

#include "keyboard.h"
#include "udc.h"
#include "udi_cdc.h"

//! \brief Serial number to be passed to USB stack
extern uint8_t cdc_serial_number[];

//! \brief Start up the needed parts for USB CDC
void cdc_start(void);

/**
 * \brief Called by CDC interface to open/close port communication
 *
 * \param enable Open if true, and close if false
 */
void cdc_set_dtr(bool enable);

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host enable CDC interface
 *
 * \retval true if cdc startup is successfully done
 */
static inline bool cdc_enable(void)
{
	return true;
}

/**
 * \brief Called by CDC interface
 *
 * Callback running when USB Host disable cdc interface
 */
static inline void cdc_disable(void)
{
}

//! \brief Read character from CDC interface and translate to keycode
keycode_t cdc_getkey(void);

#endif /* CDC_H_INCLUDED */
