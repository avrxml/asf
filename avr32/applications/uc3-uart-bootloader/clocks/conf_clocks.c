/**************************************************************************
 *
 * \file
 *
 * \brief Clocks configuration functions.
 *
 * This file contains the functions to apply the ISP clock settings.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "conf_clocks.h"
#include "power_clocks_lib.h" // Used for embedded includes.


//_____ D E F I N I T I O N S ______________________________________________

#if UC3L
// Low footprint implementation (preprocessor computed parameters)
int conf_clocks_uc3l(void);
int conf_clocks_uc3l(void)
{
  scif_dfll_closedloop_conf_t DfllConfig;
  // Configure and start the DFLL main reference generic clock (CLK_DFLLIF_REF).
  AVR32_SCIF.gcctrl[AVR32_SCIF_GCLK_DFLL0_REF] = ((0 << AVR32_SCIF_GCCTRL_DIV_OFFSET)&AVR32_SCIF_GCCTRL_DIV_MASK)
                            |((0 << AVR32_SCIF_GCCTRL_DIVEN_OFFSET)&AVR32_SCIF_GCCTRL_DIVEN_MASK)
                            |((SCIF_GCCTRL_SLOWCLOCK << AVR32_SCIF_GCCTRL_OSCSEL_OFFSET)&AVR32_SCIF_GCCTRL_OSCSEL_MASK)
                            |(AVR32_SCIF_GCCTRL_CEN_MASK);

  // Configure the DFLL.
  // The coarse value (= (dfll_f - SCIF_DFLL_MINFREQ_KHZ)*255/(SCIF_DFLL_MAXFREQ_KHZ - SCIF_DFLL_MINFREQ_KHZ))
  DfllConfig.coarse = ((unsigned long long)(TARGET_DFLL_FREQ_HZ - SCIF_DFLL_MINFREQ_HZ)*255)/(SCIF_DFLL_MAXFREQ_HZ - SCIF_DFLL_MINFREQ_HZ);

  // imul = (fDFLL)/fref,
  // fmul = (fDFLL*2^16)/fref - imul*2^16,
  // with fref being the frequency of the DFLL main reference generic clock
  // and fDFLL being the target frequency of the DFLL
  DfllConfig.imul = ((unsigned long long)TARGET_DFLL_FREQ_HZ)/SCIF_SLOWCLOCK_FREQ_HZ;
  DfllConfig.fmul = ((unsigned long long)TARGET_DFLL_FREQ_HZ<<16)/SCIF_SLOWCLOCK_FREQ_HZ - ((unsigned long long)(DfllConfig.imul)<<16);

  // The fine and coarse maxstep values
  DfllConfig.finemaxstep = 0x0000004;
  DfllConfig.coarsemaxstep = 0x0000004;

  // Note: Dithering disabled.

  // Configure and start the DFLL0 in closed loop mode.
  if(scif_dfll0_closedloop_start(&DfllConfig))
    return FAIL;

  //#
  //# Set the Synchronous clock division ratio for each clock domain
  //#
  pm_set_all_cksel(TARGET_DFLL_FREQ_HZ, TARGET_CPU_FREQ_HZ, TARGET_PBA_FREQ_HZ, TARGET_PBB_FREQ_HZ);

  //#
  //# Set the Flash wait state and the speed read mode (depending on the target CPU frequency).
  //#
  flashcdw_set_flash_waitstate_and_readmode(TARGET_CPU_FREQ_HZ);

  //#
  //# Switch the main clock source to the selected clock.
  //#
  pm_set_mclk_source(PM_CLK_SRC_DFLL0);

  return PASS;
}

//! \brief Undo the settings performed by conf_clocks_uc3l().
//
void stop_clocks_uc3l(void);
void stop_clocks_uc3l(void)
{
  //# Switch the main clock source to the RCSYS clock.
  pm_set_mclk_source(PM_CLK_SRC_SLOW);

  //# Set the Flash wait state and the speed read mode (depending on the new CPU frequency).
  flashcdw_set_flash_waitstate_and_readmode(SCIF_SLOWCLOCK_FREQ_HZ);

  //# Set the Synchronous clock division ratio for each clock domain
  pm_set_all_cksel( SCIF_SLOWCLOCK_FREQ_HZ, SCIF_SLOWCLOCK_FREQ_HZ,
                    SCIF_SLOWCLOCK_FREQ_HZ, SCIF_SLOWCLOCK_FREQ_HZ );

  //# Disable the DFLL (reset the SCIF.DFLLOCONF register to its reset default value)
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0CONF);
  AVR32_SCIF.dfll0conf = AVR32_SCIF_DFLL0CONF_RESETVALUE;
  AVR32_LEAVE_CRITICAL_REGION( );

  //# Reset the SCIF.DFLL0MUL register to its reset default value
  AVR32_ENTER_CRITICAL_REGION( );
#if AVR32_SCIF_H_VERSION < 110
  SCIF_UNLOCK(AVR32_SCIF_DFLL0FMUL);
  AVR32_SCIF.dfll0fmul = AVR32_SCIF_DFLL0FMUL_RESETVALUE;
#else
  SCIF_UNLOCK(AVR32_SCIF_DFLL0MUL);
  AVR32_SCIF.dfll0mul = AVR32_SCIF_DFLL0MUL_RESETVALUE;
#endif
  AVR32_LEAVE_CRITICAL_REGION( );

  //# Reset the SCIF.DFLL0STEP register to its reset default value
  AVR32_ENTER_CRITICAL_REGION( );
  SCIF_UNLOCK(AVR32_SCIF_DFLL0STEP);
  AVR32_SCIF.dfll0step = AVR32_SCIF_DFLL0STEP_RESETVALUE;
  AVR32_LEAVE_CRITICAL_REGION( );

  //# Stop the DFLL main reference generic clock (CLK_DFLLIF_REF) and reset its
  //# SCIF control register to its reset default value.
  AVR32_SCIF.gcctrl[AVR32_SCIF_GCLK_DFLL0_REF] = AVR32_SCIF_GCCTRL_RESETVALUE;
}

#elif UC3A || UC3B || UC3C

int conf_clocks_uc3_extosc0(void)
{
    int cpu_freq_hz, mul;
  const int freq_hz[] = { 8000000, 12000000, 16000000 };
  const struct autobaud_cfg_osc_cmp_t autobaud_cfg = {
    .osc_ref_freq_hz = 115000,
    .osc_ref_counter_reset = osc_rtc_counter_reset,
    .osc_ref_counter_value = osc_rtc_counter_value,
    .osc_target_counter_reset = osc_isp_counter_reset,
    .osc_target_counter_value = osc_isp_counter_value,
    .convergence_rate = 10000
  };
  const struct autobaud_matching_freq_t match_freq = {
    .freq_hz = freq_hz,
    .nb_entries = sizeof(freq_hz)/sizeof(freq_hz[0])
  };
  bool sav_glob_int_en;

  #define MAX_OSC_FREQ    16000000

  //##
  //## 1) External Oscillator Frequency Automatic Identification
  //##
  // Switch to OSC ISP
  // Set max startup time to make sure any crystal will be supported
  // We cannot use a TC to measure this OSC frequency because the master clock must be faster than the clock selected by the TC
  pcl_switch_to_osc(PCL_OSC##ISP_OSC, MAX_OSC_FREQ, ATPASTE3(AVR32_PM_OSCCTRL, ISP_OSC, _STARTUP_16384_RCOSC));
  // Intialize the RTC with the internal RC oscillator
  // RTC will count at the frequency of 115KHz/2
  rtc_init(&AVR32_RTC, RTC_OSC_RC, 0);
  rtc_enable(&AVR32_RTC);

  // Calculate the frequency
  if (!(cpu_freq_hz = autobaud_detect_osc_cmp(&autobaud_cfg)))
    cpu_freq_hz = 12000000;
  cpu_freq_hz = autobaud_match_frequency(cpu_freq_hz, &match_freq);

  switch (cpu_freq_hz)
  {
    case 8000000:
      mul = 5;
      break;
    case 12000000:
      mul = 3;
      break;
    case 16000000:
      mul = 2;
      break;
    default:
      mul = 3;
  }

  //##
  //## 2) Set up CPU freq & PBx freq to 12MHz
  //##
#if UC3A || UC3B
  // Set PLL0 VCO @ 96 MHz
  pm_pll_setup(PM, 0,                         // pll
                   mul,                       // mul
                   0,                         // div
                   ISP_OSC,                   // osc
                   63);                       // lockcount

  AVR32_ENTER_CRITICAL_REGION();

  // Set PLL0 @ 48 MHz
  pm_pll_set_option(PM, 0,  // pll
                    1,      // pll_freq
                    1,      // pll_div2
                    0);     // pll_wbwdisable

  // Enable PLL0
  pm_pll_enable(PM, 0);

  // Wait for PLL0 locked with a 10-ms time-out
  pm_wait_for_pll0_locked(PM);

  // Use 1 flash wait state
  flashc_set_wait_state(1);

  // Switch the main clock to PLL0
  pm_switch_to_clock(PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  // fPBA: 12 MHz
  // fPBB: 12 MHz
  // fHSB: 12 MHz
  pm_cksel(PM, 1,   // pbadiv
               1,   // pbasel
               1,   // pbbdiv
               1,   // pbbsel
               1,   // hsbdiv
               1);  // hsbsel

  // Use 0 flash wait state
  flashc_set_wait_state(0);

  AVR32_LEAVE_CRITICAL_REGION();
#elif UC3C
  scif_pll_opt_t opt;

  opt.osc = SCIF_OSC0,     // Sel Osc0 or Osc1
  opt.lockcount = 16,      // lockcount in main clock for the PLL wait lock
  opt.div = 0,
  opt.mul = mul,
  opt.pll_div2 = 1,        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
  opt.pll_wbwdisable = 0,  //pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
  opt.pll_freq = (pll_freq < AVR32_PM_PLL_VCO_RANGE0_MIN_FREQ) ? 1 : 0,        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.


  scif_pll_setup(SCIF_PLL0, &opt); // lockcount in main clock for the PLL wait lock

  /* Enable PLL0 */
  scif_pll_enable(SCIF_PLL0);

  /* Wait for PLL0 locked */
  scif_wait_for_pll_locked(SCIF_PLL0) ;

  pm_set_clk_domain_div(PM_CLK_DOMAIN_0, (pm_divratio_t) 1); // CPU
  pm_set_clk_domain_div(PM_CLK_DOMAIN_1, (pm_divratio_t) 1); // HSB
  pm_set_clk_domain_div(PM_CLK_DOMAIN_3, (pm_divratio_t) 1); // PBB
  pm_set_clk_domain_div(PM_CLK_DOMAIN_2, (pm_divratio_t) 1); // PBA
  pm_set_clk_domain_div(PM_CLK_DOMAIN_4, (pm_divratio_t) 1); // PBC

  // Set Flashc Wait State
  flashc_set_flash_waitstate_and_readmode(12000000);

  // Set the main clock source as being PLL0.
  pm_set_mclk_source(PM_CLK_SRC_PLL0);
#endif
}


int stop_clocks_uc3_extosc0(void)
{
  // TODO
}

#endif
