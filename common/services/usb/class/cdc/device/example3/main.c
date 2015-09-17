/**
 * \file
 *
 * \brief CDC Application Main functions
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_usb.h"
#include "ui.h"

static volatile bool main_b_cdc_enable = false;
static volatile uint8_t main_port_open;

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	sysclk_init();
	board_init();
	ui_init();
	ui_powerdown();

	// Start USB stack to authorize VBus monitoring
	udc_start();

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {
		sleepmgr_enter_sleep();
		if (main_b_cdc_enable) {
			// Here CPU wakeup at each SOF (1ms)
			for (uint8_t port = 0; port < UDI_CDC_PORT_NB; port++) {
				if (!(main_port_open & (1 << port))) {
					// Port not open
					continue;
				}
				if (!udi_cdc_multi_is_rx_ready(port)) {
					// No data received
					continue;
				}
				int value = udi_cdc_multi_getc(port);
				if (value != 'p') {
					// Ignore this value
					continue;
				}
				udi_cdc_multi_write_buf(port, "PORT", sizeof("PORT")-1);
				udi_cdc_multi_putc(port, port+'0');
				udi_cdc_multi_putc(port, '\n');
				udi_cdc_multi_putc(port, '\r');
			}
		}
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_cdc_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	main_port_open = 0;
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
	if (b_enable) {
		// Host terminal has open COM
		main_port_open |= 1 << port;
		ui_com_open(port);
	}else{
		// Host terminal has close COM
		main_port_open &= ~(1 << port);
		ui_com_close(port);
	}
}

/**
 * \mainpage ASF USB Device CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device CDC
 * on Atmel MCU with USB module.
 * The application note AVR4907 provides more information
 * about this implementation.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example is a implementation of 7 USB CDC interface.
 *
 * In this example, we will use a PC as a USB host:
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as 7 virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on a virtual COM port
 * - Ignore virtual COM port configuration because this does not manage a
 *   true UART
 * - Type the character 'p' in the HyperTerminal and it will display the port
 *   number"PORTX" corresponding at virtual COM port opened.
 *
 * \note
 * On the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this
 *   installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *      <br>
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds,buttons...)
 */
