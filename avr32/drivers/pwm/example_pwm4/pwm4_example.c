/*****************************************************************************
 *
 * \file
 *
 * \brief PWM4 example driver for AVR32 UC3.
 *
 * This file provides an example for the PWM4 on AVR32 UC3 devices.
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
 * This is the documentation for the data structures, functions, variables, defines, enums, and
 * typedefs for the PWM driver. <BR>It also gives an example of the usage of the
 * PWM (module version above 400) on UC3 products.
 * This example shows how to configure a PWM and output it on a GPIO.\n
 * <b>Operating mode</b>:\n Check the used PWM pin with an oscilloscope, you should see a
 * PWM frequency of 2.8 MHz with a PWM duty cycle of 1/2.\n
 * - On \b UC3C_EK, PWM_L and PWM_H are pin number 40 (PB08) and pin number 41 (PB09) with AT32UC3C0512 in QFP144 package.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a PWM module version above 400 can be used. This example has been tested
 * with the following setup:\n
 * - UC3C_EK evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed: <i>External oscillator (16 MHz)</i>.
 *
 * \section contactinfo Contact Info
 * For more info about Atmel AVR32 visit
 * <A href="http://www.atmel.com/products/AVR32/" >Atmel AVR32</A>\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "pwm4.h"
#include "gpio.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"

/*! \name PWM Configuration
 */
//! @{
#if BOARD == UC3C_EK
#  define EXAMPLE_PWM_L_PIN             AVR32_PWM_PWML_0_1_PIN
#  define EXAMPLE_PWM_L_FUNCTION        AVR32_PWM_PWML_0_1_FUNCTION
#  define EXAMPLE_PWM_H_PIN             AVR32_PWM_PWMH_0_1_PIN
#  define EXAMPLE_PWM_H_FUNCTION        AVR32_PWM_PWMH_0_1_FUNCTION
#  define EXAMPLE_PWM_CHANNEL_ID        0
#endif

#if !defined(EXAMPLE_PWM_L_PIN)      || \
    !defined(EXAMPLE_PWM_L_FUNCTION) || \
    !defined(EXAMPLE_PWM_H_PIN)      || \
    !defined(EXAMPLE_PWM_H_FUNCTION) || \
    !defined(EXAMPLE_PWM_CHANNEL_ID)
#  error The PWM configuration to use in this example is missing.
#endif
//! @}

// Start oscillator and enable PLL0, sourced by OSC0
static void local_start_highfreq_clock(void)
{
  const scif_pll_opt_t opt = {
            .osc = SCIF_OSC0,     // Sel Osc0/PLL0 or Osc1/PLL1
            .lockcount = 16,      // lockcount in main clock for the PLL wait lock
            .div = 1,             // DIV=1 in the formula
            .mul = 6,             // MUL=7 in the formula
            .pll_div2 = 1,        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
            .pll_wbwdisable = 0,  // pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
            .pll_freq = 1,        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
  };
  // Switch main clock to Osc0.
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

  /* Setup PLL0 on Osc0, mul=7 ,no divisor, lockcount=16, ie. (16Mhzx7)/(div2) = 56MHz output */
  scif_pll_setup(SCIF_PLL0, &opt); // lockcount in main clock for the PLL wait lock

  /* Enable PLL0 */
  scif_pll_enable(SCIF_PLL0);

  /* Wait for PLL0 locked */
  scif_wait_for_pll_locked(SCIF_PLL0) ;
}

// Start PWM generic clock input
static void pwm_start_gc(void)
{
  scif_gc_setup(AVR32_SCIF_GCLK_PWM,
                SCIF_GCCTRL_PLL0,
                AVR32_SCIF_GC_NO_DIV_CLOCK,
                0);
  // Now enable the generic clock
  scif_gc_enable(AVR32_SCIF_GCLK_PWM);
}

/*! \brief Main function. Execution starts here.
 *
 *  \return 0 on success
 */
int main(void)
{
  pwm_opt_t pwm_opt;                // PWM option config.
  avr32_pwm_channel_t pwm_channel = {{0}, // cmr
                                     {0}, // cdty
                                     {0}, // cdtyupd
                                     {0}, // cprd
                                     {0}, // cprdupd
                                     {0}, // ccnt
                                     {0}, // dt
                                     {0}};// dtupd  ;  One channel config.
  unsigned int channel_id;

  // Start Main Clock On external 16MHz Oscillator
  // Start PLL for PWM
  local_start_highfreq_clock();
  // Start Enable Generic Clock with PLL as source clock
  pwm_start_gc();

  channel_id = EXAMPLE_PWM_CHANNEL_ID;
  gpio_enable_module_pin(EXAMPLE_PWM_L_PIN, EXAMPLE_PWM_L_FUNCTION);
  gpio_enable_module_pin(EXAMPLE_PWM_H_PIN, EXAMPLE_PWM_H_FUNCTION);
  // PWM controller configuration.
  pwm_opt.diva = AVR32_PWM_DIVA_CLK_OFF;
  pwm_opt.divb = AVR32_PWM_DIVB_CLK_OFF;
  pwm_opt.prea = AVR32_PWM_PREA_CCK;
  pwm_opt.preb = AVR32_PWM_PREB_CCK;

  pwm_opt.fault_detection_activated = false;
  pwm_opt.sync_channel_activated    = true;
  pwm_opt.sync_update_channel_mode  = PWM_SYNC_UPDATE_MANUAL_WRITE_MANUAL_UPDATE;
  pwm_opt.sync_channel_select[0]    = true;
  pwm_opt.sync_channel_select[1]    = true;
  pwm_opt.sync_channel_select[2]    = false;
  pwm_opt.sync_channel_select[3]    = false;
  pwm_opt.cksel                     = PWM_CKSEL_GCLK;
  pwm_init(&pwm_opt);

  // Update the period
  pwm_update_period_value(10);

  // Channel configuration
  pwm_channel.CMR.dte   = 1;        // Enable Deadtime for complementary Mode
  pwm_channel.CMR.dthi  = 1;        // Deadtime Inverted on PWMH
  pwm_channel.CMR.dtli  = 0;        // Deadtime Not Inverted on PWML
  pwm_channel.CMR.ces   = 0;        // 0/1 Channel Event at the End of PWM Period
  pwm_channel.CMR.calg  = PWM_MODE_LEFT_ALIGNED;       // Channel mode.
  pwm_channel.CMR.cpol  = PWM_POLARITY_LOW;            // Channel polarity.
  pwm_channel.CMR.cpre  = AVR32_PWM_CPRE_CCK;           // Channel prescaler.
  pwm_channel.cdty      = 10;       // Channel duty cycle, should be < CPRD.
  pwm_channel.cprd      = 20;       // Channel period.

  // With these settings, the output waveform period will be :
  // (56MHz)/20 == 2.8MHz == (MCK/prescaler)/period, with MCK == 56MHz,
  // prescaler == 1, period == 20.

  pwm_channel_init(channel_id, &pwm_channel); // Set channel configuration to channel 0
  pwm_start_channels((1 << channel_id));  // Start channel 0 & 1.

  while(1);
}
