/**
 * \file
 *
 * \brief PMIC example for AVR XMEGA E
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "conf_example.h"

int main(void)
{
	board_init();
	sysclk_init();

	/* Turn off LEDs to start with */
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);

	/* Enable all three interrupt levels of the PMIC.
	 * Alternatively, use pmic_init() to achieve the same.
	 */
	pmic_enable_level(PMIC_LVL_LOW | PMIC_LVL_MEDIUM | PMIC_LVL_HIGH);

	/* Enable the timer/counter's clock. */
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC4);

	/* Initialize count, period and compare registers of the timer/counter. */
	TCC4.CNT = 0x0000;
	TCC4.PER = 0xffff;
	TCC4.CCA = 0x5555;
	TCC4.CCB = 0xaaaa;

	/* Set up timer/counter in normal mode with two compare channels. */
	TCC4.CTRLB |= TC45_WGMODE_NORMAL_gc;
	TCC4.CTRLE = TC45_CCAMODE_COMP_gc | TC45_CCBMODE_COMP_gc;

	/* Set levels for overflow and compare channel interrupts. */
	TCC4.INTCTRLA = TC45_OVFINTLVL_HI_gc;
	TCC4.INTCTRLB = TC45_CCAINTLVL_LO_gc | TC45_CCBINTLVL_MED_gc;

	/* Start the timer/counter and enable interrupts. */
	TCC4.CTRLA = TC45_CLKSEL_DIV64_gc;
	cpu_irq_enable();

	while (1) {
		/* Do nothing - LED toggling is managed by interrupts. */
		/* NOP to allow for debug pauses. */
		asm("nop");
	}
}

/* Definitions of Interrupt Service Routines */
ISR(TCC4_CCA_vect)
{
	LED_Toggle(LED0_GPIO);

	while (ioport_get_pin_level(BUTTON_PIN_0) == false) {
		/* Block while button 0 is being held */
	}
}

ISR(TCC4_CCB_vect)
{
	LED_Toggle(LED1_GPIO);

	while (ioport_get_pin_level(BUTTON_PIN_1) == false) {
		/* Block while button 0 is being held */
	}
}

ISR(TCC4_OVF_vect)
{
	LED_Toggle(LED2_GPIO);

	while (ioport_get_pin_level(BUTTON_PIN_2) == false) {
		/* Block while button 0 is being held */
	}
}

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows elementary use of the \ref pmic_group and
 * \ref interrupt_group.
 * It demonstrates:
 * - definition of Interrupt Service Routines (ISRs)
 * - enabling of individual interrupt levels
 * - global enabling of interrupts
 * - interruptability of lower interrupt levels
 *
 * \section files Main files:
 * - pmic_example_xmegae.c: PMIC and interrupt example application
 * - conf_example.h: configuration of I/O pins
 *
 * \section apiinfo PMIC and interrupt APIs
 * The PMIC API can be found \ref pmic_group "here".
 * The interrupt API can be found \ref interrupt_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with a type 0 timer/counter on port C can be used.
 * The example has been tested on the following boards:
 * - STK600 with RC023X routing card
 *
 * \section exampledescription Description of the example
 * Six I/O pins and a timer/counter (TCC4) are used in this example.
 * Three of the I/O pins (\ref LED_PIN_0 , \ref LED_PIN_1 , \ref LED_PIN_2) are
 * used to control on-board LEDs, while the rest (\ref BUTTON_PIN_0 ,
 * \ref BUTTON_PIN_1 , \ref BUTTON_PIN_2) are used for input from on-board
 * buttons (active low).
 *
 * The timer/counter is run in normal waveform generation mode, with the compare
 * channels A and B enabled. The interrupts for the compare channels, and the
 * timer/counter overflow are configured with LOW, MEDIUM and HIGH levels,
 * respectively. These interrupts will trigger in sequence, and their service
 * routines (ISR) toggle their respective LEDs on and off.
 *
 * To prevent the ISRs from returning, the user can press and hold the button
 * that corresponds to an ISR's LED, e.g., \ref BUTTON_PIN_0 for \ref LED_PIN_0.
 * The ISRs for interrupts of higher level will still execute in this case,
 * causing only the LEDs with higher pin number to toggle. Once the button is
 * released, the ISR will return and pending interrupts be handled.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
