/*****************************************************************************
 *
 * \file
 * \brief Timer/Counter example 2.
 *
 * This example involves 2 timer/counter channels, one configured in capture
 * mode(input) and the other configured in Waveform mode(output) to generate a
 * PWM on the output.
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
 * This example involves 2 timer/counter channels, one configured in capture
 * mode (input) and the other configured in waveform mode (output) to generate a
 * PWM on the output.
 *
 * Channel 1 of a timer/counter module is used with the input pin TIOA1. You
 * will find the port and pin number in the datasheet of your device. The
 * selected timer input clock is the internal clock labelled TC4 referred to
 * as TIMER_CLOCK4 in the datasheet.
 *
 * Channel 0 of a timer/counter module is used with the output pin TIOA0.
 * You will find the port and pin number in the datasheet of your device.
 * The selected timer input clock is the internal clock labelled TC3
 * referred to as TIMER_CLOCK3 in the datasheet.
 * \note
 * - On the AT32UC3A0512, the input pin is TIOA1 is mapped on PB25 and the output pin TIOA0 is mapped on PB23. \n
 * - On the AT32UC3B0256, the input pin is TIOA1 is mapped on PA21 and the output pin TIOA0 is mapped on PB00. \n
 * - On the AT32UC3A3256, the input pin is TIOA1 is mapped on PA01 and the output pin TIOA0 is mapped on PB05. \n
 * - On the AT32UC3C0512C, the input pin is TIOA1 is mapped on PA22 and the output pin TIOA0 is mapped on PB19. \n
 * - On the ATUC128D3, with the STK600 + RCUC3D routing card setup, the chosen
 *   input pin TIOA1 for the Timer/Counter 0(TC0) is mapped on PB00 (PB00 is routed
 *   to STK600.PORTE.PE0), and the chosen output pin TIOA1 is mapped on PA11 (PA11
 *   is routed to STK600.PORTB.PB3). \n
 * - On the AT32UC3L064, with the STK600 + RCUC3L0 routing card setup, the chosen
 *   input pin TIOA1 for the Timer/Counter 1(TC1) is mapped on PB06 (PB06 is routed
 *   to STK600.PORTD.PD6), and the chosen output pin TIOA0 is mapped on PB00 (PB00
 *   is routed to STK600.PORTD.PD0). \n
 * - On the AT32UC3L064, with the AT32UC3L-EK evaluation kit, the chosen input pin
 *   TIOA1 for the Timer/Counter 1(TC1) is mapped on PB01, and the chosen output
 *   pin TIOA0 is mapped on PB00.
 *
 * The 16-bit input timer/counter channel will cycle from 0x0000 to 0xFFFF
 * or until a falling edge is detected on the input pin, in which case the
 * counter is captured in RA before being reset then started with the same
 * rules. Consequently, RA is a measure of the input period modulo 16 bits.
 *
 * The 16-bit output timer/counter channel will cycle from 0x0000 to 0xFFFF,
 * starting at 0x0000 with a high output pin level and lowering the output
 * pin level when it reaches the value of RA extracted from the input timer/
 * counter while raising the output pin level when it reaches the value of
 * RC left at 0. This will hence produce a PWM output signal with a duty
 * cycle depending on the period of the input signal.
 *
 * \section files Main Files
 * - tc.c: TC driver;
 * - tc.h: TC driver header file;
 * - tc_example2.c: TC example 2.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a TC module can be used. This example has been tested
 * with the following board:
 * - EVK1100 evaluation kit
 * - EVK1101 evaluation kit
 * - EVK1104 evaluation kit
 * - STK600 starter kit with the RCUC3L0 routing card, 12MHz crystal in the STK600's crystal socket
 * - STK600 starter kit with the RCUC3D routing card, 12MHz crystal in the STK600's crystal socket
 * - AT32UC3L-EK evaluation kit
 * - AT32UC3C-EK evaluation kit
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
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC_A0_0_0_PIN
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC_A0_0_0_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC_A1_0_0_PIN
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC_A1_0_0_FUNCTION
#elif BOARD == EVK1101
#  define EXAMPLE_TC                    (&AVR32_TC)
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC_A0_0_0_PIN
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC_A0_0_0_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC_A1_0_0_PIN
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC_A1_0_0_FUNCTION
#elif BOARD == EVK1104
#  define EXAMPLE_TC                    (&AVR32_TC0)
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC0_A0_0_0_PIN
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC0_A0_0_0_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC0_A1_0_0_PIN
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC0_A1_0_0_FUNCTION
#elif BOARD == STK600_RCUC3L0
#  define EXAMPLE_TC                    (&AVR32_TC1)
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC1_A0_0_1_PIN  // With the RCUC3L0 routing card, mapped on STK600.PORTD.PD0
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC1_A0_0_1_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC1_A1_0_0_PIN  // With the RCUC3L0 routing card, mapped on STK600.PORTD.PD6
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC1_A1_0_0_FUNCTION
#elif BOARD == UC3L_EK
#  define EXAMPLE_TC                    (&AVR32_TC1)
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC1_A0_0_1_PIN  // PB00
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC1_A0_0_1_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC1_A1_0_1_PIN  // PB01
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC1_A1_0_1_FUNCTION
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
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC0_A0_0_0_PIN  // PB19
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC0_A0_0_0_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC0_A1_0_0_PIN  //PB22
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC0_A1_0_0_FUNCTION
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_TC                    (&AVR32_TC)
#  define EXAMPLE_TC_OUTPUT_PIN         AVR32_TC_A0_0_0_PIN  // PB00
#  define EXAMPLE_TC_OUTPUT_FUNCTION    AVR32_TC_A0_0_0_FUNCTION
#  define EXAMPLE_TC_INPUT_PIN          AVR32_TC_A1_0_0_PIN   //PA11
#  define EXAMPLE_TC_INPUT_FUNCTION     AVR32_TC_A1_0_0_FUNCTION
#endif

#if !defined(EXAMPLE_TC)      || \
    !defined(EXAMPLE_TC_OUTPUT_PIN)      || \
    !defined(EXAMPLE_TC_OUTPUT_FUNCTION) || \
    !defined(EXAMPLE_TC_INPUT_PIN)       || \
    !defined(EXAMPLE_TC_INPUT_FUNCTION)
#  error The TC preprocessor configuration to use in this example is missing.
#endif
//! @}


/*! \brief Initializes the timer/counter capture.
 */
