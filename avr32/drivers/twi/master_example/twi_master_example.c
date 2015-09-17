/*****************************************************************************
 *
 * \file
 *
 * \brief TWI master example driver for AVR32 UC3.
 *
 * This file provides an example for the TWI on AVR32 UC3 devices.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the TWI driver single-master mode example.
 *
 * The given example uses one evaluation kit for TWI master (EVK1100, EVK1101 or
 * STK1000) and another one for TWI slave (EVK1100 or EVK1101 to program with
 * SLAVE_EXAMPLE).\n
 * The TWI master performs a few basic checks and prints the results (PASS/FAIL)
 * on USART1. The checks are:
 *    - the TWI master probes the TWI slave,
 *    - the TWI master performs a write access to the TWI slave,
 *    - the TWI master performs a read access of what it just wrote to the TWI slave,
 *    - the read data are compared with the originally written data.
 *
 * \section validationprocedure Validation Procedure
 * - Connect a UART monitor on the UART_1 of the board with TWI master
 * - Connect the TWI master with TWI slave (VCC=VCC,SDA=SDA,SCL=SCL,GND=GND)
 * - Compile a master and a slave firmware with same configuration (addr mode,speed)
 * - Program the master and slave boards
 * - RESET Slave board (=reset TWI memory)
 * - Release RESET on MASTER board
 * - on UART, you see all tests PASS
 * - Release RESET on MASTER board
 * - on UART, you see always all tests PASS.
 *
 * \section files Main Files
 * - twi.c: TWI driver;
 * - twi.h: TWI driver header file;
 * - twi_master_example.c: TWI single-master mode example application.
 *
 * \section compinfo Compilation Info
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * You can change the following defines but you should make sure that there are
 * the same on the TWI slave side for the tests to pass:
 * - TWI slave (memory) address (\ref EEPROM_ADDRESS)
 * - Address of the virtual mem in the EEPROM (\ref VIRTUALMEM_ADDR_START)
 * - Address length of the EEPROM memory (\ref EEPROM_ADDR_LGT)
 * - speed in Hz (\ref TWI_SPEED).
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a TWI module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100 or EVK1101 evaluation kit, or STK1000 starter kit;
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable;
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


#include <avr32/io.h>
#include "board.h"
#include "print_funcs.h"
#include "gpio.h"
#include "pm.h"
#include "twi.h"


//------------------  C O N F I G U R A T I O N S  -------------------

#define EEPROM_ADDRESS        0x50        // EEPROM's TWI address
#define EEPROM_ADDR_LGT       3           // Address length of the EEPROM memory
#define VIRTUALMEM_ADDR_START 0x123456    // Address of the virtual mem in the EEPROM
#define TWI_SPEED             50000       // Speed of TWI

//------------------  D E F I N I T I O N S  -------------------

//! \brief Constants to define the sent and received pattern
#define  PATTERN_TEST_LENGTH        (sizeof(test_pattern)/sizeof(U8))
const U8 test_pattern[] =  {
   0xAA,
   0x55,
   0xA5,
   0x5A,
   0x77,
   0x99};


//--------------------------------------------------------------------------------------------------------------
//----------------------------------------- T W I   M A S T E R ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

/*! \brief Main function.
 */
int main(void)
{
  static const gpio_map_t TWI_GPIO_MAP =
  {
#if BOARD == EVK1100
    {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION},
    {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION}
#elif BOARD == EVK1101
    {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION},
    {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION}
#elif BOARD == EVK1104
#error This example only supports AVR UC3 with a TWI module. UC3 A3 series have a TWIM module.
#elif BOARD == EVK1105
    {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION},
    {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION}
#elif BOARD == STK1000
    {AVR32_TWI_SDA_0_PIN, AVR32_TWI_SDA_0_FUNCTION},
    {AVR32_TWI_SCL_0_PIN, AVR32_TWI_SCL_0_FUNCTION}
#else
#  error The TWI configuration to use in this example is missing.
#endif
  };
  twi_options_t opt;
  twi_package_t packet, packet_received;
  int status, i;

  U8 data_received[PATTERN_TEST_LENGTH] = {0};

#if BOARD == EVK1100 || BOARD == EVK1101

  // Switch to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

#endif

  // Init debug serial line
  init_dbg_rs232(FOSC0);

  // Initialize and enable interrupt
  irq_initialize_vectors();
  cpu_irq_enable();

  // Display a header to user
  print_dbg("\x0C\r\nTWI Example\r\nMaster!\r\n");

  // TWI gpio pins configuration
  gpio_enable_module(TWI_GPIO_MAP, sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));

  // options settings
  opt.pba_hz = FOSC0;
  opt.speed = TWI_SPEED;
  opt.chip = EEPROM_ADDRESS;

  // initialize TWI driver with options
  status = twi_master_init(&AVR32_TWI, &opt);
  // check init result
  if (status == TWI_SUCCESS)
  {
    // display test result to user
    print_dbg("Probe test:\tPASS\r\n");
  }
  else
  {
    // display test result to user
    print_dbg("Probe test:\tFAIL\r\n");
  }

  // TWI chip address to communicate with
  packet.chip = EEPROM_ADDRESS;
  // TWI address/commands to issue to the other chip (node)
  packet.addr[0] = VIRTUALMEM_ADDR_START >> 16;
  packet.addr[1] = VIRTUALMEM_ADDR_START >> 8;
  packet.addr[2] = VIRTUALMEM_ADDR_START;
  // Length of the TWI data address segment (1-3 bytes)
  packet.addr_length = EEPROM_ADDR_LGT;
  // Where to find the data to be written
  packet.buffer = (void*) test_pattern;
  // How many bytes do we want to write
  packet.length = PATTERN_TEST_LENGTH;

  // perform a write access
  status = twi_master_write(&AVR32_TWI, &packet);

  // check write result
  if (status == TWI_SUCCESS)
  {
    // display test result to user
    print_dbg("Write test:\tPASS\r\n");
  }
  else
  {
    // display test result to user
    print_dbg("Write test:\tFAIL\r\n");
  }

  // TWI chip address to communicate with
  packet_received.chip = EEPROM_ADDRESS ;
  // Length of the TWI data address segment (1-3 bytes)
  packet_received.addr_length = EEPROM_ADDR_LGT;
  // How many bytes do we want to write
  packet_received.length = PATTERN_TEST_LENGTH;
  // TWI address/commands to issue to the other chip (node)
  packet_received.addr[0] = VIRTUALMEM_ADDR_START >> 16;
  packet_received.addr[1] = VIRTUALMEM_ADDR_START >> 8;
  packet_received.addr[2] = VIRTUALMEM_ADDR_START;
  // Where to find the data to be written
  packet_received.buffer = data_received;

  // perform a read access
  status = twi_master_read(&AVR32_TWI, &packet_received);

  // check read result
  if (status == TWI_SUCCESS)
  {
    // display test result to user
    print_dbg("Read Test:\tPASS\r\n");
  }
  else
  {
    // display test result to user
    print_dbg("Read test:\tFAIL\r\n");
  }

  // check received data against sent data
  for (i = 0 ; i < PATTERN_TEST_LENGTH; i++)
  {
    if (data_received[i] != test_pattern[i])
    {
      // a char isn't consistent
      print_dbg("Check Read:\tFAIL\r\n");
      // Error
      while(1);
    }
  }

  // everything was OK
  print_dbg("Check Read:\tPASS\r\n");

  while(1);
}
