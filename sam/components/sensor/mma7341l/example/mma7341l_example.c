/**
 * \file
 *
 * \brief MMA7341L Accelerometer example.
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
 * \mainpage MMA7341L Accelerometer Example
 *
 * \section Purpose
 *
 * This example demonstrates how to configure MMA7341L accelerometer
 * and check the gravity values of three axes by it.
 *
 * \section Requirements
 *
 * This package can be used with SAM evaluation kits.
 *
 * \section Description
 *
 * This example first set MMA7341L working mode and g-range, then initialize and calibrate it,
 * and last get the gravity values of three axes.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# In the terminal window, the
 *     following text should appear (values depend on the board and the chip used):
 *     \code
	-- MMA7341L Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- MMA7341L  Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Global timestamp in milliseconds since start of application */
volatile uint32_t ul_ms_ticks = 0;

/**
 * \brief Systick handler.
 */
void SysTick_Handler(void)
{
	ul_ms_ticks++;
}

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 *  \brief Wait for the given number of milliseconds (using the dwTimeStamp generated
 *  by the SAM microcontrollers' system tick).
 *
 *  \param dw_dly_ticks Delay to wait for, in milliseconds.
 */
static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = ul_ms_ticks;
	while ((ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

/**
 * \brief Application entry point for MMA7341L accelerometer example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	xyz_g_t xyz_g;

	sysclk_init();
	board_init();

	/* Configure console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	puts("Configure system tick to get 1ms tick period.\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
		while (1);
	}

	/* Set MMA7341L normal mode */
	mma7341l_set_mode(MMA7341L_NORMAL_MODE);

	/* Initialize MMA7341L */
	mma7341l_init();

	/* Calibrate MMA7341L */
	mma7341l_calibration();

	while (1) {
		xyz_g = mma7341l_get_gravity_value();
		printf("X-g = %f g, Y-g = %f g, Z-g = %f g\n", xyz_g.x_g, xyz_g.y_g, xyz_g.z_g);
		mdelay(200);
	}
}
