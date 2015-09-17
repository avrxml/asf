/**
 * \file
 *
 * \brief Main functions
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
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
#include "ieee11073_skeleton.h"

static volatile bool main_b_phdc_enable = false;

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Initialize the sleep manager */
	sleepmgr_init();
#if !SAM0
	sysclk_init();
	board_init();
#else
	system_init();
#endif
	ui_init();
	ui_powerdown();

	/* Start USB stack to authorize VBus monitoring */
	udc_start();

	/* The main loop manages only the power mode
	 * because the USB management is done by interrupt
	 */
	while (true) {
		sleepmgr_enter_sleep();
		if (main_b_phdc_enable) {
			if (ieee11073_skeleton_process()) {
				ui_association(true); /* Association Ok */
			} else {
				ui_association(false); /* No association */
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
	if (!main_b_phdc_enable) {
		return;
	}

	ui_process(udd_get_frame_number());
}

bool main_phdc_enable(void)
{
	main_b_phdc_enable = ieee11073_skeleton_enable();
	return main_b_phdc_enable;
}

void main_phdc_disable(void)
{
	main_b_phdc_enable = false;
	ieee11073_skeleton_disable();
}

/**
 * \mainpage ASF USB Device PHDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device PHDC
 * on Atmel MCU with USB module.
 * This example uses a IEEEE11073 stack skeleton which has the minimum to
 * pass USB Command Verifier 2.0 Tool (CV2.0) tests from usb.org.
 * Note: No USB driver is provided by Atmel for PHDC, but it is not required
 * to execute CV2.0 PHDC tests.
 *
 * \section startup Startup
 * You must compile, load and start example.
 * After, you can start CV2.0 and execute Chapter 9 and PHDC tests.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/phdc/device/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - ieee11073_skeleton.c, IEEEE11073 stack skeleton
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
