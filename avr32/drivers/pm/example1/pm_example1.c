/*****************************************************************************
 *
 * \file
 *
 * \brief Osc0 as Main clock source and Generic clock configuration and sleep mode example.
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
* typedefs for the power manager driver.\n It also gives an example of the usage of the
* PM on UC3 products.
* This example shows how to configure the Power Manager(PM) and the
* System Control Interface(SCIF) (only for devices that include a SCIF module) to
* use Oscillator 0 as source of main clock and configure a generic clock GCLK with
* this OSC0 as input. The generic clock 0 can be then viewed on the GCLK pin (this
* is the oscillator 0 frequency). Once these configurations are done, the example
* goes into sleep mode while still maintaining GCLK output.
*
* \section compinfo Compilation Info
* This software was written for the GNU GCC for AVR32 and IAR Systems compiler
* for AVR32. Other compilers may or may not work.
*
* \section deviceinfo Device Info
* All AVR32 devices with a PM module or AVR32 devices with a PM module and a SCIF
* module can be used. This example has been tested with the following setup:\n
* - EVK1100 evaluation kit
* - EVK1101 evaluation kit
* - EVK1104 evaluation kit
* - STK600 starter kit with the RCUC3L0 routing card
* - STK600 starter kit with the RCUC3D routing card
* - UC3C_EK evaluation kit
*
* \section setupinfo Setup Information
* \n CPU speed: <i>OSC0 MHz</i>\n\n
* On EVK1100, GCLK_0_1 is pin number 51 (PB19) with AT32UC3A0512 in QFP144 package.
* - Check GCLK0 pin with an oscilloscope, this is OSC0 frequency (12MHz).
*
* On EVK1101, GCLK_2 is pin number 30 (PA30) with AT32UCB0256 in QFP64 package.
* - Check GCLK2 pin with an oscilloscope, this is OSC0 frequency (12MHz).
*
* On EVK1104, GCLK_1_0 is pin number 43 (PB11) with AT32UC3A3256 in QFP144 package.
* - Check GCLK1 pin with an oscilloscope, this is OSC0 frequency (12MHz).
*
* On the STK600 + RCUC3L0 setup, insert a 12MHz crystal in the STK600 crystal socket.
* GCLK_1_0 is GPIO pin 6/pin pa06/pin#10 on a UC3L QFP48 package; for the RCUC3L0
* routing card, this pin is mapped on STK600.PORTA.PA6.
* - Check the GCLK1 pin with an oscilloscope, it should oscillate at the crystal frequency (12MHz).
*
* On the STK600 + RCUC3D setup, insert a 12MHz crystal in the STK600 crystal socket.
* GCLK0 is GPIO pin 3/pin pa03/pin#7 on a UC3D QFP64 package; for the RCUC3D routing card,
* this pin is mapped on STK600.PORTA.PA3.
* - Check the GCLK1 pin with an oscilloscope, it should oscillate at the crystal frequency (12MHz).
*
* On UC3C_EK, GCLK_0_2 is pin number 54 (PB22) with AT32UC3C0512C in QFP144 package.
* - Check GCLK1 pin with an oscilloscope, this is OSC0 frequency (16MHz).
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
#if UC3L
// Note: for UC3L devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "scif_uc3l.h"
#include "pm_uc3l.h"
#elif UC3C
// Note: for UC3C devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#elif UC3D
// Note: for UC3D devices, the osc configurations are handled by the SCIF module
// and the synchronous clocks used to clock the main digital logic are handled
// by the PM module.
#include "pm_uc3d.h"
#include "scif_uc3d.h"
#else
#include "pm.h"
#endif


/*! \name Generic Clock Configuration
 */
//! @{
#if BOARD == EVK1100
#define EXAMPLE_GCLK_ID             AVR32_PM_GCLK_GCLK0
#define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_0_1_PIN
#define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_0_1_FUNCTION
#elif BOARD == EVK1101
#define EXAMPLE_GCLK_ID             AVR32_PM_GCLK_GCLK2
#define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_2_PIN
#define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_2_FUNCTION
#elif BOARD == EVK1104
#define EXAMPLE_GCLK_ID             AVR32_PM_GCLK_GCLK1
#define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_1_0_PIN
#define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_1_0_FUNCTION
#elif BOARD == STK600_RCUC3L0
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN // Mapped on STK600.PORTA.PA6
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#elif BOARD == UC3C_EK
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_GCLK9
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_0_2_PIN
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_0_2_FUNCTION
#elif BOARD == STK600_RCUC3D
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_GCLK0
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_0_PIN // Mapped on STK600.PORTA.PA3
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_0_FUNCTION
#elif BOARD == UC3_A3_XPLAINED
#define EXAMPLE_GCLK_ID             AVR32_PM_GCLK_GCLK1
#define EXAMPLE_GCLK_PIN            AVR32_PM_GCLK_1_0_PIN
#define EXAMPLE_GCLK_FUNCTION       AVR32_PM_GCLK_1_0_FUNCTION
#endif

