/*****************************************************************************
 *
 * \file
 *
 * \brief DFLL and generic clock configuration and sleep mode example.
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
 *****************************************************************************/

/*! \mainpage
* \section intro Introduction
* This is the documentation for the data structures, functions, variables, defines,
* enums, and typedefs for the SCIF driver.\n It also gives an example of usage of
* the SCIF & PM module on UC3 products.
* This example shows how to configure the System Control Interface(SCIF) to:
* - generate a high frequency clock (~22MHz) with a DFLL in closed-loop mode,
* - set-up a generic clock at 44kz with the DFLL as a source,
* - output the generic clock to a pin.
* At the end of the example, using the Power manager(PM), the program goes into
* the frozen sleep mode (while still maintaining GCLK output to the pin).
*
* \section compinfo Compilation Info
* This software was written for the GNU GCC for AVR32 and IAR Systems compiler
* for AVR32. Other compilers may or may not work.
*
* \section deviceinfo Device Info
* All AVR32 devices with a PM module and a SCIF module can be used. This example
* has been tested with the following setup:\n
* - STK600 starter kit with the RCUC3L0 routing card
* - AT32UC3L-EK board
*
* \section setupinfo Setup Information
* \n CPU speed: <i>115 kHz</i>\n\n
* On the STK600 + RCUC3L0 setup and also on the AT32UC3L-EK setup, the generic
* clock is output to the GCLK_1_0 pin. This is the GPIO pin 6/pin pa06/pin#10 on
* a UC3L QFP48 package. For the RCUC3L0 routing card, this pin is mapped on STK600.PORTA.PA6.
* - Check the GCLK1_0 pin with an oscilloscope, it oscillates at the frequency 44kHz.
*
* \section contactinfo Contact Info
* For more info about Atmel AVR32 visit
* <A href="http://www.atmel.com/products/AVR32/" >Atmel AVR32</A>\n
* Support and FAQ: http://www.atmel.com/design-support/
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "board.h"
#include "gpio.h"
#include "scif_uc3l.h"
#include "power_clocks_lib.h"

/*! \name Generic Clock Configuration
 */
//! @{
#if BOARD == STK600_RCUC3L0 || BOARD == UC3L_EK
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG // Do not use the AVR32_SCIF_GCLK_DFLL0_REF gc.
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN   // Mapped on STK600.PORTA.PA6; connector PA6 on AT32UC3L-EK
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#endif

#if !defined(EXAMPLE_GCLK_ID)  || \
    !defined(EXAMPLE_GCLK_PIN) || \
    !defined(EXAMPLE_GCLK_FUNCTION)
#  error The generic clock configuration to use in this example is missing.
#endif

//! The target output frequency (22MHz) of the DFLL
#define   EXAMPLE_FDFLL_KHZ     22579
#define   EXAMPLE_FDFLL_HZ      22579200UL

//! The target frequency (44kHz) of the generic clock.
#define   EXAMPLE_GCLK_FREQ_HZ  44100

//! @}

/*! \brief Generate a high frequency clock with a DFLL running in closed-loop mode
 *
 */
static void local_start_dfll_clock(void)
{
  scif_dfll_closedloop_conf_t DfllConfig;
  scif_gclk_opt_t             GcConf;


  // 1) Configure and start the DFLL main reference generic clock:
  // use the undivided RCOSC slow clock as source for the generic clock. The
  // generic clock frequency will thus be ~115kHz.
  GcConf.clock_source = SCIF_GCCTRL_SLOWCLOCK;
  GcConf.diven = false;
  // Note: this function will start the AVR32_SCIF_GCLK_DFLL0_REF generic clock
  // (i.e. the generic clock dedicated to be the DFLL main reference clock).
  scif_dfll0_closedloop_mainref_gc_enable(&GcConf);

  // 2) Configure and start the DFLL.
  // The coarse value (= (fDFLL - SCIF_DFLL_MINFREQ_KHZ)*255/(SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ))
  DfllConfig.coarse = ((unsigned long long)(EXAMPLE_FDFLL_HZ - SCIF_DFLL_MINFREQ_HZ)*255)/(SCIF_DFLL_MAXFREQ_HZ - SCIF_DFLL_MINFREQ_HZ);
  // The fmul value (= (fDFLL*2^16)/fref, with fref being the frequency of the
  // DFLL main reference generic clock)
  DfllConfig.fmul = ((unsigned long long)EXAMPLE_FDFLL_HZ<<16)/SCIF_SLOWCLOCK_FREQ_HZ;
  // The fine and coarse maxstep values
#if (defined(__GNUC__) \
      && (defined(__AVR32_UC3L064__) || defined(__AVR32_UC3L032__) || defined(__AVR32_UC3L016__))) \
  ||((defined(__ICCAVR32__) || defined(__AAVR32__)) \
      && (defined(__AT32UC3L064__) || defined(__AT32UC3L032__) || defined(__AT32UC3L016__)))
      // UC3L revC and higher
  DfllConfig.finemaxstep = 0x0000004;
  DfllConfig.coarsemaxstep = 0x0000004;
#else
      // UC3L revB
      //+ Errata UC3L revB: 35.2.7 SCIF.12 DFLLIF indicates coarse lock too early
      //+ The DFLLIF might indicate coarse lock too early, the DFLL will lose
      //+ coarse lock and regain it later.
      //+ Fix/Workaround
      //+ Use max step size (DFLL0MAXSTEP.MAXSTEP) of 4 or higher.
  DfllConfig.maxstep = 0x0040004;
#endif
  scif_dfll0_closedloop_start(&DfllConfig);
}


/*! \brief Set-up a generic clock at 44kz with the DFLL as a source, output the
 * generic clock to a pin.
 *
 */
static void local_start_gc(void)
{
  // Setup gc on DFLL; the target frequency is 44kHz => divide the DFLL frequency
  // by 512 (== EXAMPLE_FDFLL_HZ / EXAMPLE_GCLK_FREQ_HZ).
  scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_DFLL0, AVR32_GC_DIV_CLOCK,
                EXAMPLE_FDFLL_HZ/EXAMPLE_GCLK_FREQ_HZ);

  // Now enable the generic clock
  scif_gc_enable(EXAMPLE_GCLK_ID);

  /* Assign a GPIO to generic clock output */
  gpio_enable_module_pin(EXAMPLE_GCLK_PIN, EXAMPLE_GCLK_FUNCTION);
  // Note that gclk1 is GPIO pin 6 pa06 on AT32UC3L064 pin 10 on QFP48.
}



/* \brief This is an example that shows how to do the following:
 * - generate a high frequency clock (~22MHz) with a DFLL in closed-loop mode
 * - set-up a generic clock with a DFLL as a source
 * - output the generic clock to GCLK_1_0
 * - go into the frozen sleep mode (while still maintaining GCLK output)
 *
 */
int main(void)
{
  // Generate a high frequency clock (~22MHz) with a DFLL in closed-loop mode
  local_start_dfll_clock();

  // Set-up a generic clock from a high frequency clock and output it to a gpio pin.
  local_start_gc();

  //*** Sleep mode
  // If there is a chance that any PB write operations are incomplete, the CPU
  // should perform a read operation from any register on the PB bus before
  // executing the sleep instruction.
  AVR32_INTC.ipr[0];  // Dummy read

  // - Go into a sleep mode (while still maintaining GCLK output)
  SLEEP(AVR32_PM_SMODE_FROZEN);

  while(1);
}

