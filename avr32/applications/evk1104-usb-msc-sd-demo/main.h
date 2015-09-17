/**************************************************************************
 *
 * \file
 *
 * \brief Header file of the USB mass-storage example.
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


#ifndef _MASS_STORAGE_EXAMPLE_H_
#define _MASS_STORAGE_EXAMPLE_H_


//_____ I N C L U D E S ____________________________________________________

#include "pm.h"


//_____ D E F I N I T I O N S ______________________________________________

#define APPLI_CPU_SPEED   66000000
#define APPLI_PBA_SPEED   66000000

/*! \name USART Used for STDIO
 */
//! @{
#define STDIO_USART                   DBG_USART
#define STDIO_USART_RX_PIN            DBG_USART_RX_PIN
#define STDIO_USART_RX_FUNCTION       DBG_USART_RX_FUNCTION
#define STDIO_USART_TX_PIN            DBG_USART_TX_PIN
#define STDIO_USART_TX_FUNCTION       DBG_USART_TX_FUNCTION
#define STDIO_USART_IRQ               AVR32_USART0_IRQ
#define STDIO_USART_BAUDRATE          DBG_USART_BAUDRATE
//! @}


//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

extern pm_freq_param_t pm_freq_param;


#endif  // _MASS_STORAGE_EXAMPLE_H_
