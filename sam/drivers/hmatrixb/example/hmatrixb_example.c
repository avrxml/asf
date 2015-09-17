/**
 * \file
 *
 * \brief HMATRIX example for SAM.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
 * \mainpage HMATRIX Example
 *
 * \section Purpose
 *
 * This example demonstrates the HSB BUS Matrix (HMATRIX) provided on
 * SAM microcontrollers.
 *
 * \section Requirements
 *
 * This example can be used on any SAM4L boards.
 *
 * \section Description
 *
 * This example shows running speed between two matrix configurations.
 * The LED toggle times in one second is measured.
 * The first test is done with Round-Robin arbitration without default master.
 * The second test is done with Round-Robin arbitration with last access master.
 * As expected, the LED toggle times of test1 is lower than that of test2.
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
	-- HMATRIX Example --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	Configure system tick to get 1ms tick period.
	-- Test1: configure Round-Robin arbitration without default master. --
	    Led toggled xxx times in one second
	-- Test2: configure Round-Robin arbitration with last access master. --
	    Led toggled xxx times in one second
\endcode
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- HMATRIX Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Global g_ul_ms_ticks in milliseconds since start of application */
volatile uint32_t g_ul_ms_ticks = 0;

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

	/* Configure console. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Toggle led at the given time.
 *
 * \param ul_dly_ticks  Delay to wait for, in milliseconds.
 *
 * \return Led toggle times.
 */
static uint32_t toggle_led_test(uint32_t ul_dly_ticks)
{
	uint32_t ul_cnt = 0;
	uint32_t ul_cur_ticks;

	ul_cur_ticks = g_ul_ms_ticks;
	do {
		ul_cnt++;
		LED_Toggle(LED0);
	} while ((g_ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);

	return ul_cnt;
}

/**
 * \brief Handler for System Tick interrupt.
 *
 * Process System Tick Event.
 * Increment the g_ul_ms_ticks counter.
 */
void SysTick_Handler(void)
{
	g_ul_ms_ticks++;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	enum hmatrix_slave_id ul_slave_id;
	uint32_t ul_cnt;
	struct hmatrix_slave_ch_conf config;

	/* Initialize the system */
	sysclk_init();
	board_init();

	/* Configure console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure systick for 1 ms */
	puts("Configure system tick to get 1ms tick period.\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
		while (1) {
		}
	}

	/* Enable the HMATRIX access. */
	hmatrix_enable();
	hmatrix_slave_ch_get_config_defaults(&config);

	/* First, test with Round-Robin arbitration without default master */
	puts("-- Test1: configure Round-Robin arbitration without default master. --\r");
	config.def_master_type = HMATRIX_DEFAULT_MASTER_NONE;
	for (ul_slave_id = HMATRIX_SLAVE_ID_FLASH;
			ul_slave_id < HMATRIX_SLAVE_ID_NUM; ul_slave_id++) {
		hmatrix_slave_ch_set_config(ul_slave_id, &config);
	}
	ul_cnt = toggle_led_test(1000);
	printf("    Led toggled %ld times in one second\n\r", (long)ul_cnt);

	/* Second, test with Round-Robin arbitration with last access master */
	puts("-- Test2: configure Round-Robin arbitration with last access master. --\r");
	config.def_master_type = HMATRIX_DEFAULT_MASTER_PREVIOUS;
	for (ul_slave_id = HMATRIX_SLAVE_ID_FLASH;
			ul_slave_id < HMATRIX_SLAVE_ID_NUM; ul_slave_id++) {
		hmatrix_slave_ch_set_config(ul_slave_id, &config);
	}
	ul_cnt = toggle_led_test(1000);
	printf("    Led toggled %ld times in one second\n\r", (long)ul_cnt);

	/* Endless loop */
	while (1) {
	}
}
