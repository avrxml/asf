/*****************************************************************************
 *
 * \file
 * \brief Timer/Counter example 1.
 *
 * This example will start a timer/counter and generate a PWM on the output.
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
 * defines, enums, and typedefs for the TC driver.\n It also gives an example of
 * the usage of the TC module.
 *
 * This example will start a timer/counter and generate a PWM on the output.
 *
 * Channel 0 of a timer/counter module is used with the output pin TIOA0.
 * You will find the port and pin number in the datasheet of your device.
 * The selected timer input clock is the internal clock labelled TC3
 * referred to as TIMER_CLOCK3 in the datasheet. Use an oscilloscope to probe the
 * TC channel's output pin.
 * \note
 * - On the AT32UC3A0512, the output pin TIOA0 is mapped on PB23. \n
 * - On the AT32UC3B0256, the output pin TIOA0 is mapped on PB00. \n
 * - On the AT32UC3A3256, the chosen output pin TIOA0 for the Timer/Counter 0(TC0)
 *   is mapped on PB05. \n
 * - On the AT32UC3L064, the chosen output pin TIOA0 for the Timer/Counter 1(TC1)
 *   is mapped on PB00 (with the STK600 + RCUC3L0 routing card, PB00 is routed
 *   to STK600.PORTD.PD0).
 * - On the ATUC128D3, the chosen output pin TIOA0 for the Timer/Counter 1(TC0)
 *   is mapped on PB00 (with the STK600 + RCUC3D routing card, PB00 is routed
 *   to STK600.PORTE.PE01).
 * - On the AT32UC3C0512, the chosen output pin TIOA0 for the Timer/Counter 0(TC0)
 *   is mapped on PB19. \n
 *
 * The 16-bit timer/counter value register (CV) will cycle from 0x0000 to 0xFFFF.
 * The chosen channel with a low output pin level upon starting point (i.e. when
 * CV == 0) will toggle the output pin level when it reaches the values 0x0300
 * and 0x1000. This will hence produce a PWM output signal with a duty cycle of
 * (0x2000 - 0x600) / 0x10000, i.e. 10.16%.
 * \note According to datasheet, TIMER_CLOCK3 is equal to (clk_pba/8). Since we
 * switch the main CPU clock to OSC0 (=12MHz), we have TIMER_CLOCK3 == 1.5 MHz.\n
 * According to the settings performed by the application, the output waveform
 * should thus have the following properties:
 * - The period of the waveform will thus be 1500000 / 65536 = 22.9 Hz.
 * - A full duty cycle (just toggling the channel's output pin when CV reaches
 * 0xFFFF and passes to 0x0000) thus lasts 43.7 ms (65536 / 1500000). A duty cycle
 * of 10.16% means that when probing the channel's output pin you should see a
 * duty cycle of ~4.4 ms (10.16% of 43.7 ms).
 *
 * \section files Main Files
 * - tc.c: TC driver;
 * - tc.h: TC driver header file;
 * - tc_example1.c: TC example 1.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a TC module can be used. This example has been tested
 * with the following boards:
 * - EVK1100 evaluation kit with an AT32UC3A0512 MCU.
 * - EVK1101 evaluation kit with an AT32UC3B0256 MCU.
 * - EVK1104 evaluation kit with an AT32UC3A3256 MCU.
 * - STK600 + RCUC3L0 routing card with an AT32UC3L064 MCU.
 * - STK600 + RCUC3D routing card with an ATUC128D3 MCU.
 * - AT32UC3L-EK evaluation kit with an AT32UC3L064 MCU.
 * - AT32UC3C-EK evaluation kit with an AT32UC3C0512C MCU.
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> 12 MHz. </i>
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
#include "tc.h"


/*! \name TC Channel Choice
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_TC                    (&AVR32_TC)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC_A0_0_0_FUNCTION
// Note that TC_A0_0_0 pin is pin 9 (PB23 / GPIO 55) on AT32UC3A0512 QFP144.
#elif BOARD == EVK1101
#  define EXAMPLE_TC                    (&AVR32_TC)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC_A0_0_0_FUNCTION
// Note that TC_A0_0_0 pin is pin 6 (PB00 / GPIO 32) on AT32UC3B0256 QFP64.
#elif BOARD == EVK1104
#  define EXAMPLE_TC                    (&AVR32_TC0)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC0_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC0_A0_0_0_FUNCTION
// Note that TC0_A0_0_0 pin is pin 121 (PB05 / GPIO 37) on AT32UC3A3256 QFP144.
#elif BOARD == STK600_RCUC3L0
#  define EXAMPLE_TC                    (&AVR32_TC1)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC1_A0_0_1_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC1_A0_0_1_FUNCTION
// Note that TC1_A0_0_1 pin is pin 6 (PB00 / GPIO 32) on AT32UC3L064 TQFP48; with
// the STK600 + RCUC3L0 routing card, PB00 is routed to STK600.PORTD.PD0.
#elif BOARD == UC3L_EK
#  define EXAMPLE_TC                    (&AVR32_TC1)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC1_A0_0_1_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC1_A0_0_1_FUNCTION
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000  // DFLL target frequency, in Hz
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000  // MCU clock target frequency, in Hz
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000  // PBA clock target frequency, in Hz
    /*! \name Parameters to pcl_configure_clocks().
     */
    //! @{
    static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
    static pcl_freq_param_t pcl_dfll_freq_param =
    {
      .main_clk_src = PCL_MC_DFLL0,
      .cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
      .pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
      .pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
      .dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
      .pextra_params = &gc_dfllif_ref_opt
    };
    //! @}
