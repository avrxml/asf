/*****************************************************************************
 *
 * \file
 *
 * \brief PDCA and interrupt control example.
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
 ******************************************************************************/

/** \mainpage
 * \section intro Introduction
 * The Peripheral DMA controller (PDCA) transfers data between on-chip
 * peripheral modules such as USART, SPI, SSC and on- and off-chip memories.
 * Using the PDCA avoids CPU intervention for data transfers, improving the
 * performance of the microcontroller. The PDCA can transfer data from memory to
 * a peripheral or from a peripheral to memory.
 *
 * The given example is a transfer of an ASCII animation stored in internal
 * flash to a USART output. The ASCII animation is divided in two files
 * (ascii_anim1.h and ascii_anim2.h).
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section files Main Files
 * - pdca_example.c: PDCA application example
 * - ascii_anim1.h: First part of the animation string definitions
 * - ascii_anim1.h: Second part of the animation string definitions
 * - conf_example.h: Board specific example hardware definitions
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, UC3C_EK or EVK1104 or EVK1105 evaluation kit
 * - STK600+RCUC3L routing card
 * - STK600+RCUC3D routing card
 *
 * On EVK1100 or EVK1101 or STK600+RCUC3D: \n
 * Connect USART1 to a PC serial port via a standard RS232 DB9 cable.
 *
 * On UC3C_EK: \n
 * Connect to the PC via the virtual CDC class USB serial port.
 *
 * On STK600+RCUC3L connect STK600.PE2 to STK600.RS232 SPARE.TXD and
 * STK600.PE3 to STK600.RS232 SPARE.RXD, and connect the STK600 to a PC serial
 * port via a standard RS232 DB9 cable.
 *
 * On STK600+RCUC3D connect STK600.PC7 to STK600.RS232 SPARE.TXD and
 * STK600.PD0 to STK600.RS232 SPARE.RXD, and connect the STK600 to a PC serial
 * port via a standard RS232 DB9 cable.
 *
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

#include <asf.h>
#include "conf_example.h"

/** The channel instance for the USART example, here PDCA channel 0 (highest
 *  priority). */
#define PDCA_CHANNEL_USART_EXAMPLE 0

/** Counter for interrupt test. */
volatile bool animation_high_section = false;

/** String for ASCII animation: this is the first part of the ASCII animation. */
const char ascii_anim1[] =
#    include "ascii_anim1.h"
;

/** String for ASCII animation: this is the second and final part of the ASCII
 *  animation. */
const char ascii_anim2[] =
#    include "ascii_anim2.h"
;

/** \brief The PDCA interrupt handler.
 *
 * The handler reload the PDCA settings with the correct ASCII animation address
 * and size using the reload register. The interrupt will happen each time half
 * of the animation is played.
 */
#if (UC3C || UC3L)
ISR(pdca_int_handler, AVR32_PDCA_IRQ_GROUP0, 0)
#else
ISR(pdca_int_handler, AVR32_PDCA_IRQ_GROUP, 0)
#endif
{
	/* Set PDCA channel reload values with address where data to
	 * load are stored, and size of the data block to load. */
	if (animation_high_section) {
		pdca_reload_channel(PDCA_CHANNEL_USART_EXAMPLE,
				(void *)ascii_anim2, sizeof(ascii_anim2));
	}
	else {
		pdca_reload_channel(PDCA_CHANNEL_USART_EXAMPLE,
				(void *)ascii_anim1, sizeof(ascii_anim1));
	}

	/* Switch to the other half of the animation */
	animation_high_section = !animation_high_section;
}

/** \brief Init interrupt controller and register pdca_int_handler interrupt.
 */
static void pdca_set_irq(void)
{
#if __GNUC__
	INTC_init_interrupts();

	/* Register the compare interrupt handler to the interrupt controller
	 * and enable the compare interrupt. */
	INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_0,
			AVR32_INTC_INT0);
#endif

	/* Enable all interrupt/exception. */
	Enable_global_interrupt();
}

/** \brief The given example is a transfer of an ASCII animation stored in
 * internal flash to the USART_0 output (57600 bps/8 data bits/no parity bit/1
 * stop bit/no flow control)
 *
 * This example uses the USART, GPIO, INTC and PDCA modules.
 */
int main(void)
{
	sysclk_init();
	sleepmgr_init();

	const gpio_map_t usart_gpio_map = {
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	const usart_options_t usart_options = {
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE,
	};

	const pdca_channel_options_t PDCA_OPTIONS = {
		/* Select peripheral - data is transmitted on USART TX line */
		.pid = AVR32_PDCA_PID_USART_TX,
		/* Select size of the transfer */
		.transfer_size = PDCA_TRANSFER_SIZE_BYTE,

		/* Memory address */
		.addr = (void *)ascii_anim1,
		/* Transfer counter */
		.size = sizeof(ascii_anim1),

		/* Next memory address */
		.r_addr = NULL,
		/* Next transfer counter */
		.r_size = 0,
	};

	/* Assign GPIO pins to USART. */
	gpio_enable_module(usart_gpio_map,
			sizeof(usart_gpio_map) / sizeof(usart_gpio_map[0]));

	/* Initialize the USART in RS232 mode. */
	usart_init_rs232(EXAMPLE_USART, &usart_options,
			sysclk_get_peripheral_bus_hz(EXAMPLE_USART));

	usart_write_line(EXAMPLE_USART, "PDCA Example.\r\n");

	/* Initialize the PDCA channel with the requested options. */
	pdca_init_channel(PDCA_CHANNEL_USART_EXAMPLE, &PDCA_OPTIONS);
	
	/* Register PDCA IRQ interrupt. */
	pdca_set_irq();

	/* Enable PDCA interrupt each time the reload counter reaches zero, i.e.
	 * each time half of the ASCII animation (either anim1 or anim2) is
	 * transferred. */
	pdca_enable_interrupt_reload_counter_zero(PDCA_CHANNEL_USART_EXAMPLE);

	/* Enable now the transfer. */
	pdca_enable(PDCA_CHANNEL_USART_EXAMPLE);

	/* While the PDCA is active transferring data from memory to USART, we
	 * may switch the device into IDLE sleep mode. */
	sleepmgr_lock_mode(SLEEPMGR_IDLE);

	while (1) {
		sleepmgr_enter_sleep();
	}
}