static void init_tc_input(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Options for capture mode.
  tc_capture_opt_t capture_opt =
  {
    .channel  = channel,                      // Channel selection.

    .ldrb     = TC_SEL_NO_EDGE,               // RB loading selection.
    .ldra     = TC_SEL_FALLING_EDGE,          // RA loading selection.

    .cpctrg   = TC_NO_TRIGGER_COMPARE_RC,     // RC compare trigger enable.
    .abetrg   = TC_EXT_TRIG_SEL_TIOA,         // TIOA or TIOB external trigger selection.
    .etrgedg  = TC_SEL_FALLING_EDGE,          // External trigger edge selection.

    .ldbdis   = false,                        // Counter clock disable with RB loading.
    .ldbstop  = false,                        // Counter clock stopped with RB loading.

    .burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
    .clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
    .tcclks   = TC_CLOCK_SOURCE_TC4           // Internal source clock 4, connected to fPBA / 32.
  };

  // Initialize the timer/counter capture.
  tc_init_capture(tc, &capture_opt);
}


/*! \brief Initializes the timer/counter waveform.
 */
static void init_tc_output(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Options for waveform generation.
  tc_waveform_opt_t waveform_opt =
  {
    .channel  = channel,                      // Channel selection.

    .bswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOB.
    .beevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOB.
    .bcpc     = TC_EVT_EFFECT_NOOP,           // RC compare effect on TIOB.
    .bcpb     = TC_EVT_EFFECT_NOOP,           // RB compare effect on TIOB.

    .aswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOA.
    .aeevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOA.
    .acpc     = TC_EVT_EFFECT_SET,            // RC compare effect on TIOA.
    .acpa     = TC_EVT_EFFECT_CLEAR,          // RA compare effect on TIOA.

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

  // Initialize the timer/counter waveform.
  tc_init_waveform(tc, &waveform_opt);
}


/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  static const gpio_map_t TC_GPIO_MAP =
  {
    // Assign I/O to timer/counter TIOA1 pin function (input):
    // optional as far as the port pin is not driven by the MCU.
    {EXAMPLE_TC_INPUT_PIN, EXAMPLE_TC_INPUT_FUNCTION},
    // On the AT32UC3A0512, the input pin TIOA1 is mapped on PB25.
    // On the AT32UC3B0256, the input pin TIOA1 is mapped on PA21.
    // On the AT32UC3A3256, the input pin TIOA1 is mapped on PA01.
    // On the AT32UC3L064, the input pin TIOA1 is mapped on PB06; with the RCUC3L0
    // On the AT32UC3C0512c, the input pin TIOA1 is mapped on PB22.
    // routing card, this is mapped on STK600.PORTD.PD6

    // Assign I/O to timer/counter TIOA0 pin function (output).
    {EXAMPLE_TC_OUTPUT_PIN, EXAMPLE_TC_OUTPUT_FUNCTION}
    // On the AT32UC3A0512, the output pin TIOA0 is mapped on PB23.
    // On the AT32UC3B0256, the output pin TIOA0 is mapped on PB00.
    // On the AT32UC3A3256, the output pin TIOA0 is mapped on PB05.
    // On the AT32UC3L064, the output pin TIOA0 is mapped on PB00; with the RCUC3L0
    // On the AT32UC3C0512C, the output pin TIOA0 is mapped on PB19.
    // routing card, this is mapped on STK600.PORTD.PD0
  };

  // The timer/counter instance and channel numbers are used in several functions.
  // It's defined as local variable for ease-of-use causes and readability.
  volatile avr32_tc_t *tc = EXAMPLE_TC;
  unsigned int input_channel = 1;
  unsigned int output_channel = 0;

  // Used to read the RA value for the input timer/counter instance.
  int ra = 0;

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

  // Assign I/Os to timer/counter.
  gpio_enable_module(TC_GPIO_MAP, sizeof(TC_GPIO_MAP) / sizeof(TC_GPIO_MAP[0]));

  // Initialize the timers/counters.
  init_tc_input(tc, input_channel);
  init_tc_output(tc, output_channel);

  // Set the compare trigger.
  tc_write_ra(tc, output_channel, 0x4000);

  // Start the timers/counters.
  tc_start(tc, input_channel);
  tc_start(tc, output_channel);

  while (true)
  {
    ra = tc_read_ra(tc, input_channel);
    if (ra > 0)
    {
      // RA of the input channel has changed, so it has detected a falling edge.
      // Update the RA of the output channel.
      tc_write_ra(tc, output_channel, ra);
    }
  }
}
