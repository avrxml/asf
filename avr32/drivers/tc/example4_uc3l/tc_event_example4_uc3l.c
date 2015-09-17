/*****************************************************************************
 *
 * \file
 * \brief Timer/Counter and Peripheral Event System example.
 *
 * This example involves one TC channel configured in capture mode (input). Upon
 * a GPIO pin change, an event triggers a TC.Ai capture.
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
 * defines, enums, and typedefs for the TC driver.\n It also gives an example of
 * the usage of the TC module.
 *
 * This example involves one TC channel configured in capture mode (input). Upon
 * a chosen GPIO pin change, an event triggers a TC.Ai capture.
 *
 * The GPIO pin PA11 is programmed to output a peripheral event whenever the
 * interrupt condition pin value falling edge is detected.
 *
 * Since we're using PA11 as event generator, channel 1 of the TC0 module is used
 * as event user (cf. the table "Peripheral Event Generators and Users Interconnection"
 * from the UC3L datasheet). The selected timer input clock is the internal clock
 * labelled TC4 referred to as TIMER_CLOCK4 in the datasheet.
 *
 * \note
 * - On the STK600 + RCUC3L0 routing card setup, the chosen GPIO pin PA11 generating
 *   the event is routed to STK600.PORTB.PB3.
 * - On the AT32UC3L-EK, the chosen GPIO pin PA11 generating the event is routed
 *   to the "WAKE" button.
 *
 * Upon a falling edge detection on the GPIO pin, an event is generated. This event
 * generator triggers a TC0.A1 capture. When the TC0.A1 capture occurs, an interrupt
 * is triggered and LED0 (connect STK600.PORTA.PA4 to STK600.LEDS.LED0) is toggled
 * by the interrupt handler.
 *
 * \section files Main Files
 * - tc.c: TC driver;
 * - tc.h: TC driver header file;
 * - tc_event_example4_uc3l.c: TC and Peripheral Event example.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 UC3L devices with a TC module and a Peripheral Event System can be
 * used. This example has been tested with the following boards:
 * - STK600 starter kit with the RCUC3L0 routing card
 * - AT32UC3L-EK evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed:
 * - Running on OSC0 from an external crystal at 12MHz : STK600+RCUC3L routing card setup
 * - With the DFLL using an internal osc, the CPU speed is configured at 12MHz: AT32UC3L-EK
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#if __GNUC__
#  include "intc.h"
#endif
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "tc.h"


/*! \name TC Channel Choice
 */
//! @{
#  define EXAMPLE_TC                    (&AVR32_TC0)
#  define EXAMPLE_TC_CHANNEL            1
#  define EXAMPLE_TC_IRQ_GROUP          AVR32_TC0_IRQ_GROUP
#  define EXAMPLE_TC_IRQ                AVR32_TC0_IRQ1  // Because we use channel 1.
#  define EXAMPLE_EVENT_PIN             AVR32_PIN_PA11
// On the AT32UC3L-EK, PA11 is connected to the WAKE button.
// With the RCUC3L0 routing card, PA11 is mapped on STK600.PORTB.PB3


#if BOARD == UC3L_EK
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
#endif

#if !defined(EXAMPLE_TC)                || \
    !defined(EXAMPLE_TC_CHANNEL)        || \
    !defined(EXAMPLE_TC_IRQ_GROUP)      || \
    !defined(EXAMPLE_TC_IRQ)            || \
    !defined(EXAMPLE_EVENT_PIN)
#  error The preprocessor configuration to use in this example is missing.
#endif
//! @}


static volatile int chan_status = 0;

/*! \brief TC interrupt handler.
 */
#if defined(__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
#pragma handler = EXAMPLE_TC_IRQ_GROUP, 3
__interrupt
#endif
static void tc_irq_handler(void)
{
  // Clear the interrupt flag. This is a side effect of reading the TC SR.
  chan_status = tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);

  // Toggle LED0.
  gpio_tgl_gpio_pin(LED0_GPIO);
}


/*! \brief Initializes the timer/counter capture.
 */
static void init_tc_input(volatile avr32_tc_t *tc, unsigned int channel)
{
  // Options for capture mode.
  tc_capture_opt_t capture_opt =
  {
    .channel  = channel,                      // Channel selection.

    .ldrb     = TC_SEL_NO_EDGE,               // RB loading selection.
    .ldra     = TC_SEL_RISING_EDGE,           // RA loading selection.

    .cpctrg   = TC_NO_TRIGGER_COMPARE_RC,     // RC compare trigger disabled.
    .abetrg   = TC_EXT_TRIG_SEL_TIOA,         // TIOA external trigger selection.
    .etrgedg  = TC_SEL_RISING_EDGE,           // External trigger edge selection.

    .ldbdis   = false,                        // Counter clock disable with RB loading.
    .ldbstop  = false,                        // Counter clock stopped with RB loading.

    .burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
    .clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
    .tcclks   = TC_CLOCK_SOURCE_TC4           // Internal source clock 4, connected to fPBA / 32.
  };

  // Initialize the timer/counter capture.
  tc_init_capture(tc, &capture_opt);
}


/*! \brief Main function:
 *  - Register the TC interrupt (GCC only)
 */
int main(void)
{
  static const tc_interrupt_t TC_INTERRUPT =
  {
    .etrgs = 0,
    .ldrbs = 0,
    .ldras = 1, // Generate an interrupt upon RA load occurrence
    .cpcs  = 0,
    .cpbs  = 0,
    .cpas  = 0,
    .lovrs = 0,
    .covfs = 0
  };
  // The timer/counter instance and channel numbers are used in several functions.
  // It's defined as local variable for ease-of-use and readability.
  volatile avr32_tc_t *tc = EXAMPLE_TC;

  // Clear LED0.
  gpio_set_gpio_pin(LED0_GPIO);


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

  //#
  //# Configure the EXAMPLE_EVENT_PIN gpio to generate an event on falling edge,
  //# with input Glitch filter enabled.
  //#
  //# Note: Make sure that the corresponding pin functions for the TIOA line are
  //# not enabled.
  //#
  // Configure
  gpio_configure_pin_periph_event_mode(EXAMPLE_EVENT_PIN, GPIO_FALLING_EDGE, true);
  // Enable the pull-up so that we can easily get a falling edge by connecting the
  // pin to the GND.
  gpio_enable_pin_pull_up(EXAMPLE_EVENT_PIN);
  // Enable the peripheral event generation of the EXAMPLE_EVENT_PIN pin.
  gpio_enable_pin_periph_event(EXAMPLE_EVENT_PIN);

  Disable_global_interrupt();

  // The INTC driver has to be used only for GNU GCC for AVR32.
#ifdef __GNUC__
  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Register the TC interrupt handler to the interrupt controller.
  INTC_register_interrupt(&tc_irq_handler, EXAMPLE_TC_IRQ, AVR32_INTC_INT3);
#endif

  Enable_global_interrupt();

  // Initialize the timer/counter.
  init_tc_input(tc, EXAMPLE_TC_CHANNEL);

  // Configure the chosen interrupts of the timer/counter.
  tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &TC_INTERRUPT);

  // Start the timer/counter.
  tc_start(tc, EXAMPLE_TC_CHANNEL);

  while(1);
}
