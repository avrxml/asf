/**
 * \file
 *
 * \brief Low Power Application.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 * \mainpage Low Power Application
 *
 * \section Purpose
 *
 * This example shows all the different low power modes with several types
 * of wake-up sources. And the consumption of the core in different power
 * modes can be measured.
 *
 * \section Requirements
 *
 * This package can be used with SAM evaluation kits.
 *
 * \section Description
 *
 * The program will display a menu on console. It allows users to change the
 * configuration and enter into a different power mode, and then measure the
 * power consumption.
 *
 * An amperemeter has to be plugged on the board instead of the VDDx jumper.
 *
 * Note that for better consumption measurement:
 * - Run program out of flash without ICE connected.
 *
 * \section Usage
 *
 *
 * - Build the program and download it to the evaluation board.
 * - Start the application.
 *
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
 *     -- Low Power Example xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *
 *     ===============================================
 *     Menu: press a key to continue.
 *     ===============================================
 *     Configure:
 *       0 : Power Saving Mode 0
 *       1 : Power Saving Mode 1
 *     Mode:
 *       A : Active Mode
 *       R : Retention Mode
 *       W : Wait Mode
 *       B : Backup Mode
 *     Quit:
 *       Q : Quit test.
 *
 *     -----------------------------------------------
 *     Current configuration:
 *       CPU Clock         : MCK=24000000 Hz
 *       Flash access mode : 128-bit
 *     -----------------------------------------------
 *    \endcode
 * -# Press one of the keys listed in the menu to perform the corresponding
 *    action.
 */

#include "asf.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Low Power Demo --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Button pressed flag */
volatile uint32_t g_ul_button_pressed = 0;

/**
 *  Configure serial console.
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

	/* Configure console. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

static void display_menu_core(void)
{
	printf("\n\r");
	printf("===============================================\n\r");
	printf("Menu: press a key to continue.\n\r");
	printf("===============================================\n\r");
	printf("Configure:\n\r");
	printf("  0 : Power Saving Mode 0\n\r");
	printf("  1 : Power Saving Mode 1\n\r");
	printf("Mode:\n\r");
	printf("  A : Active Mode\n\r");
	printf("  R : Retention Mode\n\r");
	printf("  W : Wait Mode\n\r");
	printf("  B : Backup Mode\n\r");
	printf("Quit:\n\r");
	printf("  Q : Quit test.\n\r");

	printf("\n\r");
	printf("-----------------------------------------------\n\r");
	printf("\n\r");
}

static void button_handler(void)
{
	/* Check if EIC push button line interrupt line is pending. */
	if (eic_line_interrupt_is_pending(EIC, BUTTON_0_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, BUTTON_0_EIC_LINE);
		g_ul_button_pressed = 1;
	}
}


static void configure_button(void)
{
	struct eic_line_config eic_line_conf;

	eic_line_conf.eic_mode = EIC_MODE_EDGE_TRIGGERED;
	eic_line_conf.eic_edge = EIC_EDGE_FALLING_EDGE;
	eic_line_conf.eic_level = EIC_LEVEL_LOW_LEVEL;
	eic_line_conf.eic_filter = EIC_FILTER_DISABLED;
	eic_line_conf.eic_async = EIC_ASYNCH_MODE;

	eic_enable(EIC);
	eic_line_set_config(EIC, BUTTON_0_EIC_LINE,
	&eic_line_conf);
	eic_line_set_callback(EIC, BUTTON_0_EIC_LINE, button_handler,
	EIC_1_IRQn, 1);
	eic_line_enable(EIC, BUTTON_0_EIC_LINE);
}

static void app_prime_number_run(void)
{
	#define PRIM_NUMS 8
	uint32_t i, d, n;
	uint32_t primes[PRIM_NUMS];


	// Find prime numbers forever
	primes[0] = 1;
	for (i = 1; i < PRIM_NUMS;) {
		for (n = primes[i - 1] + 1; ;n++) {
			for (d = 2; d <= n; d++) {
				if (n == d) {
					primes[i] = n;
					goto nexti;
				}
				if (n%d == 0) break;
			}
		}
		nexti:
		i++;
	}
}

