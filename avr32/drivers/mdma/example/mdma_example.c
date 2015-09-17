/*****************************************************************************
 *
 * \file
 *
 * \brief MDMA Example for AVR32 MDMA driver module.
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
 ******************************************************************************/

 /*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the MDMA driver.
 *
 * This example demonstrates how to use the MDMA driver.
 * - Step 1: Initialization of CPU clock, Usart, SDRAM and MDMA transfers.
 * - Step 2: Start the transfer
 *  a) It copies a buffer located in flash to external SDRAM (AVR32_EBI_CS1_0_ADDRESS).
 *  b) Then, it copies the content of the external SDRAM (AVR32_EBI_CS1_0_ADDRESS) to HSB Ram (AVR32_HRAMC0_ADDRESS).
 *  c) Then, it copies the content of the HSB Ram (AVR32_HRAMC0_ADDRESS) to CPU ram.
 *  d) Then, it copies the content of CPU ram to the HSB Ram (AVR32_HRAMC0_ADDRESS).
 *  e) Then, it copies the content of the HSB Ram (AVR32_HRAMC0_ADDRESS) to the external SDRAM (AVR32_EBI_CS1_0_ADDRESS).
 * - Step 3: Check the content of the external SDRAM
 *  a) If the transfer is successful, the content of the flash buffer has been correctly copied to external SDRAM, leds are blinking slowly
 *  b) If not, the leds are blinking rapidly
 *
 * \section files Main Files
 * - mdma.c: MDMA driver;
 * - mdma.h: MDMA driver header file;
 * - sdramc_example.c: MDMA example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an MDMA, SDRAMC and a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock:
 *      -- 16 MHz : UC3C_EK evaluation kit;
 * - USART2 (on UC3C_EK) connected to a PC serial port
 *   abstracted with a USB CDC connection to a PC;
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

#include "intc.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "mdma.h"
#include "board.h"
#include "sdramc.h"
#include "flashc_buffer.h"
#include "cycle_counter.h"
#include "print_funcs.h"

#define FCPU_HZ		48000000 //!< CPU Clock at 48MHz
#define FPBA_HZ		24000000 //!< PBA Clock at 24MHz
#define FPBB_HZ		48000000 //!< PBB Clock at 48MHz

#define CHANNEL_NBR    0        //!< Number of MDMA channel used
#define BUFFER_SIZE    1024     //!< Buffer Size
#define TRANSFERT_SIZE MDMA_TRANSFERT_SIZE_WORD       //!< Transfer Size: Word
#define BURST_MODE     MDMA_SINGLE_TRANSFERT_MODE     //!< Burst Mode : Single

/*! \name Configuration to use for the example
 */
//! @{
#if BOARD == UC3C_EK
#  define EXAMPLE_MDMA_USART               (&AVR32_USART2)
#  define EXAMPLE_MDMA_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define EXAMPLE_MDMA_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define EXAMPLE_MDMA_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define EXAMPLE_MDMA_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#  define EXAMPLE_MDMA_LED_READ            LED1
#  define EXAMPLE_MDMA_LED_ERRORS          (LED0 | LED1 | LED2 | LED3)
#  define EXAMPLE_MDMA_LED_OK              (LED0 | LED1 | LED2 | LED3)
#else
  #error The board configuration for this example must be defined here.
#endif
//! @}

// Instance declaration of MDMA
volatile avr32_mdma_t* mdma = &AVR32_MDMA;

/*! \brief Interrupt for End of MDMA Transfer
*/
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void mdma_int_handler(void)
{
  Disable_global_interrupt();
  U32 status = mdma->isr;
  mdma->icr = status;
  Enable_global_interrupt();
}

void init_usart(void)
{
  static const gpio_map_t USART_GPIO_MAP =
  {
    {EXAMPLE_MDMA_USART_RX_PIN, EXAMPLE_MDMA_USART_RX_FUNCTION},
    {EXAMPLE_MDMA_USART_TX_PIN, EXAMPLE_MDMA_USART_TX_FUNCTION}
  };

  // USART options.
  static const usart_options_t USART_OPTIONS =
  {
    .baudrate     = 57600,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to USART.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode.
  usart_init_rs232(EXAMPLE_MDMA_USART, &USART_OPTIONS, FPBA_HZ);

  print_dbg("MDMA Example : This example demonstrates how to use the MDMA driver. \r\n");
}

/*! \brief Initializes the MCU system clocks.
*/
static void init_sys_clocks(void)
{
  /*! \name System Clock Frequencies
   */
  //! @{
  static pcl_freq_param_t pcl_freq_param =
  {
    .cpu_f        = FCPU_HZ,
    .pba_f        = FPBA_HZ,
    .pbb_f        = FPBB_HZ,
    .osc0_f       = FOSC0,
    .osc0_startup = OSC0_STARTUP
  };
  //! @}

  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS) {
    while(1);
  }
}

