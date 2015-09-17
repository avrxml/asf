/*****************************************************************************
 *
 * \file
 *
 * \brief SD/MMC card driver example application.
 *
 * This file gives an example of using the SD/MMC card driver.
 * If a SD or a MMC card is detected, a message is sent to user
 * after that it copies data from SD/MMC to RAM using the PDCA.
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
 * typedefs for the SD/MMC driver in SPI mode. <BR>It also comes with an example of the usage of this driver.<BR>
 *
 * This file gives an example of using the SD/MMC card driver.
 * If a SD/MMC card is detected, the main() function reads data from SD/MMC
 * to internal RAM using the PDCA (Peripheral DMA Controller). It displays on RS232 the first
 * 20 bytes from SD/MMC physical sector number 1, 2 and 3.
 *
 * \section files Main Files
 * - sd_mmc_spi.c: SD/MMC driver;
 * - sd_mmc_spi.h: SD/MMC driver header file;
 * - sd_mmc_spi_mem.c: SD/MMC memory interface;
 * - sd_mmc_spi_mem.h: SD/MMC memory interface header file;
 * - sd_mmc_spi_example.c: SD/MMC application example ;
 * - conf_access.h: Configuration for CTRL_ACCESS module;
 * - conf_sd_mmc_spi.h: Configuration for SD/MMC module;
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an USART and a SPI module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, UC3C_EK or EVK1105 evaluation kits;
  * - CPU clock:
 *        -- 12 MHz : EVK1100, EVK1101, EVK1105 evaluation kits
 *        -- 16 Mhz : UC3C_EK
 * - USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a standard RS232 DB9 cable; USART2 (on UC3C_EK) or USART0 on EVK1105 connected to a PC via USB virtual com port
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
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "spi.h"
#include "conf_sd_mmc_spi.h"
#include "sd_mmc_spi.h"
#include "usart.h"
#include "print_funcs.h"
#include "pdca.h"
#include "intc.h"


// Dummy char table
const char dummy_data[] =
#include "dummy.h"
;


//! \brief PBA clock frequency (Hz)
#define PBA_HZ                FOSC0

//! \brief Number of bytes in the receive buffer when operating in slave mode
#define BUFFERSIZE            64

#if BOARD==EVK1100
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX
#elif BOARD==EVK1101
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI_TX
#elif BOARD==UC3C_EK
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX
#elif BOARD==EVK1105
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI0_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI0_TX
#else
#  error No known AVR32 board defined
#endif
#define AVR32_PDCA_CHANNEL_SPI_RX 0 // In the example we will use the pdca channel 0.
#define AVR32_PDCA_CHANNEL_SPI_TX 1 // In the example we will use the pdca channel 1.

// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

// Used to indicate the end of PDCA transfer
volatile bool end_of_transfer;

// Local RAM buffer for the example to store data received from the SD/MMC card
volatile char ram_buffer[1000];


// Software wait
void wait()
{
  volatile int i;
  for(i = 0 ; i < 5000; i++);
}


/* interrupt handler to notify if the Data reception from flash is
 * over, in this case lunch the Memory(ram_buffer) to USART transfer and
 * disable interrupt*/
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void pdca_int_handler(void)
{
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_spi_read_close_PDCA();//unselects the SD/MMC memory.
  wait();
  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = true;
}


/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
 */
static void sd_mmc_resources_init(void)
{
  // GPIO pins used for SD/MMC interface
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
  {
    {SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
    {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
    {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
    {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = SD_MMC_SPI_NPCS,
    .baudrate     = SD_MMC_SPI_MASTER_SPEED,  // Defined in conf_sd_mmc_spi.h.
    .bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc_spi.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
                     sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(SD_MMC_SPI, &spiOptions);

  // Set SPI selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

  // Enable SPI module.
  spi_enable(SD_MMC_SPI);

  // Initialize SD/MMC driver with SPI clock (PBA).
  sd_mmc_spi_init(spiOptions, PBA_HZ);
}


/*! \brief Initialize PDCA (Peripheral DMA Controller A) resources for the SPI transfer and start a dummy transfer
 */
void local_pdca_init(void)
{
  // this PDCA channel is used for data reception from the SPI
  pdca_channel_options_t pdca_options_SPI_RX ={ // pdca channel options

    .addr = ram_buffer,
    // memory address. We take here the address of the string dummy_data. This string is located in the file dummy.h

    .size = 512,                              // transfer counter: here the size of the string
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_RX,        // select peripheral ID - data are on reception from SPI1 RX line
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // this channel is used to activate the clock of the SPI by sending a dummy variables
  pdca_channel_options_t pdca_options_SPI_TX ={ // pdca channel options

    .addr = (void *)&dummy_data,              // memory address.
                                              // We take here the address of the string dummy_data.
                                              // This string is located in the file dummy.h
    .size = 512,                              // transfer counter: here the size of the string
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_TX,        // select peripheral ID - data are on reception from SPI1 RX line
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // Init PDCA transmission channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);

  // Init PDCA Reception channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);

  //! \brief Enable pdca transfer interrupt when completed
  INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);  // pdca_channel_spi1_RX = 0

}


/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  int i, j;


  // Switch the main clock to the external oscillator 0
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

  // Initialize debug RS232 with PBA clock
  init_dbg_rs232(PBA_HZ);

  //start test
  print_dbg("\r\nInit SD/MMC Driver");
  print_dbg("\r\nInsert SD/MMC...");

  // Initialize Interrupt Controller
  INTC_init_interrupts();

  // Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  sd_mmc_resources_init();

  // Wait for a card to be inserted
  while (!sd_mmc_spi_mem_check());
  print_dbg("\r\nCard detected!");

  // Read Card capacity
  sd_mmc_spi_get_capacity();
  print_dbg("Capacity = ");
  print_dbg_ulong(capacity >> 20);
  print_dbg(" MBytes");

  // Enable all interrupts.
  Enable_global_interrupt();

  // Initialize PDCA controller before starting a transfer
  local_pdca_init();

  // Read the first sectors number 1, 2, 3 of the card
  for(j = 1; j <= 3; j++)
  {
    // Configure the PDCA channel: the address of memory ram_buffer to receive the data at sector address j
    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX,
                     &ram_buffer,
                     512);

    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX,
                     (void *)&dummy_data,
                     512); //send dummy to activate the clock

    end_of_transfer = false;
    // open sector number j
    if(sd_mmc_spi_read_open_PDCA (j))
    {
      print_dbg("\r\nFirst 512 Bytes of Transfer number ");
      print_dbg_ulong(j);
      print_dbg(" :\r\n");

      spi_write(SD_MMC_SPI,0xFF); // Write a first dummy data to synchronize transfer
      pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
      pdca_channelrx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); // get the correct PDCA channel pointer
      pdca_channeltx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX); // get the correct PDCA channel pointer
      pdca_channelrx->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
      pdca_channeltx->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer

      while(!end_of_transfer);

      // Display the first 2O bytes of the ram_buffer content
      for( i = 0; i < 20; i++)
      {
        print_dbg_char_hex( (U8)(*(ram_buffer + i)));
      }
    }
    else
    {
      print_dbg("\r\n! Unable to open memory \r\n");
    }
  }
  print_dbg("\r\nEnd of the example.\r\n");

  while (1);
}
