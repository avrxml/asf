/*****************************************************************************
 *
 * \file
 *
 * \brief SD/MMC card driver example application.
 *
 * This file gives an example of using the SD/MMC card driver.
 * If a SD or a MMC card is detected, a message is sent to user
 * after that it copies data from SD/MMC to RAM.
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
 * This is the documentation for the data structures, functions, variables, defines, enums, and
 * typedefs for the SD/MMC driver in MCI mode. <BR>It also comes with an example of the usage of this driver.<BR>
 *
 * This file gives an example of using the SD/MMC card driver.
 * If a SD/MMC card is detected, the main() function reads data from SD/MMC
 * to internal RAM. It displays on RS232 the first
 * sector from SD/MMC physical sector number 1.
 *
 * \section files Main Files
 * - sd_mmc_mci.c: SD/MMC driver;
 * - sd_mmc_mci.h: SD/MMC driver header file;
 * - mci.c : MCI driver
 * - mci.h : MCI driver header file
 * - sd_mmc_mci_mem.c: SD/MMC memory interface;
 * - sd_mmc_mci_mem.h: SD/MMC memory interface header file;
 * - sd_mmc_example.c: SD/MMC application example ;
 * - conf_access.h: Configuration for CTRL_ACCESS module;
 * - conf_sd_mmc_mci.h: Configuration for SD/MMC module;
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an USART and a MCI module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1104 evaluation kit;
 * - CPU clock: 12 MHz;
 * - USARTx (on EVK1104 ) connected to a PC serial port via a standard RS232 DB9 cable;
 * - SD/MMC 8-bits Slot (on EVK1104 )
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
#include "pm.h"
#include "gpio.h"
#include "intc.h"
#include "print_funcs.h"
#include "sd_mmc_mci.h"
#include "sd_mmc_mci_mem.h"
#include "cycle_counter.h"

//! Configuration file for the example
#include "conf_sd_mmc_mci.h"

//! \brief PBB clock frequency (Hz)
#define APPLI_CPU_SPEED      48000000
#define APPLI_PBA_SPEED      48000000
#define PBB_HZ               pm_freq_param.pba_f
#define BENCH_TIME_MS        3000  // unit is in ms.
#define BENCH_START_SECTOR   0x15000
#define EXAMPLE_SD_SLOT      SD_SLOT_4BITS // SD_SLOT_8BITS or SD_SLOT_4BITS
#define ALLOCATED_SECTORS    16

#if (defined __GNUC__) && (defined __AVR32__)
  __attribute__((__aligned__(4)))
#elif (defined __ICCAVR32__)
  #pragma data_alignment = 4
#endif
  U8 buffer_in[SD_MMC_SECTOR_SIZE*ALLOCATED_SECTORS];

#if (defined __GNUC__) && (defined __AVR32__)
  __attribute__((__aligned__(4)))
#elif (defined __ICCAVR32__)
  #pragma data_alignment = 4
#endif
  U8 buffer_saved[SD_MMC_SECTOR_SIZE];

pm_freq_param_t   pm_freq_param=
{
   .cpu_f  =       APPLI_CPU_SPEED
,  .pba_f    =     APPLI_PBA_SPEED
,  .osc0_f     =   FOSC0
,  .osc0_startup = OSC0_STARTUP
};

/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  // Set flashc master type to last default to save one cycle for
  // each branch.
  avr32_hmatrix_scfg_t scfg = AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH];
  scfg.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.SCFG[AVR32_HMATRIX_SLAVE_FLASH] = scfg;
}


/*! \brief Initializes SD/MMC resources: GPIO, MCI and SD/MMC.
 */
