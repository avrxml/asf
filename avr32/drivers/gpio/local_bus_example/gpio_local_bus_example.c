/*****************************************************************************
 *
 * \file
 *
 * \brief GPIO example application for AVR32 using the local bus interface.
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
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the GPIO driver.
 *
 * The General Purpose Input/Output manages the I/O pins of the microcontroller. Each I/O line
 * may be dedicated as a general-purpose I/O or be assigned to a function of an embedded peripheral.
 * This assures effective optimization of the pins of a product.
 *
 * The given example covers various uses of the GPIO controller and demonstrates
 * different GPIO functionalities using the local bus interface.
 *
 * This interface operates with high clock frequency (fCPU), and its timing is
 * deterministic since it does not need to access a shared bus which may be
 * heavily loaded.
 *
 * To use this interface, the clock frequency of the peripheral bus on which the
 * GPIO peripheral is connected must be set to the CPU clock frequency
 * (fPBx = fCPU).
 *
 * The example toggles PA10 on each CPU clock cycle. The CPU is set at the
 * maximal frequency at which instructions can be fetched from flash without
 * wait states, i.e. 33 MHz for UC3A & UC3A3 & UC3B & UC3D devices, 25MHz for UC3L devices.
 * Hence, the pin signal frequency is:
 * - for UC3A & UC3A3 & UC3B & UC3D: \f$ \frac{33 MHz}{2} = 16.5 MHz \f$ because the pin is toggled at 33 MHz
 * - for UC3L & UC3L4: \f$ \frac{25 MHz}{2} = 12.5 MHz \f$ because the pin is toggled at 25 MHz
 * This can be observed with an oscilloscope.
 *
 * \section files Main Files
 * - gpio.c: GPIO driver;
 * - gpio.h: GPIO driver header file;
 * - gpio_local_bus_example.c: GPIO example application using the local bus.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a GPIO module can be used. This example has been tested
 * with the following setup:
 *   - EVK1100, EVK1101, EVK1104, EVK1105, AT32UC3C-EK, AT32UC3L-EK evaluation kit;
 *   - STK600+RCUC3L0 or STK600+RCUC3L4 routing card (PA10 is routed to STK600.PORTB.PB2).
 *   - STK600+RCUC3D routing card  (PA10 is routed to STK600.PORTB.PB2).
 *
 * \section setupinfo Setup Information
 * - CPU speed for UC3A & UC3A3 & UC3B: <i> 33 MHz from 12-MHz Osc0 crystal </i>
 * - CPU speed for UC3C: <i> 32 MHz from 16-MHz Osc0 crystal </i>
 * - CPU speed for UC3L & UC3L4: <i> 25 MHz, no external component used </i>
 * - CPU speed for UC3D: <i> 33 MHz from 12-MHz Osc0 crystal </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#if UC3L
// Note: for UC3L devices, the clock configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "power_clocks_lib.h"
#elif UC3C
#include "scif_uc3c.h"
#include "pm_uc3c.h"
#elif UC3D
#include "scif_uc3d.h"
#include "pm_uc3d.h"
#else // default for UC3A and UC3B series
#include "pm.h"
#endif
#include "gpio.h"


/*! \name Pin Configuration
 */
//! @{
#define GPIO_PIN_EXAMPLE  AVR32_PIN_PA10
//! @}


/*! \name Target DFLL0 frequency
 */
//! @{
#if UC3L
#define EXAMPLE_FDFLL_HZ              (100000000ULL)  // 100MHz
#define EXAMPLE_FDFLL_KHZ             (100000UL)
#define EXAMPLE_MCUCLK_HZ             (25000000UL)    // 25MHz
#endif


//! @}


