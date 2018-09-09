/**
 * \file
 *
 * \brief Nested External Interrupt example for megaRF device
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
 * nested external interrupt
 *
 * \section files Main files:
 * - ext_int_example2.c: This example application
 * - conf_interrupt.h: configuration of the megaRF driver
 *
 * \section driverinfo Interrupt Driver
 * The interrupt driver can be found \ref megarf_interrupt_group "here".
 *
 * \section deviceinfo Device Info
 * AVR MEGARF device can be used.
 *
 * \section setup Setup
 * External interrupt INT0 pin is enabled and user defined another external
 * interrupt source(example INT1) is also enabled.SW1 and SW2 are the switches
 * connected to the interrupt pins
 *
 * \section exampledescription Description of the example
 * switch SW1 is pressed, due to pull-up the
 * interrupt INT0 will be triggered only when SW1 is released and LED1 blinks
 * for some time. During the time while LED1 is glowing, a switch action on
 * another SW2 will trigger INT1 and hence the LED2 blinks for a moment and
 * goes off. This is because, inside the ISR of INT0, the I-bit is set so
 * that all other interrupts are activated. So even when the CPU is inside
 * the INT0 routine it senses the interrupt INT1 and jumps to the ISR of
 * INT1 and executes the routine and then jumps back to ISR of INT0.
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
#include <conf_interrupt.h>

#ifndef CONFIG_INTERRUPT_SOURCE
# define CONFIG_INTERRUPT_SOURCE    EXT_INT0
#endif /* CONFIG_INTERRUPT_SOURCE */

#ifndef CONFIG_INTERRUPT_MODE
# define CONFIG_INTERRUPT_MODE      IOPORT_SENSE_FALLING
#endif /* CONFIG_INTERRUPT_SOURCE */

/**
 *  \brief delay routine for LED to be ON for some time
 *  \param none
 */
static inline void ext_int_delay(void)
{
	for (volatile unsigned int i = 0; i < 1000; i++) {
	}
}

/**
 * \brief Interrupt callback for External interrupt
 *  Switch ON LED2(PORTE2) for some time and then switches OFF
 *  when the interrupt is triggered
 *  \param none
 */

static void ext_int1_callback(void)
{
	/* Switch ON LED2 */
	LED_On(LED_GREEN_GPIO);

	ext_int_delay();

	/* Switch OFF LED2 */
	LED_Off(LED_GREEN_GPIO);
}

/**
 *  \brief Interrupt callback for External interrupt
 *  Switch ON LED1(PORTE3) for some time and then switches OFF
 *  when the interrupt is triggered. Also I-bit is set so that all
 *  other interrupts are activated
 *  \param none
 */
static void ext_int0_callback(void)
{
	/* Enable Global interrupt */
	cpu_irq_enable();

	/* ! Switch ON LED1 */
	LED_On(LED_YELLOW_GPIO);

	for (volatile unsigned int i = 0; i < 100; i++) {
		ext_int_delay();
	}

	/* Switch OFF LED1 */
	LED_Off(LED_YELLOW_GPIO);
}

/**
 * \brief Main function.
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
	ext_int_set_interrupt_callback(CONFIG_INTERRUPT_SOURCE,ext_int1_callback);

	/* Enable the Ext Int */
	ext_int_init(CONFIG_INTERRUPT_SOURCE, CONFIG_INTERRUPT_MODE);
	

	/* Set Interrupt CallBack Function **/
	ext_int_set_interrupt_callback(EXT_INT0_PIN,ext_int0_callback);

	/* Enable the Ext Int0 */
	ext_int_init(EXT_INT0_PIN, IOPORT_SENSE_RISING);
	

	/* Enable Global interrupt */
	cpu_irq_enable();

	/* Infinite loop and waiting for the external interrupt to occur */
	while (1) {
	}
}
