/*****************************************************************************
 *
 * \file
 *
 * \brief High frequency Main clock source and Generic clock configurations.
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
 * typedefs for the power manager driver. <BR>It also gives an example of the usage of the
 * PM on UC3 products.
 * <BR>This example shows:
 * - how to configure a PLL (or a DFLL, depending on the device) and use it as the main clock source for the synchronous clocks,
 * - configure a Generic Clock and output it to a GPIO pin,
 * - LED0 blinks endlessly.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices can be used. This example has been tested with the following setups:<BR>
 * <ul><li>EVK1100 evaluation kit
 * <li>EVK1105 evaluation kit
 * <li>EVK1101 evaluation kit
 * <li>EVK1104 evaluation kit
 * <li>STK600 starter kit with the RCUC3L0 routing card
 * <li>AT32UC3L-EK evaluation kit
 * <li>AT32UC3C-EK evaluation kit
 * <li>STK600 starter kit with the RCUC3D routing card</ul>
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> 48 MHz </i>
 * - On \b EVK1100, check GCLK0 pin with an oscilloscope, the frequency should be 48MHz.
 * On EVK1100, GCLK_0 is pin number 51 (PB19) with AT32UC3A0512 in QFP144 package.
 * Since LED5 is also on PB19, you should also see LED5 turn red.
 * - On \b EVK1105, check GCLK2 pin with an oscilloscope, the frequency should be 48MHz.
 * On EVK1100, GCLK_2 is pin number 62 (PB30) with AT32UC3A0512 in QFP144 package.
 * This pin is accessible on J16.6 on EVK1105 revB or on J17.5 on EVK1105 revC.
 * - On \b EVK1101, check GCLK2 pin with an oscilloscope, the frequency should be 48MHz.
 * On EVK1101, GCLK_2 is pin number 30 (PA30) with AT32UC3B0256 in QFP64 package.
 * - On \b EVK1104, check GCLK1 pin with an oscilloscope, the frequency should be 48MHz.
 * On EVK1104, GCLK_1 is pin number 43 (PB11) with AT32UC3A3256 in QFP144 package.
 * - On the \b STK600 + \b RCUC3L0 setup or on the AT32UC3L-EK board, check
 * the GCLK_1_0 pin with an oscilloscope, the frequency should be 48MHz.
 * GCLK_1_0 is GPIO pin 6/pin pa06/pin#10 on a UC3L QFP48 package; for the RCUC3L0
 * routing card, this pin is mapped on STK600.PORTA.PA6; for the AT32UC3L-EK board,
 * place the oscilloscope probe on connector PA6.
 * - On \b UC3C_EK, check GCLK0 pin with an oscilloscope, the frequency should be 48MHz.
 * On UC3C_EK, GCLK_0 is pin number 54 (PB22) with AT32UC3C0512C in QFP144 package.
 * - On the \b STK600 + \b RCUC3D setup , check the GCLK_0 pin with an oscilloscope,
 * the frequency should be 36MHz. GCLK_0 is GPIO pin 3/pin pa03.
 * For the RCUC3D routing card, this pin is mapped on STK600.PORTA.PA3.
 * - Check that LED0 blinks (whatever the board).
 *
 * \section contactinfo Contact Info
 * For more info about Atmel AVR32 visit<BR>
 * <A href="http://www.atmel.com/products/AVR32/" >Atmel AVR32</A><BR>
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "gpio.h"

#if UC3L
// Note: for UC3L devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "scif_uc3l.h"
#include "pm_uc3l.h"
#include "flashcdw.h"
#elif UC3C
// Note: for UC3C devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "flashc.h"
#elif UC3D
// Note: for UC3D devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "pm_uc3d.h"
#include "scif_uc3d.h"
#include "flashcdw.h"

#else
#include "pm.h"
#include "flashc.h"
#endif

/*! \name Generic Clock Configuration
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_GCLK_ID             0
#  define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_0_1_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_0_1_FUNCTION
// Note that gclk0_1 is pin 51 pb19 on AT32UC3A0512 QFP144.
#elif BOARD == EVK1105
#  define EXAMPLE_GCLK_ID             2
#  define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_2_1_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_2_1_FUNCTION
// Note that gclk2_1 is pin 62 pb30 on AT32UC3A0512 QFP144. It is accessible on
// J16.6 on EVK1105 revB or on J17.5 on EVK1105 revC.
#elif BOARD == EVK1101
#  define EXAMPLE_GCLK_ID             2
#  define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_2_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_2_FUNCTION
// Note that gclk_2 is pin 30 pa30 on AT32UC3B0256 QFP64.
#elif BOARD == EVK1104
#  define EXAMPLE_GCLK_ID             1
#  define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_1_0_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_1_0_FUNCTION
// Note that gclk_1 is pin 43 pb11 on AT32UC3A3256 QFP144.
#elif BOARD == STK600_RCUC3L0 || BOARD == UC3L_EK
#  define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG
#  define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#  define EXAMPLE_CPUCLK_HZ           (48000000UL)
// Note that GCLK_1_0 is GPIO pin 6/pin pa06/pin#10 on a UC3L QFP48 package; with
// the RCUC3L0 routing card, this pin is mapped on STK600.PORTA.PA6.
#  define EXAMPLE_DFLL_COARSE_FDFLL96 150
#  define EXAMPLE_DFLL_FINE_FDFLL96   65
#elif BOARD == UC3C_EK
#  define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_GCLK9
#  define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_0_2_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_0_2_FUNCTION
// Note that gclk_0 is pin 54 pb22 on AT32UC3C0512C QFP144.
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_GCLK0
#  define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_0_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_0_FUNCTION
// Note that GCLK_0 is GPIO pin 3/pin PA03; with
// the RCUC3D routing card, this pin is mapped on STK600.PORTA.PA3.
#endif

#if !defined(EXAMPLE_GCLK_ID)  || \
    !defined(EXAMPLE_GCLK_PIN) || \
    !defined(EXAMPLE_GCLK_FUNCTION)
#  error The generic clock configuration to use in this example is missing.
#endif
//! @}


/* \brief Start PLL0, switch main clock to PLL0 output.
 *
 *
 */
