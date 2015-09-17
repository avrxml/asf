/**
 * \file
 *
 * \brief AVR XMEGA WDT driver example
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 * This example demonstrates how to use XMEGA WDT module.
 *
 * \section files Main Files
 * - wdt_example.c: the example application.
 * - wdt.c: the watchdog driver.
 * - wdt.h: the watchdog driver header.
 * - conf_board.h: board configuration
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices can be used. This example has been tested
 * with the following setup:
 *   - Xplain evaluation kit.
 *   - XMEGA A1 Xplained evaluation kit.
 *   - XMEGA A3BU Xplained evaluation kit.
 *   - STK600/RC044X with ATxmega32A4U.
 *   - STK600/RC064X with ATxmega64A3.
 *   - STK600/RC064X with ATxmega256A3.
 *   - STK600/RC064X with ATxmega256A3U.
 *
 * \section exampledescription Description of the example
 * The example enables the WDT in both modes, standard and window, and
 * plays with all the WDT features. The LEDs output every 2 seconds the number
 * of the test (states of the example) and can display a WDT failure.
 *
 * The following LED configuration is used in the example:
 *   - XMEGA A1 Xplained: LED 0 through 3.
 *   - XMEGA A3BU Xplained: red on the status LED indicates that LED2 is on,
 *     while green on the status LED indicates that LED3 is on.
 *   - XMEGA B1 Xplained: LED 0 through 3.
 *   - STK600/RC044X: requires PORTA to be connected to LEDs. Output will be
 *     shown on LED 0 through 3.
 *   - STK600/RC064X: requires PORTE to be connected to LEDs. Output will be
 *     shown on LED 0 through 3.
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

/**
 * \brief LED display value
 *
 * \param led_val LED display value
 */
static void led_display(uint8_t led_val)
{
	if (led_val & 0x01) {
		LED_On(LED0_GPIO);
	} else {
		LED_Off(LED0_GPIO);
	}

	if (led_val & 0x02) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
	}

	if (led_val & 0x04) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}

	if (led_val & 0x08) {
		LED_On(LED3_GPIO);
	} else {
		LED_Off(LED3_GPIO);
	}
}

/** brief States of the example. */
enum wdt_example_state_t {
	/** Writing tests in WDT */
	START_OF_PROG  = 1,
	/** MCU reset using WDT */
	WDT_MCU_RESET  = 2,
	/** WDT in normal mode, 8x wdt_reset() before TIMEOUT (TO) */
	REFRESH_NO_WIN = 3,
	/** WDT in window mode, 4x wdt_reset() when window is opened & before TO */
	REFRESH_WINDOW = 4,
	/** WDT in normal mode, no wdt_reset(), reaching the TO */
	WDT_RST_NO_WIN = 5,
	/** WDT in window mode, no wdt_reset(), reaching the TO */
	WDT_RST_WINDOW = 6,
	/** WDT in window mode, wdt_reset() when window is closed */
	WDT_RST_RFSH_W = 7,
	/** MCU software Reset and the program restarts */
	END_OF_PROG    = 8,
	/** WDT error (loop without end) */
	ERROR_STATE    = 9,
};

/** \brief Global variables not initialized. */
#if defined (__GNUC__)
volatile enum wdt_example_state_t state_flag
		__attribute__ ((section(".noinit")));
#elif defined(__ICCAVR__)
__no_init volatile enum wdt_example_state_t state_flag;
#else
#  error Unsupported compiler.
#endif

