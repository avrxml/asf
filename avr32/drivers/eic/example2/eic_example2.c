/*****************************************************************************
 *
 * \file
 *
 * \brief External Interrupt Controller Example 2
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the EIC driver. This documentation is also
 * bundled with an example.
 *
 * This example shows how to use an external interrupt and to generate interrupt.\n
 * <b>Operating mode:</b>
 * - For EVK1100:\n
 *    - When the user presses JOYSTICK : the led LED0 switches ON for a short while(software delay) then it switches OFF\n
 * - For EVK1101:\n
 *    - When the user presses PB0 : the led LED0 switch ON for a short while(software delay) then it switches OFF / LED1 switches OFF\n
 *    - When the user presses PB1 : the led LED0 switch OFF / LED1 switches ON for a short while(software delay) then it switches OFF\n
 * - For EVK1104:\n
 *    - When the user presses PA20 : the led LED0 switches ON for a short while(software delay) then it switches OFF
 * - For UC3C_EK:\n
 *    - When the user presses PB1 : the led LED0 switches ON for a short while(software delay) then it switches OFF
 * - For STK600-RCUC3D:\n
 *    - When the user presses SW0 : the led LED0 switches ON for a short while(software delay) then it switches OFF
 *
 * \section files Main Files
 * - eic.c: EIC driver;
 * - eic.h: EIC driver header file;
 * - eic_example2.c: EIC example application.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with a EIC module can be used. This example has been tested
 * with the following boards:
 * - EVK1100 evaluation kit
 * - EVK1101 evaluation kit
 * - EVK1104 evaluation kit
 * - UC3C_EK evaluation kit
 * - STK600-RCUC3D evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> Switch to oscillator external OSC0 = 12 MHz. </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "board.h"
#include "eic.h"
#include "intc.h"
#include "conf_example2.h"


//! Structure holding the configuration parameters of the EIC module.
eic_options_t eic_options[EXT_INT_EXAMPLE_NB_LINES];

//! Global var used to switch between different LED patterns.
unsigned char key_number=0;

//! Global var used to cancel the software delay if an External Interrupt occurs.
static volatile bool bCancelSwDelay = false;


#if BOARD==EVK1100 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == STK600_RCUC3D
/*!
 * \brief Interrupt handler of the External interrupt line "1" assigned to the NMI exception.
 *
 * \note This function is not static because it is referenced outside
 * (in exception.S for GCC and in exception.s82 for IAR).
 */
#if __GNUC__
__attribute__((__naked__))
#elif __ICCAVR32__
#pragma shadow_registers = full
#endif
void eic_nmi_handler( void )
{
	__asm__ __volatile__ (
			/* Save registers not saved upon NMI exception. */
			"pushm   r0-r12, lr\n\t"
			);
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_EXAMPLE_LINE1);
	key_number = 1;
	bCancelSwDelay = true;
	__asm__ __volatile__ (
			/* Restore the registers. */
			"popm   r0-r12, lr\n\t"
			/* Leaving the exception handler. */
			"rete"
			);
}
#endif // #if BOARD==EVK1100 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == STK600_RCUC3D

#if BOARD==EVK1101
/*!
 * \brief Interrupt handler of the External interrupt line "1".
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
	__interrupt
#endif
static void eic_int_handler1(void)
{
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_EXAMPLE_LINE1);
	key_number = 1;
	bCancelSwDelay = true;
}

/*!
 * \brief Interrupt handler of the External interrupt line "2".
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
	__interrupt
#endif
static void eic_int_handler2(void)
{
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_EXAMPLE_LINE2);
	key_number = 2;
	bCancelSwDelay = true;
}
#endif //if BOARD==EVK1101


/* \brief Software Delay
 *
 */
static void software_delay(void)
{
	volatile int i;
	bCancelSwDelay = false;
	for (i=10000; (i>=0)&&(false == bCancelSwDelay); i--);
}

/*
 * \brief main function : do init and wait for interrupt caught by the EIC controller.
 */
