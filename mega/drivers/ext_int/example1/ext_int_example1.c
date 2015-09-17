/**
 * \file
 *
 * \brief External Interrupt example for megaRF device
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 * This simple example shows how to use the \ref megarf_interrupt_group for
 * external interrupt
 *
 * \section files Main files:
 * - ext_int_example1.c: This example application
 * - conf_interrupt.h: configuration of the megaRF driver
 * - conf_example.h example definitions
 *
 * \section driverinfo Interrupt Driver
 * The interrupt driver can be found \ref megarf_interrupt_group "here".
 *
 * \section deviceinfo Device Info
 * AVR MEGARF device can be used. This example has been
 * tested with the following setup:
 *  - STK600 evaluation Kit (Tested with STK600-ATMEGA128RFA1)
 *  - ATmega256RFR2 Xplained Pro kit
 *
 * \section setup Setup
 * user defined External interrupt INTx pin is enabled
 * The on board switch connected to the external interrupt pin 
 * is used in this example
 *
 * \section example description Description of the example
 * when on board switch is pressed, interrupt INTx will be triggered
 * when switch is released and LED on the board will blinks once.
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

#include <asf.h>
#include <conf_example.h>
#include <conf_interrupt.h>
/**
 *  \brief delay routine for LED to be ON for some time
 *  \param none
 */

static inline void ext_int_delay(void)
{
	for (volatile unsigned int i = 0; i < 10000; i++) {
	}
}

/**
 * \brief Interrupt callback for External interrupt
 *  Switch ON LED on the board for some time and then switches OFF
 *  when the interrupt is triggered
 *  \param none
 */
static void ext_int_callback(void)
{
	/* ! Switch ON LED1 */
	LED_On(LED_PIN);

	ext_int_delay();

	/* Switch OFF LED1 */
	LED_Off(LED_PIN);
}


/** \brief Main function.
 */
int main(void)
{
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	sysclk_init();
	board_init();

	/* Disable Global interrupt */
	cpu_irq_disable();

    /* Set Interrupt CallBack Function **/
    ext_int_set_interrupt_callback(BUTTON_INTERRUPT_SOURCE,ext_int_callback);
	
	/* Enable the Ext Int */
	ext_int_init(BUTTON_INTERRUPT_SOURCE, BUTTON_INTERRUPT_MODE);

	/* Enable Global interrupt */
	cpu_irq_enable();

	/* Infinite loop and waiting for the external interrupt to occur */
	while (1) {
	}
}
