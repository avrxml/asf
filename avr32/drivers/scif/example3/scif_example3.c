/*****************************************************************************
 *
 * \file
 *
 * \brief OSC32K and generic clock configuration and sleep mode example.
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
* - configure and start the OSC32K 32kHz oscillator,
* - set-up a generic clock at 32kHz with the OSC32K osc as a source,
* - output the generic clock to a pin.
* - toggle all four leds 10 times, then switch the device into the static sleep
*   mode (while still maintaining the OSC32 32kHz oscillator).
*
* \section compinfo Compilation Info
* This software was written for the GNU GCC for AVR32 and IAR Systems compiler
* for AVR32. Other compilers may or may not work.
*
* \section deviceinfo Device Info
* All AVR32 devices with a PM module and a SCIF module can be used. This example
* has been tested with the following setup:\n
* - AT32UC3L-EK board
*
* \section setupinfo Setup Information
* \n CPU speed: <i>115 kHz</i>\n\n
* On the AT32UC3L-EK setup, the generic clock is output to the GCLK_1_0 pin. This
* is the GPIO pin 6/pin pa06/pin#10 on a UC3L QFP48 package.
* - Check the GCLK1_0 pin with an oscilloscope, it oscillates at the frequency 32kHz
*   as long as the device is not in static sleep mode.
* - Check the XIN32_2 pin with an oscilloscope: it still oscillates at the
*   frequency 32kHz even when the device is in static sleep mode (~500mV peek-to-peek signal).
*
* \section contactinfo Contact Info
* For more info about Atmel AVR32 visit
* <A href="http://www.atmel.com/products/AVR32/" >Atmel AVR32</A>\n
* Support and FAQ: http://www.atmel.com/design-support/
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#if UC3L
#include "scif_uc3l.h"
#elif UC3D
#include "scif_uc3d.h"
#else
#error "Part Not Supported"
#endif
#include "power_clocks_lib.h"


/*! \name Generic Clock Configuration
 */
//! @{
#if BOARD == UC3L_EK
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN   // Connector PA6 on AT32UC3L-EK
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#elif BOARD == STK600_RCUC3D
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_EXT_NUM
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_PIN   // Connector PA6 on AT32UC3D-
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_FUNCTION
#endif

#if !defined(EXAMPLE_GCLK_ID)  || \
    !defined(EXAMPLE_GCLK_PIN) || \
    !defined(EXAMPLE_GCLK_FUNCTION)
#  error The generic clock configuration to use in this example is missing.
#endif

//! @}

/*! \brief Set-up a generic clock at 32kz with the OSC32 as a source, output the
 * generic clock to a pin.
 *
 */
static void local_start_gc(void)
{
  scif_gclk_opt_t gclkOpt = {SCIF_GCCTRL_OSC32K, 0, 0};
  volatile int i;


  if(scif_start_gclk(EXAMPLE_GCLK_ID, &gclkOpt))
  {   // Error
    while(1)
    {
      gpio_tgl_gpio_pin(LED3_GPIO);
      for(i=1000; i; i--);
    }
  }

  // Assign a GPIO to generic clock output
  gpio_enable_module_pin(EXAMPLE_GCLK_PIN, EXAMPLE_GCLK_FUNCTION);
  // Note that gclk1 is GPIO pin 6 pa06 on AT32UC3L064 pin 10 on QFP48.
}



/* \brief This is an example that shows how to do the following:
 * - configure and start the OSC32K 32kHz oscillator,
 * - set-up a generic clock at 32kHz with the OSC32K osc as a source,
 * - output the generic clock to a pin,
 * - toggle all four leds 10 times,
 * - then switch the device into the static sleep mode (while still maintaining
 *   the OSC32 32kHz oscillator).
 *
 */
int main(void)
{
  //! OSC32K config.
  scif_osc32_opt_t  osc32Conf = {
    SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR, // 2-pin Crystal and high current mode. Crystal is connected to XIN32/XOUT32.
    OSC32_STARTUP,                    // oscillator startup time
    true,                             // select the alternate xin32_2 and xout32_2 for the 32kHz crystal oscillator
    false,                            // disable the 1kHz output
    true                              // enable the 32kHz output
    };
  volatile int i,j;

  // - configure and start the OSC32K 32kHz oscillator,
  if(PASS != scif_start_osc32(&osc32Conf, true))
  {   // Error
    while(1)
    {
      gpio_tgl_gpio_pin(LED3_GPIO);
      for(i=1000; i; i--);
    }
  }

  // - set-up a generic clock at 32kHz with the OSC32K osc as a source,
  // - output the generic clock to a pin.
  local_start_gc();

  // - toggle all four leds 10 times,
  for(i=25;i;i--)
  {
    gpio_tgl_gpio_pin(LED0_GPIO); gpio_tgl_gpio_pin(LED1_GPIO);
    gpio_tgl_gpio_pin(LED2_GPIO); gpio_tgl_gpio_pin(LED3_GPIO);
    for(j=1000;j;j--);
  }

  // - then switch the device into the static sleep mode (while still maintaining
  // the OSC32 32kHz oscillator).

  // If there is a chance that any PB write operations are incomplete, the CPU
  // should perform a read operation from any register on the PB bus before
  // executing the sleep instruction.
  AVR32_INTC.ipr[0];  // Dummy read

  SLEEP(AVR32_PM_SMODE_STATIC);
  // Note: in static sleep mode, the GCLK output is not maintained but the OSC32K
  // oscillator is still active (check the XIN32_2 pin (500mv peek-to-peek amplitude)).

  while(1);
}

