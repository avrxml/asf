/**
 * \file
 *
 * \brief Main functions for USB device and host (dual rual) mouse example
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "conf_usb.h"
#include "conf_usb_host.h"
#include "ui.h"

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
#if SAMD21 || SAML21 || SAMDA1
	system_init();
#else
	sysclk_init();
	board_init();
#endif

	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	ui_init();

	// Start USB dual mode which will start the correct mode (device or host)
	// corresponding at USB ID signal.
	uhc_start();

	// The main loop manages only the power mode
	// because the USB stack is full interrupt driven.
	while (true) {
		sleepmgr_enter_sleep();
	}
}

/**
 * \mainpage ASF USB host and device HID mouse
 *
 * \section intro Introduction
 * This example shows how to implement a USB dual mode (device and host)
 * HID Mouse on Atmel MCU with USB module.
 *
 * \section startup Startup
 * In device mode, the example uses the buttons or sensors available
 * on the board to simulate a standard mouse.
 * After loading firmware, connect the board to the USB Host.
 * When connected to a USB host system this application provides
 * a mouse application in the Unix/Mac/Windows operating systems.
 * This example uses the native HID driver for these operating systems.
 *
 * In host mode, after loading firmware, connect the board
 * to a USB device mouse.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB device and host stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/device/mouse/
 *   <br>services/usb/class/hid/host/mouse/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
