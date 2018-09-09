/*****************************************************************************
 *
 * \file
 *
 * \brief Strings and integers print module for debug purposes.
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


#include "compiler.h"
#include "gpio.h"
#include "usart.h"
#include "print_funcs.h"


//! ASCII representation of hexadecimal digits.
static const char HEX_DIGITS[16] = "0123456789ABCDEF";


void init_dbg_rs232(long pba_hz)
{
  init_dbg_rs232_ex(DBG_USART_BAUDRATE, pba_hz);
}


void init_dbg_rs232_ex(unsigned long baudrate, long pba_hz)
{
  static const gpio_map_t DBG_USART_GPIO_MAP =
  {
    {DBG_USART_RX_PIN, DBG_USART_RX_FUNCTION},
    {DBG_USART_TX_PIN, DBG_USART_TX_FUNCTION}
  };

  // Options for debug USART.
  usart_options_t dbg_usart_options =
  {
    .baudrate = baudrate,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Setup GPIO for debug USART.
  gpio_enable_module(DBG_USART_GPIO_MAP,
                     sizeof(DBG_USART_GPIO_MAP) / sizeof(DBG_USART_GPIO_MAP[0]));

  // Initialize it in RS232 mode.
  usart_init_rs232(DBG_USART, &dbg_usart_options, pba_hz);
}


void print_dbg(const char *str)
{
  // Redirection to the debug USART.
  print(DBG_USART, str);
}


void print_dbg_char(int c)
{
  // Redirection to the debug USART.
  print_char(DBG_USART, c);
}


void print_dbg_ulong(unsigned long n)
{
  // Redirection to the debug USART.
  print_ulong(DBG_USART, n);
}


void print_dbg_char_hex(unsigned char n)
{
  // Redirection to the debug USART.
  print_char_hex(DBG_USART, n);
}


void print_dbg_short_hex(unsigned short n)
{
  // Redirection to the debug USART.
  print_short_hex(DBG_USART, n);
}


void print_dbg_hex(unsigned long n)
{
  // Redirection to the debug USART.
  print_hex(DBG_USART, n);
}


void print(volatile avr32_usart_t *usart, const char *str)
{
  // Invoke the USART driver to transmit the input string with the given USART.
  usart_write_line(usart, str);
}


void print_char(volatile avr32_usart_t *usart, int c)
{
  // Invoke the USART driver to transmit the input character with the given USART.
  usart_putchar(usart, c);
}


void print_ulong(volatile avr32_usart_t *usart, unsigned long n)
{
  char tmp[11];
  int i = sizeof(tmp) - 1;

  // Convert the given number to an ASCII decimal representation.
  tmp[i] = '\0';
  do
  {
    tmp[--i] = '0' + n % 10;
    n /= 10;
  } while (n);

  // Transmit the resulting string with the given USART.
  print(usart, tmp + i);
}


void print_char_hex(volatile avr32_usart_t *usart, unsigned char n)
{
  char tmp[3];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[2] = '\0';
  for (i = 1; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}


void print_short_hex(volatile avr32_usart_t *usart, unsigned short n)
{
  char tmp[5];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[4] = '\0';
  for (i = 3; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}


void print_hex(volatile avr32_usart_t *usart, unsigned long n)
{
  char tmp[9];
  int i;

  // Convert the given number to an ASCII hexadecimal representation.
  tmp[8] = '\0';
  for (i = 7; i >= 0; i--)
  {
    tmp[i] = HEX_DIGITS[n & 0xF];
    n >>= 4;
  }

  // Transmit the resulting string with the given USART.
  print(usart, tmp);
}