// Pointer on SRAM Buffer
U32  sram_buffer[BUFFER_SIZE];

// Declaration of MDMA Buffer
mdma_descriptor_t mdma_buf_desc[5];

/*! \brief Initialize the MDMA buffer descriptor.
*/
void mdma_init_buffer_descriptor(void)
{
  // First MDMA Descriptor
  // Source : Flash
  // Destination : SDRAM
  // Size : BUFFER_SIZE
  // Transfer Size: TRANSFERT_SIZE
  // Mode : BURST_MODE
  // Interrupt : 1 (Enable)
  // Valid : 1 (true)
  // Last : 0 (false)
  mdma_buf_desc[0].src            = (U32)flashc_data;             // First MDMA Descriptor
  mdma_buf_desc[0].src            = (U32)flashc_data;             // Source : Flash
  mdma_buf_desc[0].dest           = (U32)AVR32_EBI_CS1_0_ADDRESS; // Destination : SDRAM
  mdma_buf_desc[0].ccr.count      = BUFFER_SIZE;                  // Size : BUFFER_SIZE
  mdma_buf_desc[0].ccr.size       = TRANSFERT_SIZE;               // Transfer Size: TRANSFERT_SIZE
  mdma_buf_desc[0].ccr.burst_size = BURST_MODE;                   // Mode : BURST_MODE
  mdma_buf_desc[0].ccr.tc_ienable = 1;                            // Interrupt : 1 (Enable)
  mdma_buf_desc[0].ccr.V          = 1;                            // Valid : 1 (true)
  mdma_buf_desc[0].ccr.L          = 0;                            // Last : 0 (false)

  mdma_buf_desc[0].src             = (U32)((unsigned short *)flashc_data);
  mdma_buf_desc[0].src             = (U32)((unsigned short *)flashc_data);
  mdma_buf_desc[0].dest            = (U32)((unsigned short *)AVR32_EBI_CS1_0_ADDRESS);
  mdma_buf_desc[0].ccr.count       = BUFFER_SIZE;
  mdma_buf_desc[0].ccr.size        = TRANSFERT_SIZE;
  mdma_buf_desc[0].ccr.burst_size  = BURST_MODE;
  mdma_buf_desc[0].ccr.tc_ienable  = 1;
  mdma_buf_desc[0].ccr.V           = 1;
  mdma_buf_desc[0].ccr.L           = 0;

  // Second MDMA Descriptor
  // Source : SDRAM
  // Destination : HSB Ram
  // Size : BUFFER_SIZE
  // Transfer Size: TRANSFERT_SIZE
  // Mode : BURST_MODE
  // Interrupt : 1 (Enable)
  // Valid : 1 (true)
  // Last : 0 (false)
  mdma_buf_desc[1].src             = (U32)((unsigned short *)AVR32_EBI_CS1_0_ADDRESS);
  mdma_buf_desc[1].dest            = (U32)((unsigned short *)AVR32_HRAMC0_ADDRESS);
  mdma_buf_desc[1].ccr.count       = BUFFER_SIZE;
  mdma_buf_desc[1].ccr.size        = TRANSFERT_SIZE;
  mdma_buf_desc[1].ccr.burst_size  = BURST_MODE;
  mdma_buf_desc[1].ccr.tc_ienable  = 1;
  mdma_buf_desc[1].ccr.V           = 1;
  mdma_buf_desc[1].ccr.L           = 0;

  // Third MDMA Descriptor
  // Source : HSB Ram
  // Destination : CPU Ram
  // Size : BUFFER_SIZE
  // Transfer Size: TRANSFERT_SIZE
  // Mode : BURST_MODE
  // Interrupt : 1 (Enable)
  // Valid : 1 (true)
  // Last : 0 (false)
  mdma_buf_desc[2].src             = (U32)((unsigned short *)AVR32_HRAMC0_ADDRESS);
  mdma_buf_desc[2].dest            = (U32)sram_buffer;
  mdma_buf_desc[2].ccr.count       = BUFFER_SIZE;
  mdma_buf_desc[2].ccr.size        = TRANSFERT_SIZE;
  mdma_buf_desc[2].ccr.burst_size  = BURST_MODE;
  mdma_buf_desc[2].ccr.tc_ienable  = 1;
  mdma_buf_desc[2].ccr.V           = 1;
  mdma_buf_desc[2].ccr.L           = 0;

  // Fourth MDMA Descriptor
  // Source : CPU Ram
  // Destination : HSB Ram
  // Size : BUFFER_SIZE
  // Transfer Size: TRANSFERT_SIZE
  // Mode : BURST_MODE
  // Interrupt : 1 (Enable)
  // Valid : 1 (true)
  // Last : 0 (false)
  mdma_buf_desc[3].src             = (U32)((unsigned short *)sram_buffer);
  mdma_buf_desc[3].dest            = (U32)AVR32_HRAMC0_ADDRESS;
  mdma_buf_desc[3].ccr.count       = BUFFER_SIZE;
  mdma_buf_desc[3].ccr.size        = TRANSFERT_SIZE;
  mdma_buf_desc[3].ccr.burst_size  = BURST_MODE;
  mdma_buf_desc[3].ccr.tc_ienable  = 1;
  mdma_buf_desc[3].ccr.V           = 1;
  mdma_buf_desc[3].ccr.L           = 0;

  // Fifth MDMA Descriptor
  // Source : HSB Ram
  // Destination : SDRAM
  // Size : BUFFER_SIZE
  // Transfer Size: TRANSFERT_SIZE
  // Mode : BURST_MODE
  // Interrupt : 1 (Enable)
  // Valid : 1 (true)
  // Last : 1 (true)
  mdma_buf_desc[4].src             = (U32)((unsigned short *)AVR32_HRAMC0_ADDRESS);
  mdma_buf_desc[4].dest            = (U32)AVR32_EBI_CS1_0_ADDRESS;
  mdma_buf_desc[4].ccr.count       = BUFFER_SIZE;
  mdma_buf_desc[4].ccr.size        = TRANSFERT_SIZE;
  mdma_buf_desc[4].ccr.burst_size  = BURST_MODE;
  mdma_buf_desc[4].ccr.tc_ienable  = 1;
  mdma_buf_desc[4].ccr.V           = 1;
  mdma_buf_desc[4].ccr.L           = 1;
}

