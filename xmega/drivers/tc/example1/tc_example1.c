/**
 * \file
 *
 * \brief AVR XMEGA TC Driver Example 1
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This simple example shows how to use the \ref tc_group to toggle LEDs.
 *
 * \section files Main files:
 *  - tc.c Timer XMEGA Timer Counter driver implementation
 *  - tc.h Timer XMEGA Timer Counter driver definitions
 *  - tc_example1.c example application
 *  - conf_example.h: configuration of the example
 *
 * \section driverinfo TC Driver
 * The XMEGA TC driver can be found \ref tc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an tc can be used.
 *
 * \section exampledescription Description of the example
 * The example will configure one TC module (\ref TIMER_EXAMPLE) in normal mode and
 * use three TC interrupts (overflow, Capture/Compare Channels A and B).
 *
 * The TC is setup to use a 31250Hz resolution clock and a 31250 period value
 * resulting in a 1Hz TC overflow frequency.
 * Compare Channels A and B will be setup at 1/4 and 1/2 of the TC period.

 * Each callback interrupts functions are setup to toggle a LED.
 *  - LED0: Toggles on TC overflow interrupt
 *  - LED1: Toggle on Compare Channel A
 * In the case of A1 Xplain:
 *  - LED2: Toggle on Compare Channel B interrupts
 * In the case of A3BU Xplained:
 *  - Status LED: Toggles red on Compare Channel B interrupts
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
#include <conf_example.h>
#include <string.h>
#include <asf.h>

/**
 * \brief Timer Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred on
 * TIMER_EXAMPLE and toggles LED0.
 */
static void example_ovf_interrupt_callback(void)
{
	gpio_toggle_pin(LED0_GPIO);
}

/**
 * \brief Timer Counter Capture/Compare A interrupt callback function
 *
 * This function is called when an a capture compare channel A has occurred
 * TIMER_EXAMPLE and toggles LED1.
 */
static void example_cca_interrupt_callback(void)
{
	gpio_toggle_pin(LED1_GPIO);
}
/**
 * \brief Timer Counter Capture/Compare B interrupt callback function
 *
 * This function is called when an a capture compare channel B has occurred
 * TIMER_EXAMPLE and toggles LED2.
 */
static void example_ccb_interrupt_callback(void)
{
	gpio_toggle_pin(LED2_GPIO);
}


int main(void)
{
	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();

	cpu_irq_enable();

#if (BOARD == XMEGA_A3BU_XPLAINED)
	/* The status LED must be used as LED2, so we turn off
	 * the green led which is in the same packaging. */
	ioport_set_pin_high(LED3_GPIO);
#endif

	/*
	* Unmask clock for TIMER_EXAMPLE
	*/
	tc_enable(&TIMER_EXAMPLE);

	/*
	* Configure interrupts callback functions for TIMER_EXAMPLE
	* overflow interrupt, CCA interrupt and CCB interrupt
	*/
	tc_set_overflow_interrupt_callback(&TIMER_EXAMPLE,
			example_ovf_interrupt_callback);
	tc_set_cca_interrupt_callback(&TIMER_EXAMPLE,
			example_cca_interrupt_callback);
	tc_set_ccb_interrupt_callback(&TIMER_EXAMPLE,
			example_ccb_interrupt_callback);

	/*
	* Configure TC in normal mode, configure period, CCA and CCB
	* Enable both CCA and CCB channels
	*/

	tc_set_wgm(&TIMER_EXAMPLE, TC_WG_NORMAL);
	tc_write_period(&TIMER_EXAMPLE, TIMER_EXAMPLE_PERIOD);
	tc_write_cc(&TIMER_EXAMPLE, TC_CCA, TIMER_EXAMPLE_PERIOD / 2);
	tc_write_cc(&TIMER_EXAMPLE, TC_CCB, TIMER_EXAMPLE_PERIOD / 4);
	tc_enable_cc_channels(&TIMER_EXAMPLE,(enum tc_cc_channel_mask_enable_t)(TC_CCAEN | TC_CCBEN));

	/*
	* Enable TC interrupts (overflow, CCA and CCB)
	*/
	tc_set_overflow_interrupt_level(&TIMER_EXAMPLE, TC_INT_LVL_LO);
	tc_set_cca_interrupt_level(&TIMER_EXAMPLE, TC_INT_LVL_LO);
	tc_set_ccb_interrupt_level(&TIMER_EXAMPLE, TC_INT_LVL_LO);

	/*
	* Run TIMER_EXAMPLE at TIMER_EXAMPLE_PERIOD(31250Hz) resolution
	*/
	tc_set_resolution(&TIMER_EXAMPLE, TIMER_EXAMPLE_PERIOD);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