/** \brief Main function. */
int main(void)
{
	uint8_t i;
	
	/* Initialize the board hardware and system clocks. */
	board_init();
	sysclk_init();

	/* Detection of all RESET excepted WDT RESET. */
	if (reset_cause_get_causes() & ~CHIP_RESET_CAUSE_WDT) {
		/* Wait for 2 s. */
		delay_ms(2000);
		state_flag = START_OF_PROG;
		reset_cause_clear_causes(
				CHIP_RESET_CAUSE_POR |
				CHIP_RESET_CAUSE_EXTRST |
				CHIP_RESET_CAUSE_BOD_CPU |
				CHIP_RESET_CAUSE_OCD |
				CHIP_RESET_CAUSE_SOFT |
				CHIP_RESET_CAUSE_SPIKE);
	} else {
		reset_cause_clear_causes(CHIP_RESET_CAUSE_WDT);
	}

	while (true) {
		led_display(state_flag);

		switch (state_flag) {
		case START_OF_PROG:
			/* Writing test. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_8CLK);
			if (wdt_get_timeout_period() != WDT_TIMEOUT_PERIOD_8CLK) {
				state_flag = ERROR_STATE;
				break;
			}

			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_250CLK);
			if (wdt_get_timeout_period() != WDT_TIMEOUT_PERIOD_250CLK) {
				state_flag = ERROR_STATE;
				break;
			}

			/* Wait for 2 s. */
			delay_ms(2000);
			state_flag = WDT_MCU_RESET;
			break;

		case WDT_MCU_RESET:
			/* Wait for 2 s. */
			delay_ms(2000);
			state_flag = REFRESH_NO_WIN;
			wdt_reset_mcu();
			break;

		case REFRESH_NO_WIN:
			/* Enable WDT 500 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_500CLK);
			wdt_enable();
			for (i = 0; i < 8; i++) {
				/* Wait for 8x 250 ms = 2 s. */
				delay_ms(250);
				wdt_reset();
			}
			wdt_disable();
			state_flag = REFRESH_WINDOW;
			break;

		case REFRESH_WINDOW:
			/* Enable Window 250 ms & WDT 500 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_500CLK);
			wdt_enable();
			if (!(wdt_set_window_period(WDT_WINDOW_PERIOD_250CLK)))
			{
				state_flag = ERROR_STATE;
				break;
			}

			if (!(wdt_enable_window_mode())) {
				state_flag = ERROR_STATE;
				break;
			}

			for (i = 0; i < 4; i++) {
				/* Wait for 500 ms. */
				delay_ms(500);
				wdt_reset();
			}
			wdt_disable();
			state_flag = WDT_RST_NO_WIN;
			break;

		case WDT_RST_NO_WIN:
			state_flag = WDT_RST_WINDOW;
			/* Enable WDT 2 s. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_2KCLK);
			wdt_enable();
			while (true) {
				/* Wait for Watchdog reset. */
			}
			break;

		case WDT_RST_WINDOW:
			state_flag = WDT_RST_RFSH_W;
			/* Enable Window 1 s & WDT 1 s. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_1KCLK);
			wdt_enable();
			if (!(wdt_set_window_period(WDT_WINDOW_PERIOD_1KCLK))) {
				state_flag = ERROR_STATE;
				break;
			}

			if (!(wdt_enable_window_mode())) {
				state_flag = ERROR_STATE;
				break;
			}

			while (true) {
				/* Wait for Watchdog reset. */
			}

			break;

		case WDT_RST_RFSH_W:
			state_flag = END_OF_PROG;
			/* Enable Window 4 s & WDT 250 ms. */
			wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_250CLK);
			wdt_enable();
			if (!(wdt_set_window_period(WDT_WINDOW_PERIOD_4KCLK))) {
				state_flag = ERROR_STATE;
				break;
			}

			if (!(wdt_enable_window_mode())) {
				state_flag = ERROR_STATE;
				break;
			}

			/* Wait for 2 s. */
			delay_ms(2000);
			wdt_reset();
			while (true) {
				/* Wait for Watchdog reset. */
			}

			break;

		case ERROR_STATE:
			while (true) {
				led_display(ERROR_STATE);
				/* Wait for 500 ms. */
				delay_ms(500);
				/* Blinking. */
				led_display(~ERROR_STATE);
				/* Wait for 500 ms. */
				delay_ms(500);
			}
		
			break;

		case END_OF_PROG:
		default:
			/* Wait for 2 s. */
			delay_ms(2000);
			reset_do_soft_reset();
			break;
		}
	}
}
