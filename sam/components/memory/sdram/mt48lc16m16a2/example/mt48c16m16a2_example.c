/**
 * \file
 *
 * \brief SDRAMC on EBI example for SAM.
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
 *
 * \mainpage SDRAM Example for MT48LC16M16A2
 *
 * \section Purpose
 *
 * This example shows how to configure the MT48LC16M16A2.
 * It will perform write and read operations to check the configuration.
 *
 * \section Description
 *
 * An accurate one-to-one comparison is processed after the complete SDRAM
 * configuration.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR EWARM.
 * Other compilers may or may not work.
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
 * -# In the terminal window, the following text should appear
 *    (values depend on the board and the chip used):
 *    \code
	-- SDRAMC Example --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	...
\endcode
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "conf_board.h"

/** SDRAM benchmark test size */
#define SDRAMC_TEST_BUFF_SIZE        (30 * 1024)

/** SDRAM benchmark test page number */
#define SDRAMC_TEST_PAGE_NUM        (1024)

/** SDRAMC access test length */
#define SDRAMC_TEST_LENGTH        (10 * 1024)

/** SDRAMC access test data in even address */
#define SDRAMC_TEST_EVEN_TAG        (0x55aaaa55)

/** SDRAMC access test data in odd address */
#define SDRAMC_TEST_ODD_TAG        (0xaa5555aa)

#define STRING_EOL    "\r"
#define STRING_HEADER "-- SDRAMC Example --\r\n" \
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** SDRAM benchmark buffer */
static volatile uint16_t gs_pus_mem_buff[SDRAMC_TEST_BUFF_SIZE];

/* Ticks in 1ms */
static volatile uint32_t gs_ul_ms_ticks = 0;

/** SDRAMC status */
typedef enum {
	SDRAMC_OK = 0,
	SDRAMC_TIMEOUT = 1,
	SDRAMC_ERROR = 2,
} sdramc_status_t;

/**
 * \brief Systick handler to manage timestamp and delay.
 */
void SysTick_Handler(void)
{
	gs_ul_ms_ticks++;
}

/**
 *  \brief Get current tick count (in ms).
 *
 * \return Current tick count value.
 */
static uint32_t tick_get_count(void)
{
	return gs_ul_ms_ticks;
}

/**
 * \brief Get delayed number of ticks.
 *
 * \param ul_start_tick Start tick point.
 * \param ul_end_tick   End tick point.
 *
 * \return Delayed number of ticks.
 */
static uint32_t get_delay_in_ticks(uint32_t ul_start_tick, uint32_t ul_end_tick)
{
	if (ul_end_tick > ul_start_tick) {
		return (ul_end_tick - ul_start_tick);
	}

	return (ul_end_tick + (0xFFFFFFFF - ul_start_tick));
}

/**
 * \brief Access test of the first 10K bytes of external SDRAM.
 *
 * \return SDRAMC_OK if test is passed, else SDRAMC_ERROR.
 */
static uint8_t sdram_access_test(void)
{
	uint32_t i;
	uint32_t *pul = (uint32_t *)BOARD_SDRAM_ADDR;

	for (i = 0; i < SDRAMC_TEST_LENGTH; ++i) {
		if (i & 1) {
			pul[i] = SDRAMC_TEST_ODD_TAG | (1 << i);
		} else {
			pul[i] = SDRAMC_TEST_EVEN_TAG | (1 << i);
		}
	}

	for (i = 0; i < SDRAMC_TEST_LENGTH; ++i) {
		if (i & 1) {
			if (pul[i] != (SDRAMC_TEST_ODD_TAG | (1 << i))) {
				return SDRAMC_ERROR;
			}
		} else {
			if (pul[i] != (SDRAMC_TEST_EVEN_TAG | (1 << i))) {
				return SDRAMC_ERROR;
			}
		}
	}

	return SDRAMC_OK;
}

/**
 * \brief SDRAM benchmark test.
 *
 * \return SDRAMC_OK for success and SDRAMC_ERROR for error.
 */
static uint32_t sdram_benchmarks(void)
{
	uint16_t i;
	uint16_t *pus = (uint16_t *)BOARD_SDRAM_ADDR;
	uint32_t ul_tick_start, ul_tick_end, ul_rw_speed;
	uint32_t ul_ticks = 0;
	uint32_t ul_page_num = 0;

	puts("Start SDRAMC benchmarks.\r");

	for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
		gs_pus_mem_buff[i] = (uint16_t)(i & 0xffff);
	}

	/* Start SDRAM write test */
	ul_tick_start = tick_get_count();
	for (ul_page_num = 0; ul_page_num < SDRAMC_TEST_PAGE_NUM;
			ul_page_num++) {
		for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
			pus[i + ul_page_num *
			SDRAMC_TEST_BUFF_SIZE]
				= gs_pus_mem_buff[i];
		}
	}
	ul_tick_end = tick_get_count();
	ul_ticks = get_delay_in_ticks(ul_tick_start, ul_tick_end);
	ul_rw_speed = SDRAMC_TEST_BUFF_SIZE * SDRAMC_TEST_PAGE_NUM *
			sizeof(uint16_t) / ul_ticks;
	printf("SDRAMC write speed: %uK/s\n\r", (uint32_t)ul_rw_speed);

	/* Start SDRAM read test */
	ul_tick_start = tick_get_count();
	for (ul_page_num = 0; ul_page_num < SDRAMC_TEST_PAGE_NUM;
			ul_page_num++) {
		for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
			gs_pus_mem_buff[i]
				= pus[i + ul_page_num *
					SDRAMC_TEST_BUFF_SIZE];
		}
	}
	ul_tick_end = tick_get_count();
	ul_ticks = get_delay_in_ticks(ul_tick_start, ul_tick_end);
	ul_rw_speed = SDRAMC_TEST_BUFF_SIZE * SDRAMC_TEST_PAGE_NUM *
			sizeof(uint16_t) / ul_ticks;
	printf("SDRAMC read speed: %uK/s\n\r", (uint32_t)ul_rw_speed);

	for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
		if (gs_pus_mem_buff[i] != (i & 0xffff)) {
			puts("-F- SDRAMC read data error\r");
			return SDRAMC_ERROR;
		}
	}

	return SDRAMC_OK;
}

/**
 *  \brief Configure the console uart.
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
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#endif
}

/**
 * \brief Application entry point for sdramc_example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the system */
	sysclk_init();
	board_init();
	sleepmgr_init();

	/* Configure the console uart */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Systick configuration */
	if (SysTick_Config(SystemCoreClock / (1000))) {
		puts("-F- Systick configuration error.\r");
	}

	/* Enable SMC peripheral clock */
	pmc_enable_periph_clk(ID_SMC);

	/* Complete SDRAM configuration */
	sdramc_init((sdramc_memory_dev_t *)&SDRAM_MICRON_MT48LC16M16A2,
			sysclk_get_cpu_hz());

	/* Test external SDRAM access */
	puts("Test external SDRAM access. \r");

	if (sdram_access_test() == SDRAMC_OK) {
		puts("SDRAM access is successful.\n\r");
	} else {
		puts("SDRAM access is failed.\r");
	}

	if (sdram_benchmarks() == SDRAMC_OK) {
		puts("SDRAM test: pass.\r");
	}

	for (;;) {
		sleepmgr_enter_sleep();
	}
}
