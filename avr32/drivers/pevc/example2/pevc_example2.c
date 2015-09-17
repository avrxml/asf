/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC example 2 application for AVR32 using Generic Clock as generator
 *      and PDCA as event user.
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
 *
 * The given example covers a use of PEVC controller and demonstrates how to
 *  use it.
 * In the present example the Generic clock generate event. This one is transmitted
 * to the PDCA. Each time a new event is coming, a character is sent to the
 * USART without use of the CPU. The main loop of the function is a 500ms delay and
 * toggle LED 1 continuously to show CPU activity.
 *
 * \section files Main Files
 * - pevc.c: PEVC driver;
 * - pevc.h: PEVC driver header file;
 * - pevc_example2.c: PEVC example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a PEVC module can be used. This example has been tested
 * with the following setup:
 *   - UC3C_EK evaluation kit;
 *
 * \section setupinfo Setup Information
 * - CPU speed for UC3C : <i> 60 MHz from 12-MHz Osc0 crystal </i>
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
#include "intc.h"
#include "power_clocks_lib.h"
#include "pevc.h"
#include "gpio.h"
#include "usart.h"
#include "print_funcs.h"
#include "pdca.h"
#include "delay.h"

//! Size of the string to transfer to USART0 through the PDCA
#define STRING_TRANSFER_SIZE  36

//! The PDCA channel instance for the USART0 Tx
// NOTE: the PEVC-PDCA links are only available for PDCA channel 0 & 1.
#define PDCA_CHANNEL_USART      0
#define PDCA_CHANNEL_IRQ        AVR32_PDCA_IRQ_0
#define PEVC_PDCA_SOT_USER      AVR32_PEVC_ID_USER_PDCA_0

volatile avr32_pdca_channel_t *pdca_channel;
volatile avr32_pevc_t         *ppevc  = &AVR32_PEVC;
volatile avr32_pm_t           *pm     = &AVR32_PM;

static volatile U32 u32PdcaIsr;

// String to transfer to USART0 through the PDCA.
unsigned char aDataTransfered[STRING_TRANSFER_SIZE];

/**
 ** PDCA Interrupts handler.
 **/
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void pdca_int_handler(void)
{
  u32PdcaIsr = pdca_channel->isr;
  if( u32PdcaIsr & (1<<AVR32_PDCA_ISR0_TRC_OFFSET) )
  {
    // Count the number of Transfer Complete interrupts.
    pdca_reload_channel(PDCA_CHANNEL_USART, (void *)aDataTransfered, sizeof( aDataTransfered ));
    print_dbg("\r\n");
  }
}

/**
 ** USART init.
 **/
void init_usart(void)
{

  init_dbg_rs232(FCPU_HZ);
  print_dbg("\x0CPEVC Driver - EXAMPLE 2\r\n");
  print_dbg("USART transfer using PEVC, Generic Clock and PDCA\r\n");
}

/**
 ** PEVC init.
 **/
void init_pevc(void)
{

  // PEVC Event Shaper options.
  static const pevc_evs_opt_t PEVC_EVS_OPTIONS =
  {
    .igfdr = 0x0A,            // Set the IGF clock (don't care here).
    .igf = PEVC_EVS_IGF_OFF,  // Input Glitch Filter off
    .evf = PEVC_EVS_EVF_OFF,   // Enable Event on falling edge
    .evr = PEVC_EVS_EVR_ON    // Enable Event on rising edge
  };

  // Configuring the PEVC path:
  //  Change on pevc input pin0 event -> PDCA channel 0/1 trigger one transfer
  if(FAIL == pevc_channel_configure(ppevc,
                                    PEVC_PDCA_SOT_USER,
                                    AVR32_PEVC_ID_GEN_GCLK_0,
                                    &PEVC_EVS_OPTIONS))
  {
    print_dbg("PEVC channel config failed!!!\r\n");
    gpio_clr_gpio_pin(LED2_GPIO);
    while(1);
  }

  // Enable the PEVC channel 0.
  pevc_channels_enable(ppevc, 1<<PEVC_PDCA_SOT_USER);
}

/**
 ** PDCA init.
 **/
void init_pdca(void)
{
  // PDCA channel 0/1 options
  static const pdca_channel_options_t PDCA_CH_OPTIONS =
  {
    .addr = (void *)aDataTransfered,          // memory address
    .pid = AVR32_PDCA_PID_USART2_TX,          // select peripheral - data are transmit on USART TX line.
    .size = 0,                                // transfer counter
    .r_addr = (void *)aDataTransfered,        // next memory address
    .r_size = sizeof(aDataTransfered),        // next transfer counter
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE, // select size of one data packet
    .etrig = true                          // Trigger transfer on event.
  };

  Disable_global_interrupt();

  // Register the PDCA interrupt handler to the interrupt controller.
  INTC_register_interrupt(&pdca_int_handler, PDCA_CHANNEL_IRQ, AVR32_INTC_INT0);

  Enable_global_interrupt();

  // Init PDCA channel with the pdca_options.
  pdca_init_channel(PDCA_CHANNEL_USART, &PDCA_CH_OPTIONS);
  pdca_channel = pdca_get_handler(PDCA_CHANNEL_USART); // For use in the pdca interrupt handler.

  // Enable pdca transfer error interrupt & transfer complete interrupt.
  pdca_enable_interrupt_transfer_error(PDCA_CHANNEL_USART);
  pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_USART);

  // Enable the PDCA.
  pdca_enable(PDCA_CHANNEL_USART);
}

/**
 ** GCLK init.
 **/
void init_gclk(void)
{
    scif_osc32_opt_t opt =
  {
    .mode = SCIF_OSC_MODE_2PIN_CRYSTAL,
    .startup = AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC
  };

  scif_start_osc32(&opt,true);

  // Init GCLK clock
  scif_gclk_opt_t gclkOpt =
  {
    .clock_source = SCIF_GCCTRL_OSC32K,
    .divider      = 255,
    .diven        = 1
  }; // 32 678 / 512 = 64hz

  if(scif_start_gclk(AVR32_SCIF_GCLK_GCLK2_EVENT, &gclkOpt))
  {
    // Error
    while(1);
  }
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
    .osc0_f       = FOSC0,
    .osc0_startup = OSC0_STARTUP
  };
  //! @}

  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS) {
    while(1);
  }
}

int main(void)
{
  int i;

  // Init the string with a simple recognizable pattern.
  for(i=0;i<sizeof(aDataTransfered);i++) {
    aDataTransfered[i] = '0' + (i%36);
  }

  init_sys_clocks();

  delay_init(FCPU_HZ);

  init_usart();

  gpio_clr_gpio_pin(LED0_GPIO);

  init_pdca();

  init_pevc();

  init_gclk();

  while(1)
  {
    gpio_tgl_gpio_pin(LED1_GPIO);
    delay_ms(500); //Wait 500ms
  }
}