static void sd_mmc_mci_resources_init(void)
{
  // GPIO pins used for SD/MMC interface
#if EXAMPLE_SD_SLOT == SD_SLOT_8BITS
  static const gpio_map_t SD_MMC_MCI_GPIO_MAP =
  {
    {SD_SLOT_8BITS_CMD_PIN,   SD_SLOT_8BITS_CMD_FUNCTION },    // CMD Pin.
    {SD_SLOT_8BITS_CLK_PIN,   SD_SLOT_8BITS_CLK_FUNCTION},     // CLOCK Pin.
    {SD_SLOT_8BITS_DATA0_PIN, SD_SLOT_8BITS_DATA0_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA1_PIN, SD_SLOT_8BITS_DATA1_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA2_PIN, SD_SLOT_8BITS_DATA2_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA3_PIN, SD_SLOT_8BITS_DATA3_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA4_PIN, SD_SLOT_8BITS_DATA4_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA5_PIN, SD_SLOT_8BITS_DATA5_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA6_PIN, SD_SLOT_8BITS_DATA6_FUNCTION},   // DATA Pin.
    {SD_SLOT_8BITS_DATA7_PIN, SD_SLOT_8BITS_DATA7_FUNCTION}    // DATA Pin.
  };
#elif EXAMPLE_SD_SLOT == SD_SLOT_4BITS
    static const gpio_map_t SD_MMC_MCI_GPIO_MAP =
  {
    {SD_SLOT_4BITS_CMD_PIN,   SD_SLOT_4BITS_CMD_FUNCTION },    // CMD Pin.
    {SD_SLOT_4BITS_CLK_PIN,   SD_SLOT_4BITS_CLK_FUNCTION},     // CLOCK Pin.
    {SD_SLOT_4BITS_DATA0_PIN, SD_SLOT_4BITS_DATA0_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA1_PIN, SD_SLOT_4BITS_DATA1_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA2_PIN, SD_SLOT_4BITS_DATA2_FUNCTION},   // DATA Pin.
    {SD_SLOT_4BITS_DATA3_PIN, SD_SLOT_4BITS_DATA3_FUNCTION}    // DATA Pin.
  };
#else
# error SD_SLOT not supported
#endif

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
                     sizeof(SD_MMC_MCI_GPIO_MAP) / sizeof(SD_MMC_MCI_GPIO_MAP[0]));

  // Initialize SD/MMC driver with MCI clock (PBB).
  sd_mmc_mci_init(EXAMPLE_SD_SLOT, PBB_HZ, pm_freq_param.cpu_f);
}

/*! \brief Display results in bps
 */
static void display_perf_bps(U32 bandwidth)
{
  if( bandwidth<10000 )
  {
    print_dbg("Bandwidth: ");
    print_dbg(CL_GREEN);
    print_dbg_ulong( bandwidth );
    print_dbg(CL_BLACK);
    print_dbg(" Bps\r\n");
  }
  /*
  else if( bandwidth<10000000 )
  {
    print_dbg("Bandwidth: ");
    print_dbg(CL_GREEN);
    print_dbg_ulong( bandwidth/1000 );
    print_dbg(CL_BLACK);
    print_dbg(" KBps\r\n");
  }*/
  else
  {
    print_dbg("Bandwidth: ");
    print_dbg(CL_GREEN);
    //print_dbg_ulong( bandwidth/1000000 );
    print_dbg_ulong( bandwidth/1000 );
    print_dbg(CL_BLACK);
    print_dbg(" KBps\r\n");
    //print_dbg(" MBps\r\n");
  }
}

