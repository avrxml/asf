/*****************************************************************************
 *
 * \file
 *
 * \brief Example for AT24CXX touch device.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the AT24CXX driver.
 *
 * This example demonstrates how to use the AT24CXX interface.
 *
 * \section files Main Files
 * - at24cxx_example.c: the AT24CXX example
 * - at24cxx.h: the driver interface for the AT24CXX
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a TWI can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C-EK
 * - CPU clock: 60 MHz;
 * - USART abstracted with a USB CDC connection to a PC;port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// Include Files
#include <avr32/io.h>
#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "twim.h"
#include "flashc.h"
#include "gpio.h"
#include "cycle_counter.h"
#include "at24cxx.h"
#include "conf_at24cxx.h"
#include <stdio.h>

#define FCPU_HZ         60000000
#define FPBA_HZ         60000000

/*! \name System Clock Frequencies
 */
//! @{
static pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = FCPU_HZ,
  .pba_f        = FPBA_HZ,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};
//! @}

/*! \brief Initializes the MCU system clocks.
*/
static void init_sys_clocks(void)
{
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    while(1);
}

//! TWI interface initialization
static void twi_init(void)
{
  const gpio_map_t AT24CXX_TWI_GPIO_MAP =
  {
  {AT24CXX_TWI_SCL_PIN, AT24CXX_TWI_SCL_FUNCTION},
  {AT24CXX_TWI_SDA_PIN, AT24CXX_TWI_SDA_FUNCTION}
  };

  const twi_options_t AT24CXX_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed  = AT24CXX_TWI_MASTER_SPEED,
    .chip   = AT24CXX_TWI_ADDRESS,
    .smbus  = false,
  };

  // Assign I/Os to SPI.
  gpio_enable_module(AT24CXX_TWI_GPIO_MAP,
    sizeof(AT24CXX_TWI_GPIO_MAP) / sizeof(AT24CXX_TWI_GPIO_MAP[0]));
  // Initialize as master.
  twi_master_init(AT24CXX_TWI, &AT24CXX_TWI_OPTIONS);
}

// main function
int main(void) {
  init_sys_clocks();
  init_dbg_rs232(FPBA_HZ);
  print_dbg("AT24CXX Example\r\n");

  irq_initialize_vectors();

  // Activate LED0 & LED1 & LED2 & LED3 pins in GPIO output mode and switch them off.
  gpio_set_gpio_pin(LED0_GPIO);
  gpio_set_gpio_pin(LED1_GPIO);
  gpio_set_gpio_pin(LED2_GPIO);
  gpio_set_gpio_pin(LED3_GPIO);
  twi_init();

  /* Power up delay: This device needs a maximum of 230ms before it
   * can be accessed. Another method would be to wait until the the
   * detect line goes low but this won't work during a debugging session
   * since the device is not reset.*/
  cpu_delay_ms(230, FCPU_HZ);

  at24cxx_init(FCPU_HZ);
  print_dbg("Write @0: 0x55 - @1: 0xAA");
  at24cxx_write_byte(0,0x55);
  at24cxx_write_byte(1,0xaa);
  print_dbg("- Done\r\n");

  U8 tmp1,tmp2;
  tmp1 = at24cxx_read_byte(0);
  tmp2 = at24cxx_read_byte(1);
  print_dbg("Read @0: 0x");
  print_dbg_hex(tmp1);
  print_dbg("- @1: 0x");
  print_dbg_hex(tmp2);
  while(true);
}
