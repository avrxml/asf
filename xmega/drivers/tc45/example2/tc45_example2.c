/**
 * \file
 *
 * \brief AVR XMEGA TC45 Driver Example 2
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
 * This simple example shows how to use the \ref tc45_group to generate
 * single slope PWM.
 *
 * The example uses and configures two TC modules.
 *  - TCD5 is configured to generate single slope PWM on LED0
 *  - TCC4 is used in generate a 50ms overflow interrupt
 *
 * Each time the 50ms overflow interrupt of TCC4 is executed
 * the TCD5 single slope PWM duty cycle is changed so that it
 * result visible on LEDO light intensity.
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The TC45 driver API can be found \ref tc45_group "here".
 *
 * Main files:
 *  - tc45.c Timer XMEGA Timer Counter driver implementation
 *  - tc45.h Timer XMEGA Timer Counter driver definitions
 *  - tc45_example2.c example application
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PD4 must be connected to LED0
 *
 * For XMEGA-E5 Xplained board:
 * - plug USB for the power
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

/**
 * \brief Timer Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred (50ms) on
 * TCD5 and change PWM duty single generation on TCD5 CCA.
 */
static void example_overflow_interrupt_callback(void)
{
	tc45_write_cc(&TCD5, TC45_CCA,
			tc45_read_cc(&TCD5, TC45_CCA) + 0xA00);
	tc45_clear_overflow(&TCC4);
}

int main(void)
{
	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
        
	/*
	 * Configure TCC4 to generate 50ms overflow interrupt
	 * using 500kHz (2us) resolution clock (50ms = 25000 * 2us)
	 */
	/* Unmask clock for TCC4 */
	tc45_enable(&TCC4);
	/* Enable overflow interrupt */
	tc45_set_overflow_interrupt_level(&TCC4, TC45_INT_LVL_LO);
	/* Configure TC in normal mode */
	tc45_set_wgm(&TCC4, TC45_WG_NORMAL);
	/* Configure call back interrupt */
	tc45_set_overflow_interrupt_callback(&TCC4,
			example_overflow_interrupt_callback);
	/* Configure TC period and resolution */
	tc45_write_period(&TCC4, 25000);
	tc45_set_resolution(&TCC4, 500000);

	/*
	 * Configure TCD5 to generate 2ms Single Slope PWM
	 */
	/* Unmask clock for TCD5 */
	tc45_enable(&TCD5);
	/* Configure TC in PWM Single Slope PWM */
	tc45_set_wgm(&TCD5, TC45_WG_SS);
	/* Use the max period (2MHz / FFFFh = 30Hz) */
	tc45_write_period(&TCC4, 0xFFFF);
	/* Initialize and enable Channel A */
	tc45_write_cc(&TCD5, TC45_CCA, 0);
	tc45_enable_cc_channels(&TCD5, TC45_CCACOMP);
	/* Run TC at 2MHz clock resolution (CPU frequency 32MHz) */
	tc45_set_resolution(&TCD5, 20000000);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		sleepmgr_enter_sleep();
	} while (1);
}
