/**
 * \file
 *
 * \brief AVR MEGARF WDT driver example
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

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use MEGARF WDT module.
 *
 * \section files Main Files
 * - wdt_example.c: the example application.
 * - wdt_megarf.c: the watchdog driver.
 * - wdt_megarf.h: the watchdog driver header.
 * - conf_board.h: board configuration
 *
 * \section deviceinfo Device Info
 * MEGARF devices can be used.
 *
 * \section exampledescription Description of the example
 * The example enables the WDT in three different modes, system reset,
 * interrupt and interrupt reset modes, and plays with all the WDT features.
 * The LED will stay ON when all the test modes are executed successfully.
 * In case of failure LED will blink every 2 seconds once.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "conf_board.h"

/*! brief States of the example.
 */
enum wdt_example_state {
	/* ! Writing tests in WDT */
	START_OF_PROG = (0x01),
	/* ! MCU reset using WDT */
	WDT_MCU_RESET = (0x02),
	/* ! Enable WDT in Reset mode, 8x wdt_reset() before TIMEOUT (TO) */
	REFRESH_WDT = (0x03),
	/* ! Enable WDT in Reset mode, no wdt_reset(), reaching the TO */
	WDT_RST = (0x04),

	/* Enable WDT in Interrupt mode, no wdt_reset(), interrupt will
	 * get triggered when reaching the TO */
	WDT_INTERRUPT = (0x05),
	/* ! Enable WDT in Reset Interrupt mode, no wdt_reset(), reaching the TO */
	WDT_RST_INTERRUPT = (0x06),
	/* ! Indicate the test complete through LED On */
	END_OF_PROG = (0x07),
	/* ! WDT error (loop without end) */
	ERROR_STATE = (0x0F),
};

/*! \brief Global variables not initialized.
 */
#if defined (__GNUC__)
volatile enum wdt_example_state state_flag
__attribute__ ((section(".noinit")));
#elif defined(__ICCAVR__)
__no_init volatile enum wdt_example_state state_flag;
#else
#error Unsupported compiler.
#endif

/**
 * \brief Interrupt callback for wdt timer overflow interrupt
 *  Change the state when the interrupt is triggered
 *  \param none
 */
static void wdt_timer_callback(void)
{
	if (state_flag != WDT_RST_INTERRUPT) {
		state_flag = WDT_RST_INTERRUPT;
	} else {
		state_flag = END_OF_PROG;
	}
}

/** \brief Main function. */
int main(void)
{
	uint8_t delay_counter;
	volatile uint16_t delay;

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	sysclk_init();
	board_init();

	/* Detection of all RESET except WDT RESET. */
	if ((reset_cause_get_causes() & CHIP_RESET_CAUSE_WDT)
			!= CHIP_RESET_CAUSE_WDT) {
		state_flag = START_OF_PROG;
		reset_cause_clear_causes(CHIP_RESET_CAUSE_POR |
				CHIP_RESET_CAUSE_EXTRST |
				CHIP_RESET_CAUSE_BOD_CPU);
	} else {
		reset_cause_clear_causes(CHIP_RESET_CAUSE_WDT);
	}

	wdt_disable();

	while (true) {
		switch (state_flag) {
		case START_OF_PROG:
			/* Writing test. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_2KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_1024KCLK);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_1024KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			/* Wait for 2 s. */
			delay = 2000;
			delay_ms(delay);
			state_flag = WDT_MCU_RESET;
			break;

		case WDT_MCU_RESET:
			state_flag = REFRESH_WDT;
			wdt_reset_mcu();
			break;

		case REFRESH_WDT:
			/* Enable WDT 500 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_64KCLK);
			wdt_enable(SYSTEM_RESET_MODE);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_64KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			for (delay_counter = 0; delay_counter < 8;
					delay_counter++) {
				/* Wait for 8x 250 ms = 2 s. */
				delay = 250;
				delay_ms(delay);
				wdt_reset();
			}
			wdt_disable();
			state_flag = WDT_RST;
			break;

		case WDT_RST:
			state_flag = WDT_INTERRUPT;
			/* Enable WDT 2 s. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_256KCLK);
			wdt_enable(SYSTEM_RESET_MODE);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_256KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			while (true) {
				/* Wait for Watchdog reset. */
			}
		/* No break is needed */

		case WDT_INTERRUPT:
			/* Enable WDT 250 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_32KCLK);
			wdt_set_interrupt_callback(wdt_timer_callback);
			wdt_enable(INTERRUPT_MODE);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_32KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			cpu_irq_enable();
			/* Wait for interrupt to get triggered */
			delay = 400;
			delay_ms(delay);
			wdt_disable();
			break;

		case WDT_RST_INTERRUPT:

			/* Enable WDT 125 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_16KCLK);
			wdt_set_interrupt_callback(wdt_timer_callback);
			wdt_enable(INTERRUPT_SYSTEM_RESET_MODE);
			if (wdt_get_timeout_period() !=
					WDT_TIMEOUT_PERIOD_16KCLK) {
				state_flag = ERROR_STATE;
				break;
			}

			/* Wait for 200 ms. */
			delay = 200;
			delay_ms(delay);
			wdt_reset();
			while (true) {
				/* Wait for Watchdog reset. */
			}
		/* No break is needed */

		case ERROR_STATE:

			wdt_disable();

			while (true) {
				LED_On(LED_PIN);
				/* Wait for 500 ms. */
				delay = 500;
				delay_ms(delay);
				/* Blinking. */
				LED_Off(LED_PIN);
				/* Wait for 500 ms. */
				delay = 500;
				delay_ms(delay);
			}
		/* No break is needed */

		case END_OF_PROG:
		default:
			LED_On(LED_PIN);
			while (true) {
			}
			/* No break is needed */
		}
	}
}