static void local_start_highfreq_clock(void)
{
#if BOARD == STK600_RCUC3L0 || BOARD == UC3L_EK
  scif_dfll_openloop_conf_t dfllconfig = {EXAMPLE_DFLL_FINE_FDFLL96, EXAMPLE_DFLL_COARSE_FDFLL96};

  // Configure and start the DFLL0 in open loop mode to generate a frequency of 96MHz.
  scif_dfll0_openloop_start(&dfllconfig);

  // Since our target is to set the CPU&HSB frequency domains to 48MHz, we must
  // set one wait-state and enable the High-speed read mode on the flash controller.
  flashcdw_set_flash_waitstate_and_readmode(EXAMPLE_CPUCLK_HZ);

  // Set the CPU clock domain to 48MHz (by applying a division ratio = 2).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_CPU, PM_CKSEL_DIVRATIO_2);

  // Set the PBA clock domain to 24MHz (by applying a division ratio = 4).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBA, PM_CKSEL_DIVRATIO_4);

  // Set the PBB clock domain to 48MHz (by applying a division ratio = 2).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBB, PM_CKSEL_DIVRATIO_2);

  // Set the main clock source to be DFLL0.
  pm_set_mclk_source(PM_CLK_SRC_DFLL0);
#elif BOARD == UC3C_EK || BOARD == STK600_RCUC3D

  scif_pll_opt_t opt;

  // Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
  scif_configure_osc_crystalmode(SCIF_OSC0, FOSC0);

  // Enable the OSC0
  scif_enable_osc(SCIF_OSC0, OSC0_STARTUP, true);

  // Set the main clock source as being OSC0.
  pm_set_mclk_source(PM_CLK_SRC_OSC0);

  opt.osc = SCIF_OSC0;     // Sel Osc0 or Osc1
  opt.lockcount = 16;      // lockcount in main clock for the PLL wait lock
  opt.div = 1;             // DIV=1 in the formula
  opt.mul = 5;             // MUL=6 in the formula
  opt.pll_div2 = 1;        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
  opt.pll_wbwdisable = 0;  //pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
  opt.pll_freq = 1;        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.

  scif_pll_setup(SCIF_PLL0, &opt); // lockcount in main clock for the PLL wait lock

  /* Enable PLL0 */
  scif_pll_enable(SCIF_PLL0);

  /* Wait for PLL0 locked */
  scif_wait_for_pll_locked(SCIF_PLL0) ;

  /* Divide PBA clock by 2 from main clock (PBA clock = 48MHz/2 = 24MHz).
     Peripheral Bus A clock divisor enable = 1
     Peripheral Bus A select = 0
     Peripheral Bus B clock divisor enable = 0
     Peripheral Bus B select = 0
     High Speed Bus clock divisor enable = 0
     High Speed Bus select = 0
  */
  pm_set_clk_domain_div(PM_CLK_DOMAIN_2, (pm_divratio_t) 0); // PBA

  // Set one wait-state (WS) for flash controller. 0 WS access is up to 30MHz for HSB/CPU clock.
  // As we want to have 48MHz on HSB/CPU clock, we need to set 1 WS on flash controller.
#if UC3D
  flashcdw_set_wait_state(1);
#else
  flashc_set_wait_state(1);
#endif

  // Set the main clock source as being PLL0.
  pm_set_mclk_source(PM_CLK_SRC_PLL0);

#else
  volatile avr32_pm_t* pm = &AVR32_PM;
  /* \note All calculations here suppose that the Osc0 frequency is 12MHz. */

  pm_switch_to_osc0(pm, FOSC0, OSC0_STARTUP);  // Switch main clock to Osc0.

  /* Setup PLL0 on Osc0, mul=7 ,no divisor, lockcount=16, ie. 12Mhzx8 = 96MHz output */
  /*void pm_pll_setup(volatile avr32_pm_t* pm,
                  unsigned int pll,
                  unsigned int mul,
                  unsigned int div,
                  unsigned int osc,
                  unsigned int lockcount) {
   */
  pm_pll_setup(pm,
               0,   // use PLL0
               7,   // MUL=7 in the formula
               1,   // DIV=1 in the formula
               0,   // Sel Osc0/PLL0 or Osc1/PLL1
               16); // lockcount in main clock for the PLL wait lock

  /*
   This function will set a PLL option.
   *pm Base address of the Power Manager (i.e. &AVR32_PM)
   pll PLL number 0
   pll_freq Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
   pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
   pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
  */
  /* PLL output VCO frequency is 96MHz. We divide it by 2 with the pll_div2=1. This enable to get later main clock to 48MHz */
  pm_pll_set_option(pm, 0, 1, 1, 0);

  /* Enable PLL0 */
  /*
    void pm_pll_enable(volatile avr32_pm_t* pm,
                  unsigned int pll) {
  */
  pm_pll_enable(pm,0);

  /* Wait for PLL0 locked */
  pm_wait_for_pll0_locked(pm) ;

  /* Divide PBA clock by 2 from main clock (PBA clock = 48MHz/2 = 24MHz).
     Peripheral Bus A clock divisor enable = 1
     Peripheral Bus A select = 0
     Peripheral Bus B clock divisor enable = 0
     Peripheral Bus B select = 0
     High Speed Bus clock divisor enable = 0
     High Speed Bus select = 0
  */
  pm_cksel(pm, 1, 0, 0, 0, 0, 0);

  // Set one wait-state (WS) for flash controller. 0 WS access is up to 30MHz for HSB/CPU clock.
  // As we want to have 48MHz on HSB/CPU clock, we need to set 1 WS on flash controller.
  flashc_set_wait_state(1);

  pm_switch_to_clock(pm, AVR32_PM_MCSEL_PLL0); /* Switch main clock to 48MHz */
#endif
}


