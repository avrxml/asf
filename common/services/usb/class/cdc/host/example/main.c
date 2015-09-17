/**
 * \file
 *
 * \brief Main functions for USB host mass storage example
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_usb_host.h"
#include <asf.h>

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
#if SAMD21 || SAML21 || SAMDA1
	system_init();
#else
	/* Initialize the synchronous clock system to the default configuration
	   set in conf_clock.h.
	   \note All non-essential peripheral clocks are initially disabled. */
	sysclk_init();
	/* Initialize the resources used by this example to the default
	   configuration set in conf_board.h */
	board_init();
#endif

	/* Initialize interrupts */
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();

	/* Initialize the user interface */
	ui_init();

	/* Start USB host stack */
	uhc_start();

	/* The main loop manages only the power mode
	   because the USB management is done by interrupt */
	while (true) {
		sleepmgr_enter_sleep();
	}
}

/**
 * \mainpage ASF USB host CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB host CDC
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * After loading firmware, connect the board (EVKxx,Xplain,...) to a USB
 * device CDC.
 * The example is a bridge between a USART from the main MCU
 * and the USB host CDC interface.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB host stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/cdc/host/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
