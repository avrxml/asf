/**************************************************************************
 *
 * \file
 *
 * \brief UART configuration file.
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


#ifndef _CONF_UART_H_
#define _CONF_UART_H_

#include <avr32/io.h>
#include "compiler.h"


//_____ D E F I N I T I O N S ______________________________________________

/*! \name USART Settings
 */
//! @{

#define DFU_UART_BAUD_RATE  115200

#if UC3L

// USART0 WARNING: (USART0_TX, USART0_RX) are multiplexed with the JTAG.(TCK,TMS) functions
#define DFU_USART_CANDIDATE0              (&AVR32_USART0)
#define DFU_USART_CANDIDATE0_RX_PIN       AVR32_USART0_RXD_0_PIN
#define DFU_USART_CANDIDATE0_RX_FUNCTION  AVR32_USART0_RXD_0_FUNCTION
#define DFU_USART_CANDIDATE0_TX_PIN       AVR32_USART0_TXD_0_PIN
#define DFU_USART_CANDIDATE0_TX_FUNCTION  AVR32_USART0_TXD_0_FUNCTION
#define DFU_USART_CANDIDATE0_IRQ          AVR32_USART0_IRQ

#define DFU_USART_CANDIDATE1              (&AVR32_USART1)
#define DFU_USART_CANDIDATE1_RX_PIN       AVR32_USART1_RXD_0_2_PIN
#define DFU_USART_CANDIDATE1_RX_FUNCTION  AVR32_USART1_RXD_0_2_FUNCTION
#define DFU_USART_CANDIDATE1_TX_PIN       AVR32_USART1_TXD_0_2_PIN
#define DFU_USART_CANDIDATE1_TX_FUNCTION  AVR32_USART1_TXD_0_2_FUNCTION
#define DFU_USART_CANDIDATE1_IRQ          AVR32_USART1_IRQ

#define DFU_USART_CANDIDATE2              (&AVR32_USART2)
#define DFU_USART_CANDIDATE2_RX_PIN       AVR32_USART2_RXD_0_1_PIN
#define DFU_USART_CANDIDATE2_RX_FUNCTION  AVR32_USART2_RXD_0_1_FUNCTION
#define DFU_USART_CANDIDATE2_TX_PIN       AVR32_USART2_TXD_0_1_PIN
#define DFU_USART_CANDIDATE2_TX_FUNCTION  AVR32_USART2_TXD_0_1_FUNCTION
#define DFU_USART_CANDIDATE2_IRQ          AVR32_USART2_IRQ

#define DFU_USART_CANDIDATE3              (&AVR32_USART3)
#define DFU_USART_CANDIDATE3_RX_PIN       AVR32_USART3_RXD_0_0_PIN
#define DFU_USART_CANDIDATE3_RX_FUNCTION  AVR32_USART3_RXD_0_0_FUNCTION
#define DFU_USART_CANDIDATE3_TX_PIN       AVR32_USART3_TXD_0_0_PIN
#define DFU_USART_CANDIDATE3_TX_FUNCTION  AVR32_USART3_TXD_0_0_FUNCTION
#define DFU_USART_CANDIDATE3_IRQ          AVR32_USART3_IRQ

#endif
//! @}

#endif  // _CONF_UART_H_