/* \brief Set-up a generic clock to run from a high-frequency clock and output it to a gpio pin.
 *
 */
static void local_start_gc(void)
{
#if BOARD == STK600_RCUC3L0 || BOARD == UC3L_EK
  // Note: for UC3L devices, the generic clock configurations are handled by the
  // SCIF module.
  // Setup gc to use the DFLL0 as source clock, divisor enabled, apply a division factor.
  // Since the DFLL0 frequency is 96MHz, set the division factor to 2 to have a
  // gclk frequency of 48MHz.
  scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_DFLL0, AVR32_GC_DIV_CLOCK, 2);

  /* Now enable the generic clock */
  scif_gc_enable(EXAMPLE_GCLK_ID);
#elif BOARD == UC3C_EK || BOARD == STK600_RCUC3D
  // Note: for UC3 C, D and L series, the generic clock configurations are handled by the
  // SCIF module.
  /* setup gc on Osc0, no divisor */
  scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_PLL0, AVR32_SCIF_GC_NO_DIV_CLOCK, 0);

  /* Now enable the generic clock */
  scif_gc_enable(EXAMPLE_GCLK_ID);
#else
  volatile avr32_pm_t* pm = &AVR32_PM;
  /* Setup generic clock on PLL0, with Osc0/PLL0, no divisor */
  /*
  void pm_gc_setup(volatile avr32_pm_t* pm,
                  unsigned int gc,
                  unsigned int osc_or_pll, // Use Osc (=0) or PLL (=1)
                  unsigned int pll_osc, // Sel Osc0/PLL0 or Osc1/PLL1
                  unsigned int diven,
                  unsigned int div) {
  */
  pm_gc_setup(pm,
              EXAMPLE_GCLK_ID,
              1,  // Use Osc (=0) or PLL (=1), here PLL
              0,  // Sel Osc0/PLL0 or Osc1/PLL1
              0,  // disable divisor
              0); // no divisor

  /* Enable Generic clock */
  pm_gc_enable(pm, EXAMPLE_GCLK_ID);
#endif
  /* Set the GCLOCK function to the GPIO pin */
  gpio_enable_module_pin(EXAMPLE_GCLK_PIN, EXAMPLE_GCLK_FUNCTION);
}

/* \brief Software Delay
 *
 */
static void software_delay(void)
{
  volatile int i;
  for (i=0; i<1000000; i++);
}


/* \brief This is an example that shows how to do the following:
 * - start a high frequency clock
 * - switch the main clock to that high frequency clock
 * - set-up a generic clock with a high frequency clock as a source
 * - output a generic clock to GCLK_0_1(EVK1100) / GCLK_2_1 / GCLK_2(EVK1101) /
 *  GCLK_1_0(EVK1104) / GCLK_0_2(UC3C_EK) / GCLK_1_0(STK600_RCUC3L0 & AT32UC3L-EK)
 *
 */
int main(void)
{
  /* Start a high frequency clock and switch the main clock to that high frequency clock */
  local_start_highfreq_clock();

  /* Set-up a generic clock from a high frequency clock and output it to a gpio pin. */
  local_start_gc();

  /* Now toggle LED0 using a GPIO */
  while(1)
  {
    gpio_tgl_gpio_pin(LED0_GPIO);
    software_delay();
  }
}