int main(void)
{
	// Enable edge-triggered interrupt.
	eic_options[0].eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	// Interrupt will trigger on falling edge.
	eic_options[0].eic_edge  = EIC_EDGE_FALLING_EDGE;
	// Initialize in synchronous mode : interrupt is synchronized to the clock
	eic_options[0].eic_async  = EIC_SYNCH_MODE;
	// Set the interrupt line number.
	eic_options[0].eic_line   = EXT_INT_EXAMPLE_LINE1;

	// Only available for EVK1101
#if BOARD==EVK1101
	// Enable edge-triggered interrupt.
	eic_options[1].eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	// Interrupt will trigger on falling edge.
	eic_options[1].eic_edge  = EIC_EDGE_FALLING_EDGE;
	// Initialize in synchronous mode : interrupt is synchronized to the clock
	eic_options[1].eic_async  = EIC_SYNCH_MODE;
	// Set the interrupt line number.
	eic_options[1].eic_line   = EXT_INT_EXAMPLE_LINE2;
#endif

	// Activate LED0 & LED1 pins in GPIO output mode and switch them off.
	gpio_set_gpio_pin(LED0_GPIO);
	gpio_set_gpio_pin(LED1_GPIO);

	// Map the interrupt lines to the GPIO pins with the right peripheral functions.
#if BOARD==EVK1100 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == STK600_RCUC3D
	gpio_enable_module_pin(EXT_INT_EXAMPLE_PIN_LINE1,EXT_INT_EXAMPLE_FUNCTION_LINE1);
#elif BOARD==EVK1101
	static const gpio_map_t EIC_GPIO_MAP =
	{
		{EXT_INT_EXAMPLE_PIN_LINE1, EXT_INT_EXAMPLE_FUNCTION_LINE1},
		{EXT_INT_EXAMPLE_PIN_LINE2, EXT_INT_EXAMPLE_FUNCTION_LINE2}
	};
	gpio_enable_module(EIC_GPIO_MAP,sizeof(EIC_GPIO_MAP) / sizeof(EIC_GPIO_MAP[0]));
#endif

	// Disable all interrupts.
	Disable_global_interrupt();

#if BOARD==EVK1101
	// Initialize interrupt vectors.
	INTC_init_interrupts();

	// Register the EIC interrupt handlers to the interrupt controller.
	INTC_register_interrupt(&eic_int_handler1,
			EXT_INT_EXAMPLE_IRQ_LINE1, AVR32_INTC_INT0);
	INTC_register_interrupt(&eic_int_handler2,
			EXT_INT_EXAMPLE_IRQ_LINE2, AVR32_INTC_INT0);
#endif
	/*
	 * On EVK1100, the JOYSTICK External interrupt line is assigned to the NMI.
	 * On EVK1104, the J17.7 External interrupt line is assigned to the NMI.
	 * Since the NMI is not an interrupt but an exception managed by the CPU, we have
	 * to make sure that the NMI handler calls our handler: this is done in the
	 * files exception.S(for GCC) & exception.s82(for IAR); look for the _handle_NMI
	 * assembly label.
	 */

	// Init the EIC controller with the options
	eic_init(&AVR32_EIC, eic_options,EXT_INT_EXAMPLE_NB_LINES);

	// Enable the chosen lines and their corresponding interrupt feature.
#if BOARD==EVK1100 || BOARD == EVK1104 || BOARD == UC3C_EK || BOARD == STK600_RCUC3D
	eic_enable_line(&AVR32_EIC, eic_options[0].eic_line);
	eic_enable_interrupt_line(&AVR32_EIC, eic_options[0].eic_line);
#elif BOARD==EVK1101
	eic_enable_lines(&AVR32_EIC,
			(1<<eic_options[1].eic_line)|(1<<eic_options[0].eic_line));
	eic_enable_interrupt_lines(&AVR32_EIC,
			(1<<eic_options[1].eic_line)|(1<<eic_options[0].eic_line));
#endif

	// Enable all interrupts.
	Enable_global_interrupt();

	// Loop infinitely, toggling the leds depending on the latest external event.
	while(1)
	{
		switch (key_number)
		{
			case 1 :
				gpio_clr_gpio_pin(LED0_GPIO);
				gpio_set_gpio_pin(LED1_GPIO);
				key_number = 0;
				software_delay();
				break;
			case 2 :
				gpio_set_gpio_pin(LED0_GPIO);
				gpio_clr_gpio_pin(LED1_GPIO);
				key_number = 0;
				software_delay();
				break;
			default :
				gpio_set_gpio_pin(LED0_GPIO);
				gpio_set_gpio_pin(LED1_GPIO);
				break;
		}
	}
}