static void clockfrequencies_configure(void)
{
#if UC3L
    static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
    static pcl_freq_param_t pcl_dfll_freq_param =
    {
      .main_clk_src = PCL_MC_DFLL0,
      .cpu_f        = EXAMPLE_MCUCLK_HZ,
      .pba_f        = EXAMPLE_MCUCLK_HZ,
      .pbb_f        = EXAMPLE_MCUCLK_HZ,
      .dfll_f       = EXAMPLE_FDFLL_HZ,
      .pextra_params = &gc_dfllif_ref_opt
    };
// Implementation for UC3L
    // Note: on the AT32UC3L-EK board, there is no crystal/external clock connected
    // to the OSC0 pinout XIN0/XOUT0. We shall then program the DFLL and switch the
    // main clock source to the DFLL.
    pcl_configure_clocks(&pcl_dfll_freq_param);
    // Note: since it is dynamically computing the appropriate field values of the
    // configuration registers from the parameters structure, this function is not
    // optimal in terms of code size. For a code size optimal solution, it is better
    // to create a new function from pcl_configure_clocks_dfll0() and modify it
    // to use preprocessor computation from pre-defined target frequencies.
#elif UC3C
    // Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
    scif_configure_osc_crystalmode(SCIF_OSC0, FOSC0);
    // Enable the OSC0
    scif_enable_osc(SCIF_OSC0, OSC0_STARTUP, true);
    // Set the main clock source as being OSC0.
    pm_set_mclk_source(PM_CLK_SRC_OSC0);

    scif_pll_opt_t opt;

    // Setup PLL0 on Osc0, mul=7 ,no divisor, lockcount=16: (16Mhzx8)/2 = 64MHz output
    opt.osc = SCIF_OSC0;     // Sel Osc0 or Osc1
    opt.lockcount = 16;      // lockcount in main clock for the PLL wait lock
    opt.div = 1;             // DIV=1 in the formula
    opt.mul = 7;             // MUL=7 in the formula
    opt.pll_div2 = 1;        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
    opt.pll_wbwdisable = 0;  //pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
    opt.pll_freq = 1;        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.

    scif_pll_setup(SCIF_PLL0, &opt); // lockcount in main clock for the PLL wait lock

    /* Enable PLL0 */
    scif_pll_enable(SCIF_PLL0);

    /* Wait for PLL0 locked */
    scif_wait_for_pll_locked(SCIF_PLL0) ;

    // Divide PLL0 output by 2 for CPU, HSB and PBx clocks = 32MHz
    pm_set_clk_domain_div(PM_CLK_DOMAIN_0, (pm_divratio_t) 0); // CPU
    pm_set_clk_domain_div(PM_CLK_DOMAIN_1, (pm_divratio_t) 0); // HSB
    pm_set_clk_domain_div(PM_CLK_DOMAIN_3, (pm_divratio_t) 0); // PBB
    pm_set_clk_domain_div(PM_CLK_DOMAIN_2, (pm_divratio_t) 0); // PBA
    pm_set_clk_domain_div(PM_CLK_DOMAIN_4, (pm_divratio_t) 0); // PBC

    /* Set the main clock source as being PLL0. */
    pm_set_mclk_source(PM_CLK_SRC_PLL0);

#elif UC3D
    // Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
    scif_configure_osc_crystalmode(SCIF_OSC0, FOSC0);
    // Enable the OSC0
    scif_enable_osc(SCIF_OSC0, OSC0_STARTUP, true);
    // Set the main clock source as being OSC0.
    pm_set_mclk_source(PM_CLK_SRC_OSC0);

    scif_pll_opt_t opt;

    // Setup PLL0 on Osc0, mul=10 ,no divisor, lockcount=16: (12Mhzx11)/2 = 66MHz output
    opt.osc = SCIF_OSC0;     // Sel Osc0 or Osc1
    opt.lockcount = 16;      // lockcount in main clock for the PLL wait lock
    opt.div = 1;             // DIV=1 in the formula
    opt.mul = 10;            // MUL=10 in the formula
    opt.pll_div2 = 1;        // pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
    opt.pll_wbwdisable = 0;  // pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
    opt.pll_freq = 1;        // Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.

    scif_pll_setup(SCIF_PLL0, &opt); // lockcount in main clock for the PLL wait lock

    /* Enable PLL0 */
    scif_pll_enable(SCIF_PLL0);

    /* Wait for PLL0 locked */
    scif_wait_for_pll_locked(SCIF_PLL0) ;

    // Divide PLL0 output by 2 for CPU, HSB and PBx clocks = 33MHz
    pm_set_clk_domain_div(PM_CLK_DOMAIN_0, (pm_divratio_t) 0); // CPU
    pm_set_clk_domain_div(PM_CLK_DOMAIN_1, (pm_divratio_t) 0); // HSB
    pm_set_clk_domain_div(PM_CLK_DOMAIN_3, (pm_divratio_t) 0); // PBB
    pm_set_clk_domain_div(PM_CLK_DOMAIN_2, (pm_divratio_t) 0); // PBA

    /* Set the main clock source as being PLL0. */
    pm_set_mclk_source(PM_CLK_SRC_PLL0);

#else // UC3A and UC3B series
  // Switch the main clock source to Osc0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Setup PLL0 on Osc0, mul=10 ,no divisor, lockcount=16: 12Mhzx11 = 132MHz output
  pm_pll_setup(&AVR32_PM, 0,  // pll.
               10,  // mul.
               1,   // div.
               0,   // osc.
               16); // lockcount.
  // PLL output VCO frequency is 132MHz.
  // We divide it by 2 with the pll_div2=1 to get a main clock at 66MHz.
  pm_pll_set_option(&AVR32_PM, 0, // pll.
                    1,  // pll_freq.
                    1,  // pll_div2.
                    0); // pll_wbwdisable.
  // Enable the PLL.
  pm_pll_enable(&AVR32_PM, 0);
  // Wait until the PLL output is stable.
  pm_wait_for_pll0_locked(&AVR32_PM);
  // Configure each clock domain to use the main clock divided by 2
  // => fCPU = fPBA = fPBB = 33MHz.
  pm_cksel(&AVR32_PM,
           1,   // pbadiv.
           0,   // pbasel.
           1,   // pbbdiv.
           0,   // pbbsel.
           1,   // hsbdiv=cpudiv
           0);  // hsbsel=cpusel
  // Switch the main clock source to PLL0.
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);
#endif
}

