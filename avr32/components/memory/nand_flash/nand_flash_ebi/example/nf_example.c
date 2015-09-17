/**************************************************************************
 *
 * \file
 *
 * \brief Nand Flash example.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the NAND FLASH components.
 *
 * The given example covers simple uses of a external NAND FLASH using EBI.
 *
 * This example operates at 12MHz.
 *
 * \section files Main Files
 * - nf.c: NAND FLASH driver;
 * - nf.h: NAND FLASH driver header file;
 * - CONF/conf_nf.h: NAND FLASH configuration file
 * - nf_2KB.h: Listing of 2-kB NAND FLASH types
 * - nf_512B.h: Listing of 512B NAND FLASH types
 * - nf_example.c: NAND FLASH example application
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a EBI module can be used. This example has been tested
 * with the following setup:
 *   - EVK1104 evaluation kit;
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include <stdio.h>
#include "usart.h"
#include "print_funcs.h"
#include "cycle_counter.h"
#include "board.h"
#include "intc.h"
#include "flashc.h"
#include "pm.h"
#include "gpio.h"
#include "nf.h"
#include "stdio_serial.h"
#include "conf_stdio_serial.h"

/*! \brief Initializes MCU exceptions.
 */
static void init_exceptions(void)
{
#if (defined __GNUC__) && (defined __AVR32__)
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system
  // register.
  Set_system_register(AVR32_EVBA, (int)&_evba);
#endif

  // Enable exceptions globally.
  Enable_global_exception();
}


/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  // Set flashc master type to last default to save one cycle for
  // each branch.
  avr32_hmatrix_scfg_t scfg;

  scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH];
  scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH] = scfg;

  scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EBI];
  scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_EBI] = scfg;
}


/*! \brief Initializes STDIO.
 */
static void init_stdio(void)
{
  // USART options.
  static usart_serial_options_t USART_SERIAL_OPTIONS =
  {
    .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
    .charlength   = USART_SERIAL_CHAR_LENGTH,
    .paritytype   = USART_SERIAL_PARITY,
    .stopbits     = USART_SERIAL_STOP_BIT
  };

  // Initialize Serial Interface using Stdio Library
  stdio_serial_init(USART_SERIAL_EXAMPLE,&USART_SERIAL_OPTIONS);
}


/*! \brief Initializes MCU interrupts.
 */
static void init_interrupts(void)
{
  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Enable interrupts globally.
  Enable_global_interrupt();
}


/*! \brief Low-level initialization routine called during startup, before the
 *         \ref main function.
 */
#if (defined __GNUC__) && (defined __AVR32__)
int _init_startup(void)
#elif (defined __ICCAVR32__)
int __low_level_init(void)
#endif
{
  init_exceptions();
  init_hmatrix();
  init_interrupts();

  // EWAVR32: Request initialization of data segments.
  // GCC: Don't-care value.
  return 1;
}



/*! \brief Convert a block number into a page number.
 */
U32 nf_block_2_page(U16 block_addr)
{
   //#define G_SHIFT_BLOCK_PAGE 6
   return (U32)block_addr<<G_SHIFT_BLOCK_PAGE;
}

U16 valid_block_addr[NF_N_DEVICES];


/*! \brief Main function.
 */
