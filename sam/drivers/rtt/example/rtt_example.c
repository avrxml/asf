/**
 * \file
 *
 * \brief Real-time Timer (RTT) example for SAM.
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

/**
 * \mainpage RTT Example
 *
 * \section Purpose
 *
 * This example demonstrates the Real-Time Timer (RTT) provided on
 * SAM microcontrollers. It enables the user to set an alarm and watch
 * it being triggered when the timer reaches the corresponding value.
 *
 * \section Requirements
 *
 * This package can be used with SAM evaluation kits.
 *
 * \section Description
 *
 * When launched, this program displays a timer count and a menu on the
 * terminal, enabling the user to choose between several options.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 *    \code
	-- RTT Example --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	Time: 1
	Menu:
	r - Reset timer
	s - Set alarm
	Choice?
\endcode
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

/** Device state: in the main menu. */
#define STATE_MAIN_MENU      0
/** Device state: user is setting an alarm time. */
#define STATE_SET_ALARM      1

/** ASCII char definition for backspace. */
#define ASCII_BS    8
/** ASCII char definition for carriage return. */
#define ASCII_CR    13

#define STRING_EOL    "\r"
#define STRING_HEADER "-- RTT Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Current device state. */
volatile uint8_t g_uc_state;

/** New alarm time being currently entered. */
volatile uint32_t g_ul_new_alarm;

/** Indicate that an alarm has occurred but has not been cleared. */
volatile uint8_t g_uc_alarmed;

/**
 * \brief Refresh display on terminal.
 *
 * Update the terminal display to show the current menu and the current time
 * depending on the device state.
 */
static void refresh_display(void)
{
	printf("%c[2J\r", 27);
	printf("Time: %u\n\r", (unsigned int)rtt_read_timer_value(RTT));

	/* Display alarm */
	if (g_uc_alarmed) {
		puts("!!! ALARM !!!\r");
	}

	/* Main menu */
	if (g_uc_state == STATE_MAIN_MENU) {
		puts("Menu:\n\r"
				" r - Reset timer\n\r"
				" s - Set alarm\r");
		if (g_uc_alarmed) {
			puts(" c - Clear alarm notification\r");
		}
		puts("\n\rChoice? ");
	} else {
		if (g_uc_state == STATE_SET_ALARM) {
			puts("Enter alarm time: ");
			if (g_ul_new_alarm != 0) {
				printf("%u", (unsigned)g_ul_new_alarm);
			}
		}
	}
}

/**
 * \brief RTT configuration function.
 *
 * Configure the RTT to generate a one second tick, which triggers the RTTINC
 * interrupt.
 */
static void configure_rtt(void)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
#if SAM4N || SAM4S || SAM4E || SAM4C || SAM4CP || SAM4CM || SAMV71 || SAMV70 || SAME70 || SAMS70
	rtt_sel_source(RTT, false);
#endif
	rtt_init(RTT, 32768);

	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN);
}

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Interrupt handler for the RTT.
 *
 * Display the current time on the terminal.
 */
void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* Time has changed, refresh display */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		refresh_display();
	}

	/* Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		g_uc_alarmed = 1;
		refresh_display();
	}
}

/**
 * \brief Application entry point for RTT example.
 *
 * Initialize the RTT, display the current time and allow the user to
 * perform several actions: clear the timer, set an alarm, etc.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t c;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Configure console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure RTT */
	configure_rtt();

	/* Initialize state machine */
	g_uc_state = STATE_MAIN_MENU;
	g_uc_alarmed = 0;
	refresh_display();

	/* User input loop */
	while (1) {
		/* Wait for user input */
		scanf("%c", (char *)&c);

		/* Main menu mode */
		if (g_uc_state == STATE_MAIN_MENU) {
			/* Reset timer */
			if (c == 'r') {
				configure_rtt();
				refresh_display();
			} else if (c == 's') { /* Set alarm */
				g_uc_state = STATE_SET_ALARM;
				g_ul_new_alarm = 0;
				refresh_display();
			} else { /* Clear alarm */
				if ((c == 'c') && g_uc_alarmed) {
					g_uc_alarmed = 0;
					refresh_display();
				}
			}
		} else if (g_uc_state == STATE_SET_ALARM) { /* Set alarm mode */
			/* Number */
			if ((c >= '0') && (c <= '9')) {
				g_ul_new_alarm = g_ul_new_alarm * 10 + c - '0';
				refresh_display();
			} else if (c == ASCII_BS) {
				printf("%c", c);
				g_ul_new_alarm /= 10;
				refresh_display();
			} else if (c == ASCII_CR) {
				/* Avoid newAlarm = 0 case */
				if (g_ul_new_alarm != 0) {
					rtt_write_alarm_time(RTT, g_ul_new_alarm);
				}

				g_uc_state = STATE_MAIN_MENU;
				refresh_display();
			}
		}
	}
}
