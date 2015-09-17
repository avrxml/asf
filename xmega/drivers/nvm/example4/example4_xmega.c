/**
 * \file
 *
 * \brief NVM example for AVR XMEGA
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
#include <conf_example.h>
#include <asf.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows how to use the
 * \ref nvm_fuse_lock_group "NVM fuse and lock bits handling" of the
 * \ref nvm_group.
 *
 * \section files Main Files
 * - example4_xmega.c example application
 * - conf_example.h example configuration
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used. This example have
 * been tested with the following setup:
 * - Xplain * - XMEGA A1 Xplained
 * - XMEGA A3BU Xplained
 *
 * \section description Description of the example
 *
 * The example uses the \ref nvm_fuse_lock_group
 * "NVM fuse and lock bits driver" to read a fuse (FUSEBYTE2).
 * The BOOTRST bit (Boot Loader Section Reset Vector) in this fuse is then
 * checked, and if it is set to "Reset Vector = Application Reset" LED1 is
 * toggled. If it isn't set, the LED will not toggle. The user can set the
 * reset vector to Application Reset with an appropriate fuse programmer.
 *
 * This example also demonstrates how to write different lock bits. It is
 * recommended to perform a chip erase after running this example to make
 * sure all lock bits are cleared.
 *
 * LED 0 is toggled when the execution of this example is starting.
 * LED 2 is toggled to indicate the reset vector is pointing to application flash section.
 * For the Xplain, LED 7 is toggled when the execution of this example has ended.
 * For the A3BU Xplained, the status LED is toggled when the execution has ended.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref gpio_group for LED output.
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

int main(void)
{
	uint8_t fuse_val;

	board_init();

#if (BOARD == XMEGA_A3BU_XPLAINED)
	/* We will use the status led to show that the example has
	* completed, so turn it off initially. */
	ioport_set_pin_high(LED3_GPIO);
#endif

	/* Indicate we have started */
	gpio_toggle_pin(LED_PIN_0);

	/* Read FUSEBYTE2 */
	fuse_val = nvm_fuses_read(FUSEBYTE2);

	/* Check if reset vector is pointing to application flash section */
	if ((fuse_val & NVM_FUSES_BOOTRST_bm) == BOOTRST_APPLICATION_gc) {
		/* It is, indicate by toggling LED */
		gpio_toggle_pin(LED_PIN_1);
	}

	/* Set boot section lock bits to "read and write not allowed" */
	nvm_blbb_lock_bits_write(NVM_BLBB_RWLOCK_gc);

	/* Set application section lock bits to "read not allowed" */
	nvm_blba_lock_bits_write(NVM_BLBA_RLOCK_gc);

	/* Set application table section lock bits to "no lock" */
	nvm_blbat_lock_bits_write(NVM_BLBAT_NOLOCK_gc);

	/* Set global lock bits to "write not allowed" */
	nvm_lb_lock_bits_write(NVM_LB_WLOCK_gc);

	/* Do all of the above in one operation */
	nvm_lock_bits_write( NVM_BLBB_RWLOCK_gc, NVM_BLBA_RLOCK_gc,
		NVM_BLBAT_NOLOCK_gc, NVM_LB_WLOCK_gc);

	/* Toggle LED to indicate we are done */
#if (BOARD == XMEGA_A3BU_XPLAINED)
	gpio_toggle_pin(LED_PIN_3);
#else
	gpio_toggle_pin(LED_PIN_7);
#endif
}
