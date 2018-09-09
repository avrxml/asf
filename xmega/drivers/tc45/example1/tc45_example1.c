/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 1
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref tc45_group to toggle LEDs.
 *
 * The example will configure TCC4 module in normal mode and use three TCC4
 * interrupts (overflow, Capture/Compare Channels A and B).
 *
 * Each callback interrupts functions are setup to toggle a LED:
 *  - LED0: Toggles on TC4 overflow interrupt
 *  - LED1: Toggle on Compare Channel A interrupt
 *  - LED2: Toggle on Compare Channel B interrupt
 *
 * The TCC4 is setup to use a 31250Hz resolution clock and a 31250 period value
 * resulting in a 1Hz TC4 overflow frequency.
 *
 * Compare Channels A and B will be setup at 1/4 and 1/2 of the TC4 period.
 * Thus, the LEDs sequence produced is LED2/LED1/LED0/LED2....
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 *  - tc45.c Timer XMEGA Timer Counter driver implementation
 *  - tc45.h Timer XMEGA Timer Counter driver definitions
 *  - tc45_example1.c example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PortA must be connected to LEDs
 *
 * For XMEGA-E5 Xplained board:
 * - plug USB for the power
 * - note, no LED2 on this board
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

/* Timer resolution (Hz)
 * Note: This configure a TC prescaler equal to 1024 = SYSCLK 32MHz / 31250Hz
 */
#define TIMER_EXAMPLE_RESOLUTION 31250

/**
 * \brief Timer Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred on
 * TCC4 and toggles LED0.
 */
static void example_ovf_interrupt_callback(void)
{
	gpio_toggle_pin(LED0_GPIO);
	tc45_clear_overflow(&TCC4);
}

/**
 * \brief Timer Counter Capture/Compare A interrupt callback function
 *
 * This function is called when an a capture compare channel A has occurred
 * on TCC4 and toggles LED1.
 */
static void example_cca_interrupt_callback(void)
{
	gpio_toggle_pin(LED1_GPIO);
}

/**
 * \brief Timer Counter Capture/Compare B interrupt callback function
 *
 * This function is called when an a capture compare channel B has occurred
 * on TCC4 and toggles LED2 (only on STK600).
 */
static void example_ccb_interrupt_callback(void)
{
#if (BOARD == STK600_RC032X)
	gpio_toggle_pin(LED2_GPIO);
#endif
}

int main(void)
{
	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Unmask clock for TCC4 */
	tc45_enable(&TCC4);

	/* Configure TC in normal mode */
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);

	/* Configure period equal to resolution to obtain 1Hz */
	tc45_write_period(&TCC4, TIMER_EXAMPLE_RESOLUTION);

	/* Configure CCA to occur at the middle of TC period */
	tc45_write_cc(&TCC4, TC45_CCA, TIMER_EXAMPLE_RESOLUTION / 2);

	/* Configure CCB to occur at the quarter of TC period */
	tc45_write_cc(&TCC4, TC45_CCB, TIMER_EXAMPLE_RESOLUTION / 4);

	/* Enable both CCA and CCB channels */
	tc45_enable_cc_channels(&TCC4, TC45_CCACOMP);
	tc45_enable_cc_channels(&TCC4, TC45_CCBCOMP);

	/*
	 * Configure interrupts callback functions for TCC4
	 * overflow interrupt, CCA interrupt and CCB interrupt
	 */
	tc45_set_overflow_interrupt_callback(&TCC4,
			example_ovf_interrupt_callback);
	tc45_set_cca_interrupt_callback(&TCC4,
			example_cca_interrupt_callback);
	tc45_set_ccb_interrupt_callback(&TCC4,
			example_ccb_interrupt_callback);

	/*
	 * Enable TC interrupts (overflow, CCA and CCB)
	 */
	tc45_set_overflow_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_cca_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	tc45_set_ccb_interrupt_level(&TCC4, TC45_INT_LVL_LO);

	/*
	 * Run TCC4 
	 */
	tc45_set_resolution(&TCC4, TIMER_EXAMPLE_RESOLUTION);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