int main(void)
{
  bool   valid_block_found[NF_N_DEVICES];
  U32    u32_nf_ids, i_dev, i_block;
  U8     maker_id, device_id, byte3;
  U32    i, time_s, time_e;
  U8     data;

  // start init
  sysclk_init();

  // Enable clock for require module
  sysclk_enable_hsb_module(SYSCLK_EBI);
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
  sysclk_enable_pbb_module(SYSCLK_HMATRIX);

  // Initialize the board.
  // The board-specific conf_board.h file contains the configuration of the board
  // initialization.
  board_init();

  init_stdio();

#ifdef NF_ADDON // addon nandflash board for EVK1104 (not mounted by default)
  // Unselect NF on board
  gpio_set_gpio_pin(AVR32_PIN_PX53);
  gpio_set_gpio_pin(AVR32_PIN_PX52);
#endif

  nf_init(sysclk_get_cpu_hz());
  nf_unprotect();

  printf("\x0C");
  printf("Nand Flash example.\r\n===================\r\n\r\n");

  // - Simple test of the NF communication through the SMC.
  // - Find all bad blocks.
  // - Find a valid block for the remaining tests.
  nf_reset_nands(NF_N_DEVICES);

  printf("\tDetecting Nand Flash device(s).\r\n");
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    // Performs some init here...
    valid_block_found[i_dev] = false;

    // Test Maker and Device ids
    u32_nf_ids = nf_read_id( NF_READ_ID_CMD, i_dev );
    maker_id  = MSB0(u32_nf_ids);
    device_id = MSB1(u32_nf_ids);
    byte3     = MSB3(u32_nf_ids);
    printf("\t\tNF %ld: [Maker=0x%02x] [Device=0x%02x] [byte3=0x%02x]\r\n", i_dev, maker_id, device_id, byte3);
    if( maker_id==M_ID_MICRON )
    {
      printf("\t\t       Micron chip");
      if( (device_id==0xDA) && (byte3==0x15) )
        printf("- MT29F2G08AACWP device\r\n");
      else if( (device_id==0xDA) && (byte3==0x95) )
        printf("- MT29F2G08AADWP device\r\n");
      else
      {
        printf("- *** Error: unexpected chip detected. Please check the board settings and hardware.\r\n");
        return -1;
      }
    }
    else
    {
      printf("\t\t       *** Error: unexpected chip detected. Please check the board settings and hardware.\r\n");
      return -1;
    }
  }

  printf("\r\n\tTesting bad blocks.\r\n");
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    printf("\t\tNF %ld:\r\n", i_dev);
    nf_select(i_dev);
    for( i_block=0 ; i_block<G_N_BLOCKS ; i_block++ )
    {
      nf_open_page_read( nf_block_2_page(i_block), NF_SPARE_POS + G_OFST_BLK_STATUS );
      if( (nf_rd_data()!=0xFF) )
      { // The block is bad.
        printf("\t\t\tBlock %ld (0x%lx) is bad.\r\n", i_block, i_block);
      }
      else
      {
        if( !valid_block_found[i_dev] )
        {
          valid_block_found[i_dev]= true;
          valid_block_addr[i_dev] = i_block;
          printf("\t\t\tFirst valid block is at address %ld (0x%lx).\r\n", i_block, i_block);
        }
      }
    }
  }

  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
    if( !valid_block_found[i_dev] )
    {
      printf("Error %d\r\n", __LINE__);
      return 0;
    }



  // - Ensure good NF behaviour through simple commands.
  //   Erase, Program, Read
  // - Measures NF timings.
  printf("\r\n\tMeasuring NF timings.\r\n");
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    printf("\t\tNF %ld:\r\n", i_dev);
    nf_select(i_dev);
    nf_erase_block( nf_block_2_page(valid_block_addr[0]), false);
    time_s = Get_sys_count();
    nf_wait_busy();
    time_e = Get_sys_count();

    // Verify that the block is erased.
    nf_open_page_read( nf_block_2_page(valid_block_addr[0]), 0);
    for( i= 0; i<2048 ; i++ )
    {
      data = nf_rd_data();
      if( data!=0xFF )
      {
        printf("\tError: offset %d is not erased (read %d).\r\n", (U8)i, data);
        return 0;
      }
    }
    printf("\t\t\tTime to erase a page:%ld cy (%ld us)\r\n", time_e-time_s, cpu_cy_2_us(time_e-time_s, sysclk_get_cpu_hz()));

    nf_open_page_write( nf_block_2_page(valid_block_addr[0]), 0);
    for( i=0 ; i<2048 ; i++ )
      nf_wr_data(i%256);
    nf_wr_cmd(NF_PAGE_PROGRAM_CMD);

    time_s = Get_sys_count();
    nf_wait_busy();
    time_e = Get_sys_count();
    printf("\t\t\tTime to program a page:%ld cy (%ld us)\r\n", time_e-time_s, cpu_cy_2_us(time_e-time_s, sysclk_get_cpu_hz()));

    time_s = Get_sys_count();
    nf_open_page_read( nf_block_2_page(valid_block_addr[0]), 0);
    time_e = Get_sys_count();
    printf("\t\t\tTime to access to a page:%ld cy (%ld us)\r\n", time_e-time_s, cpu_cy_2_us(time_e-time_s, sysclk_get_cpu_hz()));
    for( i= 0; i<2048 ; i++ )
    {
      data = nf_rd_data();
      if( data!= i%256)
      {
        printf("\tError: expect %d, read %d\r\n", (U8)i, data);
        return 0;
      }
    }
  }

  printf("Example DONE\r\n");

  return 0;
}
