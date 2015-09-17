/**
 * \file
 *
 * \brief AVR UC3 Frequency Meter (FREQM) driver
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

#ifndef _CONF_BOARD_H_
#define _CONF_BOARD_H_

/**
 * \ingroup freqm_group
 * \defgroup usart_setting USART Settings
 *
 * @{
 */

// For the OSC32, connect STK600.PORTB.PB37 to STK600.AUX.32KHz

# define EXAMPLE_USART               (&AVR32_USART1)
// For the RX pin, connect STK600.PORTD.PD0 to STK600.RS232 SPARE.RXD
# define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
# define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
// For the TX pin, connect STK600.PORTC.PC7 to STK600.RS232 SPARE.TXD
# define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
# define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
# define FPBA                        FOSC0

//! @}

// Osc0 frequency
#define BOARD_OSC0_HZ                12000000
// 2048 cycles (18 ms)
#define BOARD_OSC0_STARTUP_US        18000
// Crystal connected to XIN/XOUT
#define BOARD_OSC0_IS_XTAL           true

#endif // _CONF_BOARD_H_

