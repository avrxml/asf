/**
 * \file
 *
 * \brief AVR MEGARF MAC Symbol Counter Driver Example 1
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref macsc_group to use the MAC
 * symbol counter as a timer/counter.
 *
 * \section files Main files:
 *  - macsc_megarf.c MAC Symbol Counter driver implementation
 *  - macsc_megarf.h MAC Symbol Counter driver definitions
 *  - macsc_megarf_example1.c example application
 *  - conf_example.h: configuration of the example
 *
 * \section driverinfo MACSC Driver
 * The MACSC driver can be found \ref macsc_group "here".
 *
 * \section deviceinfo Device Info
 * All megaRF devices with an MACSC can be used.
 *
 * \section exampledescription Description of the example
 * The example will configure the MACSC module and back-off slot counter
 * It will use all three MACSC compare interrupts,overflow, and back-off slot
 * counter interrupts.
 *
 * The MACSC and backoff slot counter is setup to generate overflow
 * interrupt(every 19 hours)
 * and back-off slot interrupt every 320 microseconds.
 * Compare channel 3 interrupt is set to occur every one
 * second(CONFIG_MACSC_TIMEOUT_TICK_HZ).
 * Compare Channels 1 and 2 will be setup at 1/4 and 1/2 of the
 * CONFIG_MACSC_TIMEOUT_TICK_HZ period.
 *
 * Each callback interrupts functions are setup to toggle a IO pin.
 *  - OVF_INT_CHK_PIN: Toggles on MACSC overflow interrupt
 *  - CMP1_INT_CHK_PIN: Toggle on Compare 1 interrupt
 *  - CMP2_INT_CHK_PIN: Toggle on Compare 2 interrupt
 *  - CMP3_INT_CHK_PIN: Toggle on Compare 3 interrupt
 *  - BACKOFF_INT_CHK_PIN: Toggle on backoff slot counter interrupt
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <conf_example.h>
#include <string.h>
#include <asf.h>

/**
 * \brief Symbol Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred on
 * Symbol Counter and toggles OVF_INT_CHK_PIN.
 */
static void example_ovf_int_cb(void)
{
	ioport_toggle_pin(OVF_INT_CHK_PIN);

	uint8_t tx_buf[] = "\n\rOverflow interrupt";
	for (uint8_t i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_PORT, tx_buf[i]);
	}
}

/**
 * \brief Symbol Counter Compare 1 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 1 of
 * symbol counter and toggles CMP1_INT_CHK_PIN.
 */
static void example_cmp1_int_cb(void)
{
	ioport_toggle_pin(CMP1_INT_CHK_PIN);

	uint8_t tx_buf[] = "\n\rcmp1 interrupt(1/4th a sec)";
	for (uint8_t i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_PORT, tx_buf[i]);
	}

	if ((macsc_read_count() + (CONFIG_MACSC_TIMEOUT_TICK_HZ / 4)) <
			0xFFFFFFFF) {
		macsc_use_cmp(COMPARE_MODE, macsc_read_count() +
				(CONFIG_MACSC_TIMEOUT_TICK_HZ / 4), MACSC_CC1);
	} else {
		macsc_write_count(0x00000000ul);
		macsc_use_cmp(COMPARE_MODE, macsc_read_count() +
				(CONFIG_MACSC_TIMEOUT_TICK_HZ / 4), MACSC_CC1);
	}
}

/**
 * \brief Symbol CounterCompare 2 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 2 has
 * occurred
 * Symbol Counter and toggles CMP2_INT_CHK_PIN.
 */
static void example_cmp2_int_cb(void)
{
	ioport_toggle_pin(CMP2_INT_CHK_PIN);

	uint8_t tx_buf[] = "\n\rcmp2 interrupt(1/2th a sec)";
	for (uint8_t i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_PORT, tx_buf[i]);
	}

	if ((macsc_read_count() + (CONFIG_MACSC_TIMEOUT_TICK_HZ / 2)) <
			0xFFFFFFFF) {
		macsc_use_cmp(COMPARE_MODE, macsc_read_count() +
				(CONFIG_MACSC_TIMEOUT_TICK_HZ / 2), MACSC_CC2);
	} else {
		macsc_write_count(0x00000000ul);
		macsc_use_cmp(COMPARE_MODE, macsc_read_count() +
				(CONFIG_MACSC_TIMEOUT_TICK_HZ / 2), MACSC_CC2);
	}
}