#elif BOARD == UC3C_EK
#  define EXAMPLE_TC                    (&AVR32_TC0)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC0_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC0_A0_0_0_FUNCTION
// Note that TC0_A0_0_0 pin is pin 58 (PB19 / GPIO 51) on AT32UC3C0512C QFP144.
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_TC                    (&AVR32_TC)
#  define EXAMPLE_TC_CHANNEL_ID         0
#  define EXAMPLE_TC_CHANNEL_PIN        AVR32_TC_A0_0_0_PIN
#  define EXAMPLE_TC_CHANNEL_FUNCTION   AVR32_TC_A0_0_0_FUNCTION
// Note that TC0_A0_0_0 pin is pin 32 (PB00 / GPIO 32) PORTE.PE01

#endif

#if !defined(EXAMPLE_TC)             || \
    !defined(EXAMPLE_TC_CHANNEL_ID)  || \
    !defined(EXAMPLE_TC_CHANNEL_PIN) || \
    !defined(EXAMPLE_TC_CHANNEL_FUNCTION)
#  error The TC preprocessor configuration to use in this example is missing.
#endif
//! @}


/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  // The timer/counter instance and channel number are used in several functions.
  // It's defined as local variable for ease-of-use causes and readability.
  volatile avr32_tc_t *tc = EXAMPLE_TC;

  // Options for waveform generation.
  tc_waveform_opt_t waveform_opt =
  {
    .channel  = EXAMPLE_TC_CHANNEL_ID,        // Channel selection.

    .bswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOB.
    .beevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOB.
    .bcpc     = TC_EVT_EFFECT_NOOP,           // RC compare effect on TIOB.
    .bcpb     = TC_EVT_EFFECT_NOOP,           // RB compare effect on TIOB.

    .aswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOA.
    .aeevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOA.
    .acpc     = TC_EVT_EFFECT_TOGGLE,         // RC compare effect on TIOA: toggle.
    .acpa     = TC_EVT_EFFECT_TOGGLE,         // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

    .wavsel   = TC_WAVEFORM_SEL_UP_MODE,      // Waveform selection: Up mode without automatic trigger on RC compare.
    .enetrg   = false,                        // External event trigger enable.
    .eevt     = TC_EXT_EVENT_SEL_TIOB_INPUT,  // External event selection.
    .eevtedg  = TC_SEL_NO_EDGE,               // External event edge selection.
    .cpcdis   = false,                        // Counter disable when RC compare.
    .cpcstop  = false,                        // Counter clock stopped with RC compare.

    .burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
    .clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
    .tcclks   = TC_CLOCK_SOURCE_TC3           // Internal source clock 3, connected to fPBA / 8.
  };

#if BOARD == UC3L_EK
  // Note: on the AT32UC3L-EK board, there is no crystal/external clock connected
  // to the OSC0 pinout XIN0/XOUT0. We shall then program the DFLL and switch the
  // main clock source to the DFLL.
  pcl_configure_clocks(&pcl_dfll_freq_param);
  // Note: since it is dynamically computing the appropriate field values of the
  // configuration registers from the parameters structure, this function is not
  // optimal in terms of code size. For a code size optimal solution, it is better
  // to create a new function from pcl_configure_clocks_dfll0() and modify it
  // to use preprocessor computation from pre-defined target frequencies.
#else
  // Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
  // frequency FOSC0) with an appropriate startup time then switch the main clock
  // source to Osc0.
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif

  // Assign I/O to timer/counter channel pin & function.
  gpio_enable_module_pin(EXAMPLE_TC_CHANNEL_PIN, EXAMPLE_TC_CHANNEL_FUNCTION);

  // Initialize the timer/counter.
  tc_init_waveform(tc, &waveform_opt);  // Initialize the timer/counter waveform.

  // Set the compare triggers.
  tc_write_ra(tc, EXAMPLE_TC_CHANNEL_ID, 0x0600);     // Set RA value.
  tc_write_rc(tc, EXAMPLE_TC_CHANNEL_ID, 0x2000);     // Set RC value.

  // Start the timer/counter.
  tc_start(tc, EXAMPLE_TC_CHANNEL_ID);

  //*** Sleep mode
  // If there is a chance that any PB write operations are incomplete, the CPU
  // should perform a read operation from any register on the PB bus before
  // executing the sleep instruction.
  AVR32_INTC.ipr[0];  // Dummy read

  // - Go into a sleep mode (while still maintaining TC activity)
  SLEEP(AVR32_PM_SMODE_FROZEN);

  while (true);
}