#if !defined(EXAMPLE_GCLK_ID)  || \
    !defined(EXAMPLE_GCLK_PIN) || \
    !defined(EXAMPLE_GCLK_FUNCTION)
#  error The generic clock configuration to use in this example is missing.
#endif
//! @}

/* Enable the specified GCLK output on the nominated pin for the selected board. */
static void local_enable_gclk_on_gpio(volatile avr32_pm_t* pm)
{
#if ( BOARD == STK600_RCUC3L0 ) || ( BOARD == UC3C_EK ) || ( BOARD == STK600_RCUC3D )
	// Note: for UC3L and UC3C devices, the generic clock configurations are handled
	// by the SCIF module.
	/* setup generic clock on Osc0, no divisor */
	scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_OSC0, AVR32_SCIF_GC_NO_DIV_CLOCK, 0);

	/* Now enable the generic clock */
	scif_gc_enable(EXAMPLE_GCLK_ID);
#else
	/* setup generic clock on Osc0, no divisor */
	pm_gc_setup(pm, EXAMPLE_GCLK_ID, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, AVR32_GC_NO_DIV_CLOCK, 0);

	/* Now enable the generic clock */
	pm_gc_enable(pm, EXAMPLE_GCLK_ID);
#endif

	/* Assign a GPIO to generic clock output */
	gpio_enable_module_pin(EXAMPLE_GCLK_PIN, EXAMPLE_GCLK_FUNCTION);

	// Note that gclk0_1 is GPIO pin 51 pb19 on AT32UC3A0512 QFP144.
	// Note that gclk2 is GPIO pin 30 pa30 on AT32UC3B0256 QFP64.
	// Note that gclk1 is GPIO pin 43 pb11 on AT32UC3A3256 QFP144.
	// Note that gclk1 is GPIO pin 6 pa06 on AT32UC3L064 pin 10 on QFP48.
	// Note that gclk0 is GPIO pin 54 pb22 on AT32UC3C0512C QFP144.
	// Note that gclk0 is GPIO pin 9 pa03 on ATUC128D3 QFN64.
}


/* \brief This is an example that shows:
 *  - how to configure OSC0
 *  - how to set the main clock to use OSC0 as input
 *  - how to configure a generic clock GCLK to use OSC0 as input
 *  - how to output a generic clock to a pin (GLCK_0_1 for EVK1100 or GCLK_2 for
 *    EVK1101 or GLCK_1_0 for EVK1104+UC3_A3_XPLAINED or GLCK_1_0 for STK600+RCUC3L0
      or GLCK_0 for STK600+RCUC3D)
 *  - how to go into a sleep mode (while still maintaining GCLK output)
 *
 */
int main(void)
{
	volatile avr32_pm_t* pm = &AVR32_PM;

#if ( BOARD == STK600_RCUC3L0 ) || ( BOARD == UC3C_EK ) || (BOARD == STK600_RCUC3D)
	// Note: for UC3 C, D and L series, the osc configurations are handled by the SCIF module
	// and the synchronous clocks used to clock the main digital logic are handled
	// by the PM module.
	// 1) Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
	scif_configure_osc_crystalmode(SCIF_OSC0, FOSC0);

	// 2) Enable the OSC0
	scif_enable_osc(SCIF_OSC0, OSC0_STARTUP, true);

	// 3) Set the main clock source as being OSC0.
	pm_set_mclk_source(PM_CLK_SRC_OSC0);
#else
	// 1) Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
	pm_enable_osc0_crystal(pm, FOSC0);

	// 2) Enable the OSC0
	pm_enable_clk0(pm, OSC0_STARTUP);

	// 3) Set the main clock source as being OSC0.
	pm_switch_to_clock(pm, AVR32_PM_MCSEL_OSC0);
#endif

	// - Configure a generic clock GCLK to use OSC0 as input
	// - Output that generic clock to a pin
	local_enable_gclk_on_gpio(pm);

	//*** Sleep mode
	// If there is a chance that any PB write operations are incomplete, the CPU
	// should perform a read operation from any register on the PB bus before
	// executing the sleep instruction.
	AVR32_INTC.ipr[0];  // Dummy read

	// - Go into a sleep mode (while still maintaining GCLK output)
	SLEEP(AVR32_PM_SMODE_FROZEN);

	while (true);
}
