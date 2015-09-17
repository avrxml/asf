/*****************************************************************************
 *
 * \file
 *
 * \brief QDEC example driver for AVR32 UC3.
 *
 * This file provides an example for the QDEC on AVR32 UC3 devices.
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
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the quadrature decoder (QDEC) driver. It also comes
 * bundled with an application-example of usage.
 *
 * This example demonstrates how to use the QDEC driver in timer mode.
 *
 * \section files Main Files
 * - qdec.c: Quadrature Decoder driver;
 * - qdec.h: Quadrature Decoder driver header file;
 * - qdec_example.c: Quadrature Decoder example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an QDEC and an INTC can be used.
 *
 * CPU speed: <i> 60 MHz </i>
 * - On \b UC3C_EK, check LED0_GPIO pin with an oscilloscope. The QDEC is used in Timer Mode in 32-bit.
 *   It generates an interrupt on a compare Match. At each interrupt a new value is reloaded in compare
 *   register.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "intc.h"
#include "board.h"
#include "compiler.h"
#include "qdec.h"
#include "gpio.h"
#include "power_clocks_lib.h"

#define FCPU_HZ          60000000
#define FPBA_HZ          FCPU_HZ
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

// Instance of QDEC0
volatile avr32_qdec_t* qdec = &AVR32_QDEC0;

// Used for synchronization with the main loop.
volatile unsigned char flag_qdec = 0;

/**
 ** QDEC Interrupts handler.
 **/
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void qdec_int_handler(void)
{
  // Reset CNT to CNT = 0x0000FFFF
  qdec_write_rc_cnt(qdec,0x00);
  qdec_write_pc_cnt(qdec,0xffff);
  // Synchronize with the main loop.
  flag_qdec = 1;
  // Clear the comparator interrupt.
  qdec->scr = (1<<AVR32_QDEC_SCR_CMP_OFFSET);
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  // Options for QDEC timer Mode
  static const qdec_timer_opt_t QUADRATURE_TIMER_OPT =
  {
    .upd      = false,             // Up/Down Mode Timer Disabled.
    .tsdir    = QDEC_TSIR_DOWN,       // Count Down Timer.
    .filten   = false,                // Disable filtering.
    .evtrge   = false,                // Disable event triggering.
    .rcce     = true,                 // Enable Position Compare.
    .pcce     = true,                 // Enable Revolution Compare.
  };
  // Options for QDEC Interrupt Management
  static const qdec_interrupt_t QDEC_INTERRUPT =
  {
    .cmp    = 1,                      // Enable Compare Interrupt.
    .cap    = 0,                      // Disable Capture Interrupt.
    .pcro   = 0,                      // Disable Position Roll-over Interrupt.
    .rcro   = 0                       // Disable Counter Roll-over Interrupt.
  };

  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS) {
    while(1);
  }

  // Setup the generic clock for QDEC
  scif_gc_setup(AVR32_SCIF_GCLK_QDEC0,
                SCIF_GCCTRL_OSC0,
                AVR32_SCIF_GC_NO_DIV_CLOCK,
                0);
  // Now enable the generic clock
  scif_gc_enable(AVR32_SCIF_GCLK_QDEC0);

  Disable_global_interrupt();

  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Register the QDEC interrupt handler to the interrupt controller.
  INTC_register_interrupt(&qdec_int_handler, AVR32_QDEC0_IRQ, AVR32_INTC_INT0);

  Enable_global_interrupt();

  // Initialization of counter value as a 32-bit counter to 0x0000FFFF (Rc=0x0000/Pc=0xFFFF)
  qdec_write_rc_cnt(qdec,0x0000);
  qdec_write_pc_cnt(qdec,0xffff);
  // Initialization of compare value to 0x0 as the interrupt will be generated when the counter value will be equal to 0
  qdec_write_rc_cmp(qdec,0);
  qdec_write_pc_cmp(qdec,0);

  // Initialize the QDEC in quadrature decoder mode.
  qdec_init_timer_mode(qdec,&QUADRATURE_TIMER_OPT);

  // Configure the QDEC interrupts.
  qdec_configure_interrupts(qdec,&QDEC_INTERRUPT);

   // Start the QDEC.
  qdec_software_trigger(qdec);

  unsigned int pc_cmp = 0;
  while(1)
  {
    // Compare Interrupt Flag
    if(flag_qdec == 1)
    {
        gpio_tgl_gpio_pin(LED0_GPIO);           // Check signal on oscilloscope.
        if (pc_cmp < 0xffff ) pc_cmp +=  0x100; // Increase PC CMP
        else pc_cmp = 0;                        // Start the signal pattern over.
        qdec_write_pc_cmp(qdec,pc_cmp);         // Reload Compare Flag
        flag_qdec = 0;                          // Reset Interrupt Flag
    }
  }
}
