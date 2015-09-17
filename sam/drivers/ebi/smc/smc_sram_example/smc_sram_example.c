/**
 * \file
 *
 * \brief SMC SRAM Example for SAM.
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
 * \mainpage SMC SRAM Example
 *
 * \section Purpose
 *
 * This example shows how to configure the Static Memory Controller (SMC)
 * for SRAM, and do write and read operations to check the configuration.
 *
 * \section Requirements
 *
 * This package can be used with SAM evaluation kits.
 *
 * \section Description
 *
 * An accurate one-to-one comparison is necessary between SRAM and SMC waveforms for
 * a complete SMC configuration.
 * * The required steps are:
 * <ul>
 * <li> Configure EBI I/O for SRAM connection </li>
 * <li> Configure PMC to enable the SMC clock.</li>
 * <li> Refer to the "AC Characteristics" section of the customer SRAM datasheet.</li>
 * <li> Configure SMC timing to fix the characteristics</li>
 * <li>	Configure SMC mode register for page mode if any.</li>
 * <li>	Access the SRAM for write or read.</li>
 * </ul>
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g. HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 *    \code
	-- SMC SRAM Example --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"

/** SRAM test size */
#define SRAM_TEST_SIZE  (10*1024)

/**
 * \brief Go/No-Go test of the first 10K-Bytes of external SRAM access.
 *
 * \param base_addr  Base address of the SRAM.
 *
 * \return 0 if test is failed else 1.
 */
static uint8_t access_sram_test(uint32_t base_addr)
{
	uint32_t i;
	uint8_t *ptr = (uint8_t *) (base_addr);

#ifdef PIN_EBI_NLB
	/* Pull down LB, enable sram access */
	pio_set_pin_low(PIN_EBI_NLB);
#endif

	/* Clear test data area */
	for (i = 0; i < SRAM_TEST_SIZE; ++i) {
		ptr[i] = 0xff;
	}

	for (i = 0; i < SRAM_TEST_SIZE; ++i) {
		if (i & 1) {
			ptr[i] = ((0x5a & i) & 0xff);
		} else {
			ptr[i] = ((0xa5 & i) & 0xff);
		}
	}

	for (i = 0; i < SRAM_TEST_SIZE; ++i) {
		if (i & 1) {
			if (ptr[i] != ((0x5a & i) & 0xff)) {
				return 0;
			}
		} else {
			if (ptr[i] != ((0xa5 & i) & 0xff)) {
				return 0;
			}
		}
	}

#ifdef PIN_EBI_NLB
	/* Pull up LB, SRAM standby */
	pio_set_pin_high(PIN_EBI_NLB);
#endif

	return 1;
}

/**
 *  Configure UART console.
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
 * \brief Configure the SMC for SRAM access.
 *
 * \param cs  Chip select.
 */
static void configure_sram(uint32_t cs)
{
	smc_set_setup_timing(SMC, cs, SMC_SETUP_NWE_SETUP(1)
			| SMC_SETUP_NCS_WR_SETUP(1)
			| SMC_SETUP_NRD_SETUP(1)
			| SMC_SETUP_NCS_RD_SETUP(1));
	smc_set_pulse_timing(SMC, cs, SMC_PULSE_NWE_PULSE(6)
			| SMC_PULSE_NCS_WR_PULSE(6)
			| SMC_PULSE_NRD_PULSE(6)
			| SMC_PULSE_NCS_RD_PULSE(6));
	smc_set_cycle_timing(SMC, cs, SMC_CYCLE_NWE_CYCLE(7)
			| SMC_CYCLE_NRD_CYCLE(7));
	smc_set_mode(SMC, cs, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE);
}

/**
 * \brief Application entry point for smc_sram example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	configure_console();

	/* Output example information */
	printf("\n\r\n\r\n\r");
	printf("-- SMC SRAM Example --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	/* Configure EBI I/O for SRAM connection */
	printf("Configure EBI I/O for SRAM connection.\n\r");

	/* Enable PMC clock for SMC */
	pmc_enable_periph_clk(ID_SMC);

	/* SMC configuration between SRAM and SMC waveforms. */
	configure_sram(SRAM_CHIP_SELECT);
#ifdef SRAM_CHIP_SELECT_2ND
	configure_sram(SRAM_CHIP_SELECT_2ND);
#endif

#ifdef PIN_EBI_NLB
	/* Configure LB, enable SRAM access */
	pio_configure_pin(PIN_EBI_NLB, PIN_EBI_NLB_FLAGS);
#endif

	/* Test external SRAM access */
	printf("Test external SRAM access. \n\r");
	if (access_sram_test(SRAM_BASE_ADDRESS)) {
		printf("SRAM access successful.\n\r");
	} else {
		printf("SRAM access failed.\n\r");
	}
#ifdef SRAM_CHIP_SELECT_2ND
	if (access_sram_test(SRAM_BASE_ADDRESS_2ND)) {
		printf("2nd SRAM access successful.\n\r");
	} else {
		printf("2nd SRAM access failed.\n\r");
	}
#endif
}
