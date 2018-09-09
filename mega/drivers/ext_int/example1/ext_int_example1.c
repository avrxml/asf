/**
 * \file
 *
 * \brief External Interrupt example for megaRF device
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