static bool test_sd_mmc_write(U8 pattern_id)
{
  U16 i = 0;
  U16 j = 0;
  bool b_error;
  Ctrl_status status;

  print_dbg("Testing pattern ");
  print_dbg_char_hex( pattern_id );
  // Backup sector 0.
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_saved);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR 001: can not read device.\r\n");
    return false;
  }

  // Initialize pattern.
  for( i=0 ; i<SD_MMC_SECTOR_SIZE ; i++ )
  {
    switch( pattern_id )
    {
    case 0:
      buffer_in[i] = i;
      break;

    case 1:
      buffer_in[i] = 0xAA;
      break;

    case 2:
      buffer_in[i] = 0x55;
      break;

    default:
      buffer_in[i] = (256-(U8)i);
      break;
    }
  }

  // Write the pattern into the sector.
  status = sd_mmc_mci_ram_2_mem(EXAMPLE_SD_SLOT, 0, buffer_in);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not write device.\r\n");
    return false;
  }

  // Clear buffer before comparing.
  for( i=0 ; i<SD_MMC_SECTOR_SIZE ; i++ )
    buffer_in[i] = 0;

  // Read back the sector.
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR 002: can not read device.\r\n");
    return false;
  }

  // Check the sector content.
  b_error = false;
  for( i=0 ; i<SD_MMC_SECTOR_SIZE ; i++ )
  {
    switch( pattern_id )
    {
    case 0:
      if( buffer_in[i]!=(i%256) )
        b_error = true;
      break;

    case 1:
      if( buffer_in[i]!=0xAA )
        b_error = true;
      break;

    case 2:
      if( buffer_in[i]!=0x55 )
        b_error = true;
      break;

    default:
      if( buffer_in[i]!=((256-(U8)i)%256) )
        b_error = true;
      break;
    }

    if( b_error )
    {
      print_dbg("\r\nERROR: pattern comparison failed.\r\n");
      for( i=0 ; i<SD_MMC_SECTOR_SIZE ; i++ )
      {
        print_dbg_char_hex(buffer_in[i]);
        j++;
        if (j%32==0)
          print_dbg("\r\n"), j=0;
        else if (j%4==0)
          print_dbg(" ");
      }
      return false;
    }
  }

  // Restore back the old sector content.
  status = sd_mmc_mci_ram_2_mem(EXAMPLE_SD_SLOT, 0, buffer_saved);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not write device.\r\n");
    return false;
  }

  // Check that the sector restore is successful.
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR 003: can not read device.\r\n");
    return false;
  }
  for (i=0;i<(512);i++)
  {
    if( buffer_in[i]!=buffer_saved[i] )
    {
      print_dbg("\r\nERROR: old sector is not correctly restored.\r\n");
      return false;
    }
  }
  print_dbg(" [PASS].\r\n");
  return true;
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  U32 n_sector = 0;
  U32 card_size; // Unit is in sector.
  U32 bench_start_sector;
  U16 i = 0;
  U16 j = 0;
  t_cpu_time timer;
  Ctrl_status status;

  // Set CPU and PBA clock
  if( PM_FREQ_STATUS_FAIL==pm_configure_clocks(&pm_freq_param) )
     return 42;

  // Initialize HMatrix
  init_hmatrix();

  // Initialize debug RS232 with PBA clock
  init_dbg_rs232(pm_freq_param.pba_f);

  // Start test
  print_dbg("\r\nInitialize SD/MMC driver");

  // Initialize SD/MMC driver resources: GPIO, SDIO and SD/MMC.
  sd_mmc_mci_resources_init();

  // Wait for a card to be inserted
  #if (BOARD == EVK1104)
    #if EXAMPLE_SD_SLOT == SD_SLOT_8BITS
    print_dbg("\r\nInsert a MMC/SD card into the SD/MMC 8bits slot...");
    #elif EXAMPLE_SD_SLOT == SD_SLOT_4BITS
    print_dbg("\r\nInsert a MMC/SD card into the SD/MMC 4bits slot...");
    #else
    # error SD_SLOT not supported
    #endif
    while (!sd_mmc_mci_mem_check(EXAMPLE_SD_SLOT));
  #else
  # error Board not supported
  #endif

  print_dbg("Card detected!\r\n");

  // Read Card capacity
  sd_mmc_mci_read_capacity(EXAMPLE_SD_SLOT, &card_size);
  print_dbg("\r\nCapacity = ");
  print_dbg_ulong(card_size*512);
  print_dbg(" Bytes\r\n");



  // Read the first sector number 0 of the card
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in);
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not read device.\r\n");
    return -1;
  }

  // Display the ram_buffer content
  print_dbg("\r\nFirst sector of the card:\r\n");
  for (i=0;i<(512);i++)
  {
    print_dbg_char_hex(buffer_in[i]);
    j++;
    if (j%32==0)
      print_dbg("\r\n"), j=0;
    else if (j%4==0)
      print_dbg(" ");
  }



  // Write some patterns in the first sector number 0 of the card
  print_dbg("Testing write.\r\n");
  if( !test_sd_mmc_write(0) ) return -1;
  if( !test_sd_mmc_write(1) ) return -1;
  if( !test_sd_mmc_write(2) ) return -1;
  if( !test_sd_mmc_write(3) ) return -1;


  // Bench single-block read operations without DMA
  //
  print_dbg("Benching single-block read (without DMA). Please wait...");
  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;

  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, n_sector, buffer_in);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not read device.\r\n");
      return -1;
    }
    n_sector+=1;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench single-block read operations with DMA
  //
  print_dbg("Benching single-block read (with DMA).    Please wait...");
  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;

  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_dma_mem_2_ram(EXAMPLE_SD_SLOT, n_sector, buffer_in);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not read device.\r\n");
      return -1;
    }
    n_sector+=1;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench multi-block read operations without DMA
  //
  print_dbg("Benching multi-block read  (without DMA). Please wait...");
  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;

  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_multiple_mem_2_ram(EXAMPLE_SD_SLOT, n_sector, buffer_in, ALLOCATED_SECTORS);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not read device.\r\n");
      return -1;
    }
    n_sector+=ALLOCATED_SECTORS;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench multi-block read operations with DMA
  //
  print_dbg("Benching multi-block read  (with DMA).    Please wait...");
  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;

  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_dma_multiple_mem_2_ram(EXAMPLE_SD_SLOT, n_sector, buffer_in, ALLOCATED_SECTORS);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not read device.\r\n");
      return -1;
    }
    n_sector+=ALLOCATED_SECTORS;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench single-block write operations without DMA
  //
  print_dbg("Benching single-block write (without DMA). Please wait...");
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in); // Backup the first sector number 0 of the card
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not read device.\r\n");
    return -1;
  }

  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;
  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_ram_2_mem(EXAMPLE_SD_SLOT, n_sector, buffer_in);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not write device.\r\n");
      return -1;
    }
    n_sector+=1;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench single-block write operations with DMA
  //
  print_dbg("Benching single-block write (with DMA).    Please wait...");
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in); // Backup the first sector number 0 of the card
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not read device.\r\n");
    return -1;
  }

  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;
  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_dma_ram_2_mem(EXAMPLE_SD_SLOT, n_sector, buffer_in);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not write device.\r\n");
      return -1;
    }
    n_sector+=1;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench multi-block write operations without DMA
  //
  print_dbg("Benching multi-block write  (without DMA). Please wait...");
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in); // Backup the first sector number 0 of the card
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not read device.\r\n");
    return -1;
  }

  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;
  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_multiple_ram_2_mem(EXAMPLE_SD_SLOT, n_sector, buffer_in, ALLOCATED_SECTORS);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not write device.\r\n");
      return -1;
    }
    n_sector+=ALLOCATED_SECTORS;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));



  // Bench multi-block write operations with DMA
  //
  print_dbg("Benching multi-block write  (with DMA).    Please wait...");
  status = sd_mmc_mci_mem_2_ram(EXAMPLE_SD_SLOT, 0, buffer_in); // Backup the first sector number 0 of the card
  if( status!=CTRL_GOOD )
  {
    print_dbg("\r\nERROR: can not read device.\r\n");
    return -1;
  }

  n_sector =
  bench_start_sector = (card_size<BENCH_START_SECTOR) ? 0 : BENCH_START_SECTOR;
  cpu_set_timeout( cpu_ms_2_cy(BENCH_TIME_MS, pm_freq_param.cpu_f), &timer);
  while( !cpu_is_timeout(&timer) )
  {
    status = sd_mmc_mci_dma_multiple_ram_2_mem(EXAMPLE_SD_SLOT, n_sector, buffer_in, ALLOCATED_SECTORS);
    if( status!=CTRL_GOOD )
    {
      print_dbg("\r\nERROR: can not write device.\r\n");
      return -1;
    }
    n_sector+=ALLOCATED_SECTORS;
  }
  display_perf_bps((((U64)n_sector-bench_start_sector)*512)/(BENCH_TIME_MS/1000));

  return 0;
}
