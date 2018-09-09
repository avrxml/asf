/*****************************************************************************
 *
 * \file
 *
 * \brief Example for AT24CXX touch device.
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
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