/**
 * \brief Test active mode.
 */
static void test_active_mode(void)
{
	puts("Press SW0 to exit Active Mode\r");

	/* Configure button for exiting from active mode */
	configure_button();

	g_ul_button_pressed = 0;

	/* Test active mode */
	do {
		/* Run Fibonacci calculation, n = 10 (may be changed) */
		app_prime_number_run();
	} while (!g_ul_button_pressed);

	puts("Exit from Active Mode.\r");
}

static void test_wait_mode(void)
{
	puts("Press SW0 to exit Wait Mode\r");

	/* Configure button for exiting from wait mode */
	configure_button();

	sleepmgr_sleep(SLEEPMGR_WAIT);

	puts("Exit from Wait Mode.\r");
}

static void test_retention_mode(void)
{
	puts("Press SW0 to exit Retention Mode\r");

	/* Configure button for exiting from retention mode */
	configure_button();

	sleepmgr_sleep(SLEEPMGR_RET);

	puts("Exit from Retention Mode.\r");
}


static void test_backup_mode(void)
{
	puts("Press SW0 to exit Backup Mode\r");

	/* Configure button for exiting from backup mode */
	configure_button();

	// EIC can wake the device from backup mode
	bpm_enable_wakeup_source(BPM, BPM_BKUP_WAKEUP_SRC_EIC);
	// EIC can wake the device from backup mode
	bpm_enable_backup_pin(BPM, BPM_BKUP_WAKEUP_SRC_EIC);
	// Retain I/O lines after wakeup from backup
	bpm_disable_io_retention(BPM);
	bpm_enable_io_retention(BPM);
	bpm_enable_fast_wakeup(BPM);
	sysclk_disable_peripheral_clock(EIC);

	sleepmgr_sleep(SLEEPMGR_BACKUP);

	/* Note: The core will reset when exiting from backup mode. */
}


/**
 * \brief Test Core consumption.
 */
static void test_core(void)
{
	uint32_t uc_key = 0;

	bpm_configure_power_scaling(BPM,BPM_PMCON_PS(BPM_PS_1),true);
  while((bpm_get_status(BPM) & BPM_SR_PSOK) == 0);
	puts("Power Scaling Mode 1 selected !\r");

	while (1) {
		/* Display menu */
		display_menu_core();

		/* Read a key from console */
		while (usart_read(CONF_UART, &uc_key)) {
		}

		switch (uc_key) {
		/* Configuration */
		case '0':
			bpm_configure_power_scaling(BPM,BPM_PMCON_PS(BPM_PS_0),true);
			while((bpm_get_status(BPM) & BPM_SR_PSOK) == 0);
			puts("Power Scaling Mode 0 selected !\r");
			break;

		case '1':
			bpm_configure_power_scaling(BPM,BPM_PMCON_PS(BPM_PS_1),true);
			while((bpm_get_status(BPM) & BPM_SR_PSOK) == 0);
			puts("Power Scaling Mode 1 selected !\r");
		break;

		/* Mode test */
		case 'a':
		case 'A':
			test_active_mode();
			break;

		case 'r':
		case 'R':
			test_retention_mode();
			break;

		case 'w':
		case 'W':
			test_wait_mode();
			break;

		case 'b':
		case 'B':
			test_backup_mode();
			break;

		/* Quit test */
		case 'q':
		case 'Q':
			goto test_core_end;

		default:
			puts("This menu does not exist !\r");
			break;
		} /* Switch */
	}

test_core_end:
	puts(" Exit from core consumption test mode.\r");
}

/**
 * \brief Low power application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/*
	 * At startup the application run in full demo mode (all features on,
	 * includes QTouch and segment LCD). Initialize the board IO configuration,
	 * clocks, QTouch library, External interrupts, NVIC and UI SAM4L is running
	 * at 12 MHz from internal RCFAST (configured at 12MHz).
	 */
	// Initialize clock
	sysclk_init();

	// Initialize board features
	board_init();

	/* Configure the console uart */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Test core consumption */
	test_core();

	while(1u){
	}

}// end main function
