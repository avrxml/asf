/*****************************************************************************
 *
 * \file
 *
 * \brief Debugging module for the DSP library.
 * This file is used to make an abstraction of the driver used to control
 * the debugging peripheral.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "usart.h"
#include "gpio.h"
#include "board.h"
#include "dsp.h"
#include "dsp_debug.h"

/*! \name USART Settings
 */
//! @{
// Baud rate
#define EXAMPLE_USART_BAUDRATE        57600
// Number of bits per char
#define EXAMPLE_USART_BITSPERCHAR     8
// Parity
#define EXAMPLE_USART_PARITY          USART_NO_PARITY
// Stop bit
#define EXAMPLE_USART_STOPBIT         USART_1_STOPBIT

#define EXAMPLE_USART               (&AVR32_USART1)
#define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#define EXAMPLE_USART_IRQ           AVR32_USART1_IRQ
//! @}

//! This function is used by the debugging module. It permits to print a character through the USART.
void dsp_debug_write_fct(char c)
{
  usart_putchar(EXAMPLE_USART, c);
}

//! This function is used to get a string through the USART.
char dsp_debug_read_fct()
{
  return usart_getchar(EXAMPLE_USART);
}

//! this function initializes the USART module at "EXAMPLE_USART_BAUDRATE" bauds
void dsp_debug_init(int fosc)
{
  static const gpio_map_t USART_GPIO_MAP =
  {
    {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
    {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
  };

  // Configuration structure for the USART module
  static const usart_options_t USART_OPTIONS =
  {
    // Baudrate
    .baudrate     = EXAMPLE_USART_BAUDRATE,
    // Number of bits per character
    .charlength   = EXAMPLE_USART_BITSPERCHAR,
    // Parity
    .paritytype   = EXAMPLE_USART_PARITY,
    // Stop bit
    .stopbits     = EXAMPLE_USART_STOPBIT,
    // Mode normal
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to USART.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode.
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, fosc);

  // New window
  dsp_debug_print_fct("\x0C\r\n");
}
