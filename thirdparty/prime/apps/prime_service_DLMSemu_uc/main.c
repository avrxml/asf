/**
 * \file
 *
 * \brief DLMS Application Emulator for ATMEL PRIME v.1.3 Service Node
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
 *  \mainpage ATMEL PRIME DLMS Application Emulator for Service Node
 *
 *  \section Purpose
 *
 *  This application provides the service node with DLMS emulation capabilities.
 *
 *  \section Requirements
 *
 *  This package should be used with SAM4CP16BMB board, on which there is
 * dedicated PLC hardware.
 *
 *  \section Description
 *
 *  This application will configure the PRIME stack and its serial interface to
 * use PHY, MAC and IEC_432 layers as Service Node. It will also implement a DLMS
 * application emulator that will interact with a base node with DLMS capabilities.
 *
 *  \section Usage
 *
 *  -# Build the program and download it into the evaluation board. Please
 *     refer to the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *     SAM-BA User Guide</a> or the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *     GNU-Based Software Development</a>
 *     application note depending on the solutions that users choose.
 *     \endcode
 *  -# The application will start PRIME standard as Service Node mode.
 *	-# The application is configured to serialize several protocols with
 * these settings:
 *    - Uart0 Serial port
 *	  - 115200 bauds
 *    - TX buffer: 1024 bytes, Rx buffer: 1024 bytes
 *    - Serialized protocols in use:
 *              - Prime Management
 *
 */

/* Atmel library includes. */
#include "asf.h"
/* Atmel boards includes. */
#include "board.h"
/* Configuration includes. */
#include "conf_board.h"
#include "conf_example.h"

/* DLMS emu  includes. */
#include "dlms_emu.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- ATMEL DLMS EMU Application for Service Node[uC]--\r\n" \
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

#define ID_TC_1MS               ID_TC3
#define TC_1MS                  TC1
#define TC_1MS_CHN              0
#define TC_1MS_IRQn             TC3_IRQn
#define TC_1MS_Handler          TC3_Handler

#define SIGNALLING_PERIOD_MS	250
static bool b_signalling_en;
static uint16_t us_signal_period;

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

	/* update signalling ms */
	if (!us_signal_period--) {
		us_signal_period = SIGNALLING_PERIOD_MS;
		b_signalling_en = true;
	}

	/* Update internal counters of Prime Stack */
	prime_upd1ms();
	sscs432_upd1ms();
	usi_txrx_block_timer();
}

/** @brief	Init Timer interrupt (1ms)
 *
 * Initialize 1mSec timer 3 interrupt */
static void initTimer1ms(void)
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
 * \brief Configure the hardware.
 */
static void prvSetupHardware(void)
{
#ifdef EXAMPLE_LCD_SIGNALLING_ENABLE
	status_code_t status;
#endif

	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();

	/* Init timers */
	initTimer1ms();

	/* Init Prime Hardware Abstraction Layer */
	hal_init();
	hal_start();

#ifdef EXAMPLE_LCD_SIGNALLING_ENABLE
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
	c42364a_show_icon(C42364A_ICON_WLESS);
	c42364a_show_text((const uint8_t *)"SERV  ");
#endif
}

#ifdef EXAMPLE_LCD_SIGNALLING_ENABLE

/**
 * \internal
 * \brief Function to blink a symbol or led.
 * \note Please see conf_example file in order to configure the signalling.
 *
 */
static uint8_t _blink_symbol(uint8_t icon_com, uint8_t icon_seg, uint8_t status)
{
	if (!status) {
		c42364a_show_icon(icon_com, icon_seg);
		return true;
	} else {
		c42364a_clear_icon(icon_com, icon_seg);
		return false;
	}
}

#endif

/**
 * \internal
 * \brief Periodic task to provide visual feedback that the system status.
 * \note Please see conf_example file in order to configure the signalling.
 *
 */
extern uint8_t macPLCState;
#ifdef EXAMPLE_LCD_SIGNALLING_ENABLE
static uint8_t uc_blink_status;
#endif
static void _prime_signalling(void)
{
#if BOARD == SAM4CMP_DB
	LED_Toggle(LED4);
#elif BOARD == SAM4CMS_DB
	LED_Toggle(LED4);
#elif BOARD == SAM4C_EK
	LED_Toggle(LED0);
#else
	LED_Toggle(LED0);
#endif

#ifdef EXAMPLE_LCD_SIGNALLING_ENABLE
	switch (macPLCState) {
	case 0:          /* DISCONNECTED */
		uc_blink_status = _blink_symbol(C42364A_ICON_WLESS,
										uc_blink_status);
		c42364a_show_text((const uint8_t *)"SN DIS");
		break;

	case 1:          /* DETECTION */
		uc_blink_status = _blink_symbol(C42364A_ICON_WLESS,
										uc_blink_status);
		c42364a_show_text((const uint8_t *)"SN BCN");
		break;

	case 2:          /* REGISTERING */
		uc_blink_status = _blink_symbol(C42364A_ICON_WLESS,
										uc_blink_status);
		c42364a_show_text((const uint8_t *)"SN REQ");
		break;

	case 3:          /* OPERATIVE */
		c42364a_show_icon(C42364A_ICON_WLESS);
		c42364a_show_text((const uint8_t *)"SN REG");
		break;
	}
#endif
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
	/* Prepare the hardware */
	prvSetupHardware();

	/* Configure console */
	configure_dbg_console();
	puts(STRING_HEADER);

	/* Init signalling */
	b_signalling_en = false;
	us_signal_period = SIGNALLING_PERIOD_MS;

	/* Init Prime Stack */
	phy_init(DISABLE_SERIAL);
	pal_init();
	prime_init();
	sscs432_init();
	usi_init();
	prime_start();
	sscs432_start();

#ifdef PRIME_DISABLE_PRM
	prime_MLME_SET_request(PIB_MAC_PRM_ACTIVATION, 0);
#endif
#ifdef PRIME_ENABLE_EMB_SNIFFER
	prime_MLME_SET_request(PIB_MAC_SNIFFER_ACTIVATION, 1);
#endif

	/* Init the DLMS Emulation task. */
	dlms_eum_init();

	while (1) {
		/* Run Prime Stack */
		pal_process();
		prime_process();
		sscs432_process();
		usi_process();

		/* Run DLMS Emulation task */
		dlms_eum_process();

		if(b_signalling_en){
			b_signalling_en = false;
			_prime_signalling();
		}
	}
}
