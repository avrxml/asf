/**************************************************************************
 *
 * \file
 *
 * \brief ECCHRS example application.
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
 * defines, enums, and typedefs for the ECCHRS driver.
 *
 * This example demonstrates how to use the SMC on EBI driver with the
 * onboard SMC. It initializes the SMC, do a write access and a read back
 *
 * \section files Main Files
 * - ecchrs_example.c: the example
 * - ecchrs.h: the ECCHRS header driver
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a ECCHRS module can be used.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "cycle_counter.h"
#include "board.h"
#include "intc.h"
#include "gpio.h"
#include "nf.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"

// pointer to the ECCHRS registers
volatile avr32_ecchrs_t*    ecchrs = &AVR32_ECCHRS;

U32  nf_block_2_page(U16 block_addr);

U16 valid_block_addr[NF_N_DEVICES];


/*! \brief Initializes MCU exceptions.
 */
static void init_exceptions(void)
{
#if __GNUC__ && __AVR32__
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
#if __GNUC__ && __AVR32__
int _init_startup(void)
#elif __ICCAVR32__
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


// main function
int main(void)
{
  bool   valid_block_found[NF_N_DEVICES];
  U32    u32_nf_ids, i_dev, i_block;
  U8     maker_id, device_id;
  U32    i, j;
  // ECCHRS options.
  static const ecchrs_options_t ECCHRS_OPTIONS =
  {
    .typecorrect = ECCHRS_TYPECORRECT_4_BIT,
    .pagesize    = ECCHRS_PAGESIZE_4_BIT_2112_W
  };

  // switch to oscillator 0
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

   // init debug serial interface
  init_dbg_rs232(FOSC0);

  nf_init(FOSC0); // init the nand flash driver with the correct HSB frequency
  nf_unprotect();

  print_dbg("\r\nECCHRS example using Nand Flash.\r\n================================\r\n\r\n");

  // - Simple test of the NF communication through the SMC.
  // - Find all bad blocks.
  // - Find a valid block for the remaining tests.
  nf_reset_nands(NF_N_DEVICES);

  print_dbg("\tCPU, HSB is at 12000000Mhz.\r\n");
  print_dbg("\tPBA, PBB is at 12000000Mhz.\r\n");

  print_dbg("\tDetecting Nand Flash device(s).\r\n");
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    // Performs some init here...
    valid_block_found[i_dev] = false;

    // Test Maker and Device ids
    u32_nf_ids = nf_read_id( NF_READ_ID_CMD, i_dev );
    maker_id  = MSB0(u32_nf_ids);
    device_id = MSB1(u32_nf_ids);
    print_dbg("\t\tNF");
    print_dbg_hex(i_dev);
    print_dbg(" [Maker=");
    print_dbg_hex(maker_id);
    print_dbg("] [Device=");
    print_dbg_hex(device_id);
    print_dbg("]\r\n");
    if( maker_id==M_ID_MICRON )
    {
      print_dbg("\t\t       Micron chip");
      if( device_id==0xDA ){
        print_dbg("- MT29F2G08AACWP device\r\n");
      }
      else
      {
       print_dbg("- *** Error: unexpected chip detected. Please check the board settings and hardware.\r\n");
        return -1;
      }
    }
    else
    {
      print_dbg("\t\t       *** Error: unexpected chip detected. Please check the board settings and hardware.\r\n");
      return -1;
    }
  }

  // Looking for valid blocks for the test.
  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
  {
    nf_select(i_dev);
    for( i_block=0 ; i_block<G_N_BLOCKS ; i_block++ )
    {
      nf_open_page_read( nf_block_2_page(i_block), NF_SPARE_POS + G_OFST_BLK_STATUS );
      if( (nf_rd_data()==0xFF) )
      { // The block is valid.
        print_dbg("\tValid block found (");
        print_dbg_ulong(i_block);
        print_dbg(") on NF ");
        print_dbg_hex(i_dev);
        print_dbg("\r\n");
        valid_block_found[i_dev]= true;
        valid_block_addr[i_dev] = i_block;
        break;
      }
    }
  }

  for( i_dev=0 ; i_dev<NF_N_DEVICES ; i_dev++ )
    if( !valid_block_found[i_dev] )
    {
      print_dbg("Error: no valid blocks found.\r\n");
      return 0;
    }

  print_dbg("\tECCHRS IP ver ");
  print_dbg_ulong(ecchrs->version);
  print_dbg("(");
  print_dbg_hex(ecchrs->version);
  print_dbg(")");
  print_dbg("\r\n");


  // Work with NF 0 from now...
  nf_select(0);

  // Setup the ECCHRS.
  ecchrs_init(&AVR32_ECCHRS, &ECCHRS_OPTIONS);

  // Ensures that the block is erased for the test.
  print_dbg("\tErasing a free block for the test.\r\n");
  nf_erase_block(nf_block_2_page(valid_block_addr[0]), false);

  // Reset the ECCHRS state machine.
  ecchrs_reset(&AVR32_ECCHRS);

  // Program a simple patterns in the first page.
  print_dbg("\tProgramming the first page with a simple pattern.\r\n");
  nf_open_page_write( nf_block_2_page(valid_block_addr[0]), 0);
  for ( i = 0; i < 2048/2; i++ )
  {
    U16 val = i;
    nf_wr_data(MSB(val));
    nf_wr_data(LSB(val));
  }

  // Extract the ECCs and store them at the end of the page.
  // [2054; 2063]: codeword  0:9
  // [2070; 2079]: codeword 10:19
  // [2086; 2095]: codeword 20:29
  // [2102; 2111]: codeword 30:39
  // Since we use the Random Data Output command, we need to freeze
  // the ECCHRS in order to keep our ECC codewords unchanged.
  print_dbg("\tExtracting the ECCHRS codewords and store them in the page.\r\n");
  ecchrs_freeze(&AVR32_ECCHRS);  // not needed if ECCHRS reaches the end of page.
  for ( i=0 ; i<4 ; i++ )
  {
    U16 offset = 2048+6+16*i;
    nf_wr_cmd(NF_RANDOM_DATA_INPUT_CMD);
    nf_wr_addr( LSB(offset) );
    nf_wr_addr( MSB(offset) );
    for ( j=0 ; j<10 ; j++ )
      nf_wr_data( ecchrs_get_cw(&AVR32_ECCHRS, i*10+j) );
  }
  ecchrs_unfreeze(&AVR32_ECCHRS);

  nf_wr_cmd(NF_PAGE_PROGRAM_CMD);


  // Now let's test the ECC verification.
  print_dbg("\tReading the first page.\r\n");
  nf_open_page_read( nf_block_2_page(valid_block_addr[0]), 0);
  for( i=0 ; i<2048 ; i++ )
    nf_rd_data();

  print_dbg("\tChecking if the data are valid thanks to the ECCHRS codewords.\r\n");
  for ( i=0 ; i<4 ; i++ )
  {
    U16 offset = 2048+6+16*i;
    ecchrs_freeze(&AVR32_ECCHRS);
    nf_wr_cmd(NF_RANDOM_READ_CMD_C1);
    nf_wr_addr( LSB(offset) );
    nf_wr_addr( MSB(offset) );
    nf_wr_cmd(NF_RANDOM_READ_CMD_C2);
    ecchrs_unfreeze(&AVR32_ECCHRS);
    for ( j=0 ; j<10 ; j++ )
      nf_rd_data();
  }

  // Check if there is any errors after the read of the page.
  i = ecchrs_4bit_check_error(&AVR32_ECCHRS);
  print_dbg("\tSR1 is  ");
  print_dbg_ulong(i);
  print_dbg("(");
  print_dbg_hex(i);
  print_dbg(")");
  print_dbg("\r\n");

  if(i&(15))
  {
   print_dbg("\tERROR: ECCHRS detects some errors in the sectors 1, 2, 3 or 4\r\n");
  }
  else
   print_dbg("\tNo error detected.\r\n");

  // Let the block free.
  nf_erase_block(nf_block_2_page(valid_block_addr[0]), false);
  return 0;
}
