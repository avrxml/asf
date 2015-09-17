/*****************************************************************************
 *
 * \file
 *
 * \brief Example for resistive touch display measurement.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the resistive touch driver example.
 *
 * This example demonstrates how to use a resistive touch panel with the UC3.
 *
 * \section files Main Files
 * - rtouch_example.c: Resistive touch example.
 * - rtouch_calibrate.h: Calibration API.
 * - rtouch_calibrate.c: Calibration functions.
 * - rtouch.h: Resistive touch driver API.
 * - rtouch.c: Resistive touch driver.
 * - rtouch_conf.h: Resistive touch driver configuration.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1105 (rev3 or higher) or UC3C-EK
 * - CPU clock: 12 MHz (EVK1105), 16MHz (UC3C-EK);
 * - USART abstracted with a USB CDC connection to a PC;port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


// Include Files
#include <avr32/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "intc.h"
#include "cycle_counter.h"
#include "flashc.h"

#include "rtouch.h"
#include "rtouch_calibrate.h"

#include "et024006dhu.h"
#include "conf_et024006dhu.h"

#ifdef MAX_SPEED
#if BOARD == EVK1105 // set max speed as 66MHz for UC3A
#define FCPU_HZ		66000000
#define FPBA_HZ		66000000
#else // else 60MHz is safe for most UC3 devices
#define FCPU_HZ		60000000
#define FPBA_HZ		60000000
#endif
#else
#define FCPU_HZ		FOSC0
#define FPBA_HZ		FOSC0
#endif

#ifdef MAX_SPEED

/*! \name System Clock Frequencies
 */
//! @{
static pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = FCPU_HZ,
  .pba_f        = FPBA_HZ,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};
//! @}

/*! \brief Initializes the MCU system clocks.
*/
static void init_sys_clocks(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    return 42;
}
#endif

/* \brief Initialize board.
 *
 */
void init_board(void)
{

#ifdef MAX_SPEED
	init_sys_clocks();
#else
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif
	INTC_init_interrupts();

	init_dbg_rs232(FPBA_HZ);
	// Activate LED0 & LED1 & LED2 & LED3 pins in GPIO output
	// mode and switch them off.
	gpio_set_gpio_pin(LED0_GPIO);
	gpio_set_gpio_pin(LED1_GPIO);
	gpio_set_gpio_pin(LED2_GPIO);
	gpio_set_gpio_pin(LED3_GPIO);

	et024006_Init(FCPU_HZ, FCPU_HZ);
	gpio_set_gpio_pin(ET024006DHU_BL_PIN);
	et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE);
}

/* \brief Callback for touch events
 *
 * Draw a pixel on the touch location.
 */
void event_handler(rtouch_event_t const *event)
{

	switch(event->type) {
	case RTOUCH_MOVE:
		// draw a pixel
		et024006_DrawPixel(event->panelX, event->panelY, BLACK);
		break;
	case RTOUCH_PRESS:
		// draw pixel
		et024006_DrawPixel(event->panelX, event->panelY, BLACK);
		break;
	default:
		break;
	}
}

/* \brief Main loop.
 *
 * Do calibration and then wait for touch events.
 * Upon a touch or move event we will draw a pixel on that position.
 */
int main(void)
{
	init_board();
	print_dbg("Resistive Touch Example\r\n");

	rtouch_init();
	rtouch_enable();
	rtouch_set_event_handler(event_handler);

	gpio_clr_gpio_pin(LED0_GPIO);

	/* Do the calibration */
	while(rtouch_calibrate())
		;

	/* Signal end of calibration */
	gpio_set_gpio_pin(LED0_GPIO);

	while(true)
	{
	}
}
