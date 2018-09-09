/*****************************************************************************
 *
 * \file
 *
 * \brief Debugging module for the DSP library.
 * This file is used to make an abstraction of the driver used to control
 * the debugging peripheral.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
