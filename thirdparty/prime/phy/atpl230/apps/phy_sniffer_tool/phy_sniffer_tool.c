/**
 * \file
 *
 * \brief ATMEL PLC Phy Sniffer Example
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

/**
 *  \mainpage ATMEL PLC Phy Sniffer Example
 *
 *  \section Purpose
 *
 *  The Phy Sniffer example uses the Phy layer to monitor data traffic and then
 * sends it via serial communications to the ATPL Multiprotocol Sniffer Tool.
 *
 *  \section Requirements
 *
 *  This package should be used with any PLC board on which there is PLC
 * hardware dedicated.
 *
 *  \section Description
 *
 *  This application will configure the PRIME PHY and its serial interface to
 * communicate with ATMEL ATPL Multiprotocol Sniffer Tool.
 *
 *  \section Usage
 *
 *  The tool is ready to monitor data traffic.
 *
 */

/* Atmel boards includes. */
#include "board.h"

/* Atmel library includes. */
#include "asf.h"

/* Function declarations */
static void prvSetupHardware(void);
void initTimer1ms(void);

#define COUNT_MS_SWAP_LED       500

static uint32_t ul_count_ms = COUNT_MS_SWAP_LED;
static bool b_led_swap = false;

#define ID_TC_1MS               ID_TC3
#define TC_1MS                  TC1
#define TC_1MS_CHN              0
#define TC_1MS_IRQn             TC3_IRQn
#define TC_1MS_Handler          TC3_Handler

#define STRING_EOL    "\r"
#define STRING_HEADER "-- ATMEL PLC Phy Tester Tool Application --\r\n"	\
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** @brief	Interrupt handler for Timer 3
 *
 * Handler for Timer 3 */
void TC_1MS_Handler(void)
{
	volatile uint32_t ul_dummy;
	/* Clear status bit to acknowledge interrupt */
	ul_dummy = tc_get_status(TC_1MS, TC_1MS_CHN);
	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/* update count ms */
	if (!ul_count_ms--) {
		ul_count_ms = COUNT_MS_SWAP_LED;
		b_led_swap = true;
	}
}

/** @brief	Init Timer interrupt (1ms)
 *
 * Initialize 1mSec timer 3 interrupt */
void initTimer1ms(void)
{
	uint32_t ul_div, ul_tcclks;

	/* Configure PMC */
	pmc_enable_periph_clk(ID_TC_1MS);

	/* MCK = 120000000 -> tcclks = 2 : TCLK3 = MCK/32 = 3750000 = 0.266us ->
	 * ul_div = 1ms/0.2666us = 3750 */
	ul_tcclks = 2;
	ul_div = 3750;
	tc_init(TC_1MS, TC_1MS_CHN, ul_tcclks | TC_CMR_CPCTRG);

	tc_write_rc(TC_1MS, TC_1MS_CHN, ul_div);

	/* Configure and enable interrupt on RC compare */
	NVIC_SetPriority((IRQn_Type)ID_TC_1MS, 0);
	NVIC_EnableIRQ((IRQn_Type)ID_TC_1MS);
	tc_enable_interrupt(TC_1MS, TC_1MS_CHN, TC_IER_CPCS);

	/** Start the timer. TC1, chanel 0 = TC3 */
	tc_start(TC_1MS, TC_1MS_CHN);
}

/**
 *  Configure UART console.
 */
/* [main_console_configure] */
static void configure_dbg_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONF_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Main code entry point.
 */
int main( void )
{
#ifdef CONF_BOARD_LCD_EN
	status_code_t status;
#endif

	/* count ms to blink led */
	ul_count_ms = 500;

	/* Prepare the hardware */
	prvSetupHardware();

	/* UART debug */
	configure_dbg_console();
	puts(STRING_HEADER);

#ifdef CONF_BOARD_LCD_EN
	/* Initialize the C42364A LCD glass component. */
	status = c42364a_init();
	if (status != STATUS_OK) {
		puts("-- LCD Initialization fails! --\r\n");
		while (1) {
		}
	}

	c42364a_set_contrast(15);
	c42364a_clear_all();
	c42364a_show_icon(C42364A_ICON_ATMEL);
	c42364a_show_icon(C42364A_ICON_USB);
	c42364a_show_text((const uint8_t *)"PHYSNF");
#endif

	/* Init process timers */
	initTimer1ms();

	/* Init Phy Layer */
	phy_init(SNIFFER_IF_ENABLE);

	/* Init Usi Layer */
	usi_init();

	while (1) {
		/* blink led 0 */
		if (b_led_swap) {
			b_led_swap = false;
#if (BOARD == SAM4CMP_DB || BOARD == SAM4CMS_DB)
			LED_Toggle(LED4);
#else
			LED_Toggle(LED0);
#endif
		}

		/* updWatchDog (); */

		/* sniffer serialization */
		sniffer_if_process();

		/* USI */
		usi_process();
	}
}

/**
 * \brief Configure the hardware.
 */
static void prvSetupHardware(void)
{
	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();
}