/*! \brief Initializes the HSB bus matrix (use last master as default for the
 *   flash controller)
 */
static void init_hmatrix(void)
{
#if (defined AVR32_HMATRIX)
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg = {AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH]};
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
#endif
#if (defined AVR32_HMATRIXB)
  union
  {
    unsigned long                 scfg;
    avr32_hmatrixb_scfg_t          SCFG;
  } u_avr32_hmatrixb_scfg = {AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_FLASH]};
  u_avr32_hmatrixb_scfg.SCFG.defmstr_type = AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_FLASH] = u_avr32_hmatrixb_scfg.scfg;
#endif
}

/*! \brief This is an example showing how to toggle a GPIO pin at high speed.
 */
int main(void)
{
  // Initialize domain clocks (CPU, HSB, PBA and PBB) to the max frequency available
  // without flash wait states.
  // Some of the registers in the GPIO module are mapped onto the CPU local bus.
  // To ensure maximum transfer speed and cycle determinism, any slaves being
  // addressed by the CPU on the local bus must be able to receive and transmit
  // data on the bus at CPU clock speeds. The consequences of this is that the
  // GPIO module has to run at the CPU clock frequency when local bus transfers
  // are being performed => we want fPBA = fCPU.
  clockfrequencies_configure();

  // initialize hmatrix bus
  init_hmatrix();

  // Enable the local bus interface for GPIO.
  gpio_local_init();

  // Enable the output driver of the example pin.
  // Note that the GPIO mode of pins is enabled by default after reset.
  gpio_local_enable_pin_output_driver(GPIO_PIN_EXAMPLE);

  // Toggle the example GPIO pin at high speed in a loop.
  while (1)
  {
    // Explicit loop unrolling allowing consecutive ST.W instructions without
    // loop overhead if compiler optimization is activated, except every 128
    // ST.W for the while loop.
#define INSERT_GPIO_LOCAL_TGL_GPIO_PIN(idx, pin) \
    gpio_local_tgl_gpio_pin(pin);
    MREPEAT(128, INSERT_GPIO_LOCAL_TGL_GPIO_PIN, GPIO_PIN_EXAMPLE)
#undef INSERT_GPIO_LOCAL_TGL_GPIO_PIN
  }
}
