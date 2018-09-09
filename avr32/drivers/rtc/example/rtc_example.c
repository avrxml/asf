/*****************************************************************************
 *
 * \file
 *
 * \brief RTC example application for AVR32 UC3.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the real time counter driver. It also comes
 * bundled with an application-example of usage.
 *
 * This example demonstrates how to use the RTC driver with the 32 KHz oscillator crystal.
 * <b>Operating mode: </b>It uses the USART driver to display a real time counter incremented every second.
 * The display is triggered by the interrupt controller module.
 *
 * \section files Main Files
 * - rtc.c: real time counter driver;
 * - rtc.h: real time counter driver header file;
 * - rtc_example.c: real time counter example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an RTC, an INTC and a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, EVK1104 evaluation kits;
 * - CPU clock: 12 MHz;
 * - USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a standard
 *   RS232 DB9 cable, or USART0 (on EVK1105) or USART1 (on EVK1104) abstracted
 *   with a USB CDC connection to a PC;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include <avr32/io.h>
#if __GNUC__
#  include "intc.h"
#endif
#include "board.h"
#include "compiler.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"
#include "pm.h"


/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#elif BOARD == EVK1101
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#elif BOARD == EVK1104
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#endif

#if !defined(EXAMPLE_USART)             || \
    !defined(EXAMPLE_USART_RX_PIN)      || \
    !defined(EXAMPLE_USART_RX_FUNCTION) || \
    !defined(EXAMPLE_USART_TX_PIN)      || \
    !defined(EXAMPLE_USART_TX_FUNCTION)
#  error The USART configuration to use in this example is missing.
#endif
//! @}


// To specify we have to print a new time
volatile static int print_sec = 1;

// Time counter
static int sec = 0;

#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
/* RTC Interrupt  */
#pragma handler = AVR32_RTC_IRQ_GROUP, 1
__interrupt
#endif
void rtc_irq(void)
{
  // Increment the minutes counter
  sec++;

  // clear the interrupt flag
  rtc_clear_interrupt(&AVR32_RTC);

  // specify that an interrupt has been raised
  print_sec = 1;
}

/*!
 * \brief print_i function : convert the given number to an ASCII decimal representation.
 */
char *print_i(char *str, unsigned int n)
{
  int i = 10;

  str[i] = '\0';
  do
  {
    str[--i] = '0' + n%10;
    n /= 10;
  }while(n);

  return &str[i];
}

/*!
 * \brief main function : do init and loop (poll if configured so)
 */
int main( void )
{
  char temp[20];
  char *ptemp;

  static const gpio_map_t USART_GPIO_MAP =
  {
    {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
    {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
  };

  // USART options
  static const usart_options_t USART_OPTIONS =
  {
    .baudrate     = 57600,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = 0
  };

  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Assign GPIO pins to USART0.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode at 12MHz.
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, 12000000);

  // Welcome sentence
  usart_write_line(EXAMPLE_USART, "\x1B[2J\x1B[H\r\nATMEL\r\n");
  usart_write_line(EXAMPLE_USART, "AVR32 UC3 - RTC example\r\n");

  usart_write_line(EXAMPLE_USART, "RTC 32 KHz oscillator program test.\r\n");

  // Disable all interrupts. */
  Disable_global_interrupt();

  // The INTC driver has to be used only for GNU GCC for AVR32.
#if __GNUC__
  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Register the RTC interrupt handler to the interrupt controller.
  INTC_register_interrupt(&rtc_irq, AVR32_RTC_IRQ, AVR32_INTC_INT0);
#endif

  // Initialize the RTC
  if (!rtc_init(&AVR32_RTC, RTC_OSC_32KHZ, RTC_PSEL_32KHZ_1HZ))
  {
    usart_write_line(&AVR32_USART0, "Error initializing the RTC\r\n");
    while(1);
  }
  // Set top value to 0 to generate an interrupt every seconds */
  rtc_set_top_value(&AVR32_RTC, 0);
  // Enable the interrupts
  rtc_enable_interrupt(&AVR32_RTC);
  // Enable the RTC
  rtc_enable(&AVR32_RTC);

  // Enable global interrupts
  Enable_global_interrupt();

  while(1)
  {
    if (print_sec)
    {
      // Set cursor to the position (1; 5)
      usart_write_line(EXAMPLE_USART, "\x1B[5;1H");
      ptemp = print_i(temp, sec);
      usart_write_line(EXAMPLE_USART, "Timer: ");
      usart_write_line(EXAMPLE_USART, ptemp);
      usart_write_line(EXAMPLE_USART, "s");
      print_sec = 0;
    }
  }
}