/**
 * \brief Symbol Counter Compare 3 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 3 of
 * symbol counter  and toggles CMP3_INT_CHK_PIN.
 */
static void example_cmp3_int_cb(void)
{
	ioport_toggle_pin(CMP3_INT_CHK_PIN);

	uint8_t tx_buf[] = "\n\rcmp3 interrupt(1 sec)";
	for (uint8_t i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_PORT, tx_buf[i]);
	}

	if ((macsc_read_count() + CONFIG_MACSC_TIMEOUT_TICK_HZ) < 0xFFFFFFFF) {
		macsc_use_cmp(COMPARE_MODE,
				macsc_read_count() + CONFIG_MACSC_TIMEOUT_TICK_HZ,
				MACSC_CC3);
	} else {
		macsc_write_count(0x00000000ul);
		macsc_use_cmp(COMPARE_MODE,
				macsc_read_count() + CONFIG_MACSC_TIMEOUT_TICK_HZ,
				MACSC_CC3);
	}
}

/**
 * \brief Backoff slot counter interrupt callback function
 *
 * This function is called when a backoff slot counter interrupt has occured
 * and toggles BACKOFF_INT_CHK_PIN.
 */
static void example_backoff_slot_cntr_int_cb(void)
{
	ioport_toggle_pin(BACKOFF_INT_CHK_PIN);
}

int main(void)
{
	board_init();
	sysclk_init();
	ioport_init();

	sysclk_enable_peripheral_clock(&TRX_CTRL_0);

	cpu_irq_enable();

	/* USART options. */
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};

	/* Initialize usart driver in RS232 mode */
	usart_init_rs232(USART_SERIAL_PORT, &USART_SERIAL_OPTIONS);

	/* configure port pins*/
	ioport_configure_pin(OVF_INT_CHK_PIN,
			IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	ioport_configure_pin(CMP1_INT_CHK_PIN,
			IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	ioport_configure_pin(CMP2_INT_CHK_PIN,
			IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	ioport_configure_pin(CMP3_INT_CHK_PIN,
			IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	ioport_configure_pin(BACKOFF_INT_CHK_PIN,
			IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);

	uint8_t tx_buf[] = "\n\rStarting MAC symbol counter";
	for (uint8_t i = 0; i < sizeof(tx_buf); i++) {
		usart_putchar(USART_SERIAL_PORT, tx_buf[i]);
	}

	/*
	 * Enable Symbol Counter and back-off slot counter
	 */
	macsc_enable();
	macsc_write_clock_source(SYS_CLK_SRC);

	/*
	 * Configure interrupts callback functions
	 * overflow interrupt, compare 1,2,3 and back-off slot cntr interrupts
	 */
	macsc_set_ovf_int_cb(example_ovf_int_cb);
	macsc_set_cmp1_int_cb(example_cmp1_int_cb);
	macsc_set_cmp2_int_cb(example_cmp2_int_cb);
	macsc_set_cmp3_int_cb(example_cmp3_int_cb);
	macsc_set_backoff_slot_cntr_int_cb(example_backoff_slot_cntr_int_cb);

	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute,set compare value.
	 */
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_enable_cmp_int(MACSC_CC3);
	macsc_enable_overflow_interrupt();
	macsc_backoff_slot_cnt_enable();

	macsc_use_cmp(COMPARE_MODE, CONFIG_MACSC_TIMEOUT_TICK_HZ / 4,
			MACSC_CC1);
	macsc_use_cmp(COMPARE_MODE, CONFIG_MACSC_TIMEOUT_TICK_HZ / 2,
			MACSC_CC2);
	macsc_use_cmp(COMPARE_MODE, CONFIG_MACSC_TIMEOUT_TICK_HZ, MACSC_CC3);

	do {
	} while (1);
}
