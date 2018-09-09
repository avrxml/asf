/*****************************************************************************
 *
 * \file
 *
 * \brief Use the RC120M internal oscillator as main clock source and Generic clock configurations.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
 * - how to configure the RC120M oscillator and use it as the main clock source for the synchronous clocks,
 * - configure a Generic Clock and output it to a GPIO pin,
 * - LED0 blinks endlessly.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices can be used. This example has been tested with the following setups:<BR>
 * <ul>
 *     <li>AT32UC3L-EK evaluation kit</li>
 *     <li>STK600 starter kit with the RCUC3D routing card</ul>
 * </ul>
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> 30 MHz </i>
 * - On the AT32UC3L-EK board, check the GCLK_1_0 pin with an oscilloscope, the
 * frequency should be 30MHz. GCLK_1_0 is GPIO pin 6/pin PA06/pin#10 on a UC3L
 * - On the \b STK600 + \b RCUC3D setup , check the GCLK_0 pin with an oscilloscope,
 * the frequency should be 30MHz. GCLK_0 is GPIO pin 3/pin PA03.
 * QFP48 package;
 * - Check that LED0 blinks endlessly.
 *
 * \section contactinfo Contact Info
 * For more info about Atmel AVR32 visit<BR>
 * <A href="http://www.microchip.com/design-centers/32-bit /" >Atmel AVR32</A><BR>
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
#endif
#include "board.h"


/*! \name Generic Clock Configuration
 */
//! @{
#if BOARD == STK600_RCUC3L0 || BOARD == UC3L_EK
#  define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG
#  define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN
#  define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#  define EXAMPLE_CPUCLK_HZ           (30000000UL)
#  define AVR32_SCIF_GC_DIV_CLOCK     AVR32_GC_DIV_CLOCK
// Note that GCLK_1_0 is GPIO pin 6/pin pa06/pin#10 on a UC3L QFP48 package; with
// the RCUC3L0 routing card, this pin is mapped on STK600.PORTA.PA6.
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


/* \brief Start RC120M, switch main clock to RC120M.
 */
static void local_set_main_clock_to_rc120m(void)
{
  // Start the 120MHz internal RCosc (RC120M) clock
  scif_start_rc120M();

#if UC3D
  // Set one wait-state (WS) for flash controller. 0 WS access is up to 30MHz for HSB/CPU clock.
  // As we want to have 48MHz on HSB/CPU clock, we need to set 1 WS on flash controller.
   flashcdw_set_wait_state(1);
#else
  // Since our target is to set the CPU&HSB frequency domains to 30MHz, set the
  // appropriate wait-state and speed read mode on the flash controller.
  flashcdw_set_flash_waitstate_and_readmode(EXAMPLE_CPUCLK_HZ);
#endif

  // Set the CPU clock domain to 30MHz (by applying a division ratio = 4).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_CPU, PM_CKSEL_DIVRATIO_4);

  // Set the PBA clock domain to 30MHz (by applying a division ratio = 4).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBA, PM_CKSEL_DIVRATIO_4);

  // Set the PBB clock domain to 30MHz (by applying a division ratio = 4).
  pm_set_clk_domain_div((pm_clk_domain_t)AVR32_PM_CLK_GRP_PBB, PM_CKSEL_DIVRATIO_4);

  // Set the main clock source to be the RC120M.
  pm_set_mclk_source(PM_CLK_SRC_RC120M);
}


/* \brief Set-up a generic clock to run from RC120M and output it to a gpio pin.
 *
 */
static void local_start_gc(void)
{
  // Note: for UC3L devices, the generic clock configurations are handled by the
  // SCIF module.
  // Setup gc to use RC120M as source clock, divisor enabled, apply a division factor.
  // Since the RC120M frequency is 120MHz, set the division factor to 4 to have a
  // gclk frequency of 30MHz.
  scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_RC120M, AVR32_SCIF_GC_DIV_CLOCK, 4);

  // Now enable the generic clock
  scif_gc_enable(EXAMPLE_GCLK_ID);

  // Set the GCLOCK function to the GPIO pin
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
 * - start the RC120M RC Osc
 * - switch the main clock to RC120M/4
 * - set-up a generic clock with the RC120M RC Osc as a source
 * - output a generic clock to :
 *     - GCLK_1_0(STK600_RCUC3L0 & AT32UC3L-EK)
 *     - GCLK_0 (STK600_RCUC3D)
 */
int main(void)
{
  // Start RC120M, switch main clock to RC120M/4.
  local_set_main_clock_to_rc120m();

  /* Set-up a generic clock from a high frequency clock and output it to a gpio pin. */
  local_start_gc();

  /* Now toggle LED0 using a GPIO */
  while(1)
  {
    gpio_tgl_gpio_pin(LED0_GPIO);
    software_delay();
  }
}
