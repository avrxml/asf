/*****************************************************************************
 *
 * \file
 *
 * \brief TWI slave example driver for AVR32 UC3.
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
 * defines, enums, and typedefs for the TWI driver slave mode example.
 *
 * The given example uses one evaluation kit for TWI master (EVK1100, EVK1101 or
 * STK1000 to program with MASTER_EXAMPLE) and another one for TWI slave
 * (EVK1100 or EVK1101).\n
 * The TWI slave initializes the TWI as slave and registers its receive and transmit
 * callbacks. It displays on USART1 a status message upon initialization.
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
 * - twi_slave_example.c: TWI slave mode example application.
 *
 * \section compinfo Compilation Info
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * You can change the following defines but you should make sure that there are
 * the same on the TWI master side for the tests to pass:
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
 * - EVK1100 or EVK1101 evaluation kit;
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
//------------------------------------------ T W I   S L A V E -------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

//! Defines & Variables to manage a virtual TWI memory
#define TWI_MEM_SIZE    20 // The size of the virtual mem
#define TWI_MEM_IDLE    0  // Idle state
#define TWI_MEM_ADDR    1  // Address state
#define TWI_MEM_DATA    2  // Data state
U8  s_status_cmd = TWI_MEM_IDLE; // State variable
U8  s_u8_addr_pos;               // Offset in the address value (because we receive the address one Byte at a time)
U32 s_u32_addr;                  // The current address in the virtual mem
U8  s_memory[TWI_MEM_SIZE]={0};  // Content of the Virtual mem


/*! \brief Manage the received data on TWI
 */
static void  twi_slave_rx( U8 u8_value )
{

   switch( s_status_cmd )
   {
   case TWI_MEM_IDLE:
      s_u8_addr_pos = EEPROM_ADDR_LGT; // Init before receiving the target address.
      s_u32_addr = 0;
      // No break to continue on next case

   case TWI_MEM_ADDR:
      s_u8_addr_pos--;
      // Receiving the Nth Byte that makes the address (MSB first).
      s_u32_addr += ((U32)u8_value << (s_u8_addr_pos*8));
      if( 0 == s_u8_addr_pos )
      {  // the address is completely received => switch to data mode.
         s_status_cmd = TWI_MEM_DATA;
      }else{
         s_status_cmd = TWI_MEM_ADDR;
      }
      break;

   case TWI_MEM_DATA:      // We are receiving data
      // Check that we're still in the range of the virtual mem
      if( TWI_MEM_SIZE > (s_u32_addr-VIRTUALMEM_ADDR_START) )
      {
         s_memory[s_u32_addr-VIRTUALMEM_ADDR_START] = u8_value;
      }
      s_u32_addr++;  // Update to next position
      break;
   }
}

/*! \brief Transmit a data on TWI
 */
static U8 twi_slave_tx( void )
{
   U8 u8_value;

   // This callback is called after a read request from the TWI master, for each
   // Byte to transmit.
   s_status_cmd = TWI_MEM_DATA;
   // Check that we're still in the range of the virtual mem
   if( TWI_MEM_SIZE > (s_u32_addr-VIRTUALMEM_ADDR_START) )
   {
      u8_value = s_memory[s_u32_addr-VIRTUALMEM_ADDR_START];
   }else{
      u8_value = 0xFF;
   }
   s_u32_addr++;  // Update to next position
   return u8_value;
}


/*! \brief Manage stop transfer reception on TWI
 */
static void twi_slave_stop( void )
{
   s_status_cmd = TWI_MEM_IDLE;
}


/*! \brief Main function.
 */
int main(void)
{
  static const gpio_map_t TWI_GPIO_MAP =
  {
    {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION},
    {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION}
  };
  twi_options_t opt;
  twi_slave_fct_t twi_slave_fct;
  int status;

  // Switch to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Init debug serial line
  init_dbg_rs232(FOSC0);

  // Initialize and enable interrupt
  irq_initialize_vectors();
  cpu_irq_enable();

  // Display a header to user
  print_dbg("\x0C\r\nTWI Example\r\nSlave!\r\n");

  // TWI gpio pins configuration
  gpio_enable_module(TWI_GPIO_MAP, sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));

  // options settings
  opt.pba_hz = FOSC0;
  opt.speed = TWI_SPEED;
  opt.chip = EEPROM_ADDRESS;

  // initialize TWI driver with options
  twi_slave_fct.rx = &twi_slave_rx;
  twi_slave_fct.tx = &twi_slave_tx;
  twi_slave_fct.stop = &twi_slave_stop;
  status = twi_slave_init(&AVR32_TWI, &opt, &twi_slave_fct );
  // check init result
  if (status == TWI_SUCCESS)
  {
    // display test result to user
    print_dbg("Slave start:\tPASS\r\n");
  }
  else
  {
    // display test result to user
    print_dbg("slave start:\tFAIL\r\n");
  }

  while(1);
}