//! Main function
int main(void)
{
  int i;
  unsigned long noErrors = 0;
  volatile unsigned char *sdram = SDRAM;

  // Enable Channel 0 complete Interrupt
  static const mdma_interrupt_t MDMA_INTERRUPT =
  {
    .ch0c = 1,
  };

  // Initialize System Clock
  init_sys_clocks();

  // Initialize USART for Debug
  init_usart();

  // Initialize the external SDRAM chip.
  sdramc_init(FPBB_HZ);
  print_dbg("SDRAM initialized...\r\n");

  gpio_set_gpio_pin(LED0_GPIO);
  gpio_set_gpio_pin(LED1_GPIO);
  gpio_set_gpio_pin(LED2_GPIO);
  gpio_set_gpio_pin(LED3_GPIO);

  // Initialize MDMA Buffer Descriptor.
  mdma_init_buffer_descriptor();

  Disable_global_interrupt();

  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Register the MDMA interrupt handler to the interrupt controller.
  INTC_register_interrupt(&mdma_int_handler, AVR32_MDMA_IRQ, AVR32_INTC_INT0);

  // Enable global interrupt
  Enable_global_interrupt();

  // Initialize MDMA Transfer.
  mdma_descriptor_mode_xfert_init(mdma,CHANNEL_NBR,(U32*)&mdma_buf_desc);

  // Configure MDMA Interrupts.
  mdma_configure_interrupts(mdma, &MDMA_INTERRUPT);

  print_dbg("MDMA initialized...\r\n");

  // Clear SDRAM area for the flash buffer size
  for (i=0;i<FLASHC_DATA_BUFFER_SIZE;i++)
  {
    sdram[i] = 0;
  }

  // Start MDMA transfer
  mdma_start_descriptor_xfert(mdma,CHANNEL_NBR,MDMA_FIXED_PRIORITY_MODE);

  print_dbg("Start MDMA Transfer...\r\n");

  // Wait end of transfer for Descriptor 4
  while (mdma_buf_desc[4].ccr.V==1);

  print_dbg("End of MDMA Transfer, check transfer on last transfer ...\r\n");

  // Check content inside the buffer of descriptor 4
  for (i=0;i<FLASHC_DATA_BUFFER_SIZE;i++)
  {
    if (sdram[i] != flashc_data[i])
    {
      noErrors++;
    }
  }

  LED_Off(EXAMPLE_MDMA_LED_READ);
  print_dbg_ulong(noErrors);
  print_dbg(" corrupted word(s)       \r\n");
  if (noErrors)
  {
    LED_Off(EXAMPLE_MDMA_LED_ERRORS);
    while (1)
    {
      LED_Toggle(EXAMPLE_MDMA_LED_ERRORS);
      cpu_delay_ms(200, FOSC0);   // Fast blink means errors.
    }
  }
  else
  {
    LED_Off(EXAMPLE_MDMA_LED_OK);
    while (1)
    {
      LED_Toggle(EXAMPLE_MDMA_LED_OK);
      cpu_delay_ms(1000, FOSC0);  // Slow blink means OK.
    }
  }
}
