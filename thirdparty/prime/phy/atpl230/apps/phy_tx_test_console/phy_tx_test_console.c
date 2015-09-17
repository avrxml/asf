/**
 * \file
 *
 * \brief ATMEL PLC PHY TX Test Console Application
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
 *  \mainpage ATMEL PLC PHY TX Test Console Application
 *
 *  \section Purpose
 *
 *  The PHY TX Test Console Application demonstrates how to configure some
 * parameters from the PHY layer on PLC boards.
 *
 *  \section Requirements
 *
 *  This package should be used with any PLC board on which there is PLC
 *hardware dedicated.
 *
 *  \section Description
 *
 *  This application can configure the PHY with a serial interface and test
 * PLC transmission/reception processes.
 *
 *  \section Usage
 *
 *  The tool is ready to configure, transmit and receive.
 */

#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Atmel boards includes. */
#include "board.h"

/* Atmel library includes. */
#include "asf.h"
#include "math.h"

/* PRIME includes */
#include "atpl230.h"
#include "atpl230_version.h"
#include "conf_atpl230.h"
#include "phy_embedded_app.h"

/* Example Includes */
#include "conf_example.h"

/* Function declarations */
static void prvSetupHardware(void);

/* Task Monitor parameters */
#define mainMONITOR_TIMER_RATE    (500 / portTICK_RATE_MS)
#define mainMONITOR_BLOCK_TIME    (1000 / portTICK_RATE_MS)
static void prvProcessMonitorTasks(xTimerHandle pxTimer);

/* WDT configuration */
#define WATCHDOG_1s                     1000000
#define WATCHDOG_5s                     5000000

/* FreeRTOS utils */
void vApplicationIdleHook( void );
void vApplicationMallocFailedHook( void );
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
void vApplicationTickHook( void );

#define STRING_EOL    "\r"
#define STRING_HEADER "-- ATMEL PLC Getting Started Application --\r\n"	\
	"-- "BOARD_NAME " --\r\n" \
	"-- Compiled: "__DATE__ " "__TIME__ " --\r\n" \
	"-- PHY version: "ATPL230_VERSION_STR " --\r\n"

#define MENU_HEADER "\n\r-- Menu Configuration --------------\n\r" \
	"0: Select buffer to transmit\n\r" \
	"1: Select attenuation level\n\r" \
	"2: Select scheme to transmit\n\r" \
	"3: Disable Rx in transmission\n\r" \
	"4: Select mode to transmit\n\r" \
	"5: Select time period between messages to transmit(ms.)\n\r" \
	"6: Enter data to transmit\n\r"	\
	"7: Select channel to transmit\n\r" \
	"8: Select coupling board in use\n\r" \
	"9: Config Auto-Detect impedance \n\r" \
	"v: View TX configuration values\n\r" \
	"e: Execute transmission application\n\r" \
	"otherwise: Display this main menu\n\n\r"

#define MENU_SCHEME "\n\r-- Modulation Scheme --------------\r\n" \
	"0: PROTOCOL_DBPSK\n\r"	\
	"1: PROTOCOL_DQPSK\n\r"	\
	"2: PROTOCOL_D8PSK\n\r"	\
	"3: PROTOCOL_DBPSK_VTB\n\r" \
	"4: PROTOCOL_DQPSK_VTB\n\r" \
	"5: PROTOCOL_D8PSK_VTB\n\r" \
	"6: PROTOCOL_DBPSK_ROBO\n\r" \
	"7: PROTOCOL_DQPSK_ROBO\n\r"

#define MENU_COUPLING "\n\r-- Coupling Board --------------\r\n" \
	"0: ATPLCOUP000_v1\n\r"	\
	"1: ATPLCOUP000_v2\n\r"	\
	"2: ATPLCOUP001_v1\n\r"	\
	"3: ATPLCOUP002_v1\n\r"	\
	"4: ATPLCOUP002_v2\n\r"	\
	"5: ATPLCOUP003_v1\n\r"	\
	"6: ATPLCOUP004_v1\n\r"	\
	"7: ATPLCOUP005_v1\n\r"	\
	"8: ATPLCOUP006_v1\n\r"

#define MENU_MODE "\n\r-- PRIME Mode --------------\r\n" \
	"0: MODE_TYPE_A\n\r" \
	"2: MODE_TYPE_B\n\r" \
	"3: MODE_TYPE_BC\n\r"

#if (SAM4C)
#define MENU_DATA_MODE "\n\r-- Select Data Mode --------------\r\n" \
	"0: Random Data\n\r" \
	"1: Fixed Data\n\r" \
	"2: Manual Data\n\r"
#else
#define MENU_DATA_MODE "\n\r-- Select Data Mode --------------\r\n" \
	"1: Fixed Data\n\r" \
	"2: Manual Data\n\r"
#endif

#define MENU_IMPEDANCE "\n\r-- Autodetect Impedance Mode --------------\r\n" \
	"0: Autodetect mode\n\r" \
	"1: Fixed Mode. High Impedance\n\r" \
	"2: Fixed Mode. Low Impedance\n\r" \
	"3: Fixed Mode. Vlow Impedance\n\r"

#define MENU_CONSOLE "\n\rPHY-Console>"

/* Phy data configuration */
static txPhyEmbeddedConfig_t xAppPhyCfgTx;

/* Tx data buffer */
uint8_t ucv_tx_data_buffer[512];

/**
 *  Configure UART console.
 */
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
 * Fix impedance mode
 */
static void fix_impedance_mode(uint8_t uc_impedance)
{
	/* Disable autodetect mode */
	xAppPhyCfgTx.uc_autodetect = false;
	/* Config impedance to use. */
	xAppPhyCfgTx.uc_impedance = uc_impedance;
}

/**
 * Auto impedance mode
 */
static void auto_impedance_mode(void)
{
	/* Enable autodetect mode */
	xAppPhyCfgTx.uc_autodetect = true;
	/* Config impedance to use. Hi state by default */
	xAppPhyCfgTx.uc_impedance = HI_STATE;
}

/**
 * Get Autodetect Impedance mode.
 */
static void get_impedance_mode(void)
{
	uint8_t uc_char;

	puts(MENU_IMPEDANCE);

	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '0':
			auto_impedance_mode();
			printf("Autodetec mode selected\r\n");
			break;

		case '1':
			fix_impedance_mode(HI_STATE);
			printf("Fix high impedance mode selected\r\n");
			break;

		case '2':
			fix_impedance_mode(LO_STATE);
			printf("Fix low impedance mode selected\r\n");
			break;

		case '3':
			fix_impedance_mode(VLO_STATE);
			printf("Fix very low impedance mode selected\r\n");
			break;

		default:
			continue;
		}
		printf(MENU_CONSOLE);
		fflush(stdout);
		break;
	}
}

/**
 * Set configuration parameters in GPBR
 */
static void save_config(uint8_t cmd_start_mode)
{
	uint32_t ul_gpbr_value;

	ul_gpbr_value = cmd_start_mode;
	ul_gpbr_value += xAppPhyCfgTx.uc_coupling << 4;
	ul_gpbr_value += xAppPhyCfgTx.uc_channel << 12;
	ul_gpbr_value += xAppPhyCfgTx.xPhyMsg.uc_buff_id << 16;
	ul_gpbr_value += xAppPhyCfgTx.xPhyMsg.att_level << 20;
	ul_gpbr_value += xAppPhyCfgTx.xPhyMsg.scheme << 25;
	ul_gpbr_value += xAppPhyCfgTx.xPhyMsg.disable_rx << 28;
	ul_gpbr_value += xAppPhyCfgTx.xPhyMsg.mode << 29;
	gpbr_write(GPBR0, ul_gpbr_value);
	gpbr_write(GPBR1, xAppPhyCfgTx.ul_tx_period);
	ul_gpbr_value = xAppPhyCfgTx.xPhyMsg.data_len;
	ul_gpbr_value += xAppPhyCfgTx.uc_autodetect << 16;
	ul_gpbr_value += xAppPhyCfgTx.uc_impedance << 20;
	gpbr_write(GPBR2, ul_gpbr_value);
}

/**
 * Get configuration parameters from GPBR
 */
static uint8_t load_config(void)
{
	uint32_t uc_gpbr_value;
	uint8_t uc_start_mode;

	uc_gpbr_value = gpbr_read(GPBR0);
	uc_start_mode = uc_gpbr_value & 0x0F;
	if ((uc_start_mode == PHY_APP_CMD_MENU_START_MODE) || \
			(uc_start_mode == PHY_APP_CMD_TX_START_MODE)) {
		xAppPhyCfgTx.uc_coupling = (uc_gpbr_value >> 4) & 0xFF;
		xAppPhyCfgTx.uc_channel = (uc_gpbr_value >> 12) & 0x0F;
		xAppPhyCfgTx.xPhyMsg.uc_buff_id = (uc_gpbr_value >> 16) & 0x0F;
		xAppPhyCfgTx.xPhyMsg.att_level = (uc_gpbr_value >> 20) & 0x1F;
		xAppPhyCfgTx.xPhyMsg.scheme = (uc_gpbr_value >> 25) & 0x07;
		xAppPhyCfgTx.xPhyMsg.disable_rx = (uc_gpbr_value >> 28) & 0x01;
		xAppPhyCfgTx.xPhyMsg.mode = (uc_gpbr_value >> 29) & 0x07;

		xAppPhyCfgTx.ul_tx_period = gpbr_read(GPBR1);
		uc_gpbr_value = gpbr_read(GPBR2);
		xAppPhyCfgTx.xPhyMsg.data_len = uc_gpbr_value & 0xFFFF;
		xAppPhyCfgTx.uc_autodetect = (uc_gpbr_value >> 16) & 0x0F;
		xAppPhyCfgTx.uc_impedance = (uc_gpbr_value >> 20) & 0x0F;

		/* upload the content of data message from flash memroy */
		memcpy(ucv_tx_data_buffer, (uint8_t *)ADDR_APP_PHY_MESSAGE_DATA, \
				xAppPhyCfgTx.xPhyMsg.data_len );
	} else {
		uc_start_mode = PHY_APP_CMD_DEFAULT_MODE;
	}

	return uc_start_mode;
}

/**
 * Display current information
 */
static void display_config(void)
{
	printf("\n\r-- Configuration Info --------------\r\n");
	switch (xAppPhyCfgTx.uc_coupling) {
	case ATPLCOUP000_v1:
		printf("-I- Coupling Board: ATPLCOUP000_v1\n\r");
		break;

	case ATPLCOUP000_v2:
		printf("-I- Coupling Board: ATPLCOUP000_v2\n\r");
		break;

	case ATPLCOUP001_v1:
		printf("-I- Coupling Board: ATPLCOUP001_v1\n\r");
		break;

	case ATPLCOUP002_v1:
		printf("-I- Coupling Board: ATPLCOUP002_v1\n\r");
		break;

	case ATPLCOUP002_v2:
		printf("-I- Coupling Board: ATPLCOUP002_v2\n\r");
		break;

	case ATPLCOUP003_v1:
		printf("-I- Coupling Board: ATPLCOUP003_v1\n\r");
		break;

	case ATPLCOUP004_v1:
		printf("-I- Coupling Board: ATPLCOUP004_v1\n\r");
		break;

	case ATPLCOUP005_v1:
		printf("-I- Coupling Board: ATPLCOUP005_v1\n\r");
		break;

	case ATPLCOUP006_v1:
		printf("-I- Coupling Board: ATPLCOUP006_v1\n\r");
		break;

	default:
		printf("-I- Coupling Board: ERROR CFG\n\r");
	}
	printf("-I- Tx Channel: %u\n\r", (uint32_t)xAppPhyCfgTx.uc_channel);
	printf("-I- Buffer: %u\n\r", (uint32_t)xAppPhyCfgTx.xPhyMsg.uc_buff_id);
	printf("-I- Attenuation Level: %u\n\r", (uint32_t)xAppPhyCfgTx.xPhyMsg.att_level);
	switch (xAppPhyCfgTx.xPhyMsg.scheme) {
	case PROTOCOL_DBPSK:
		printf("-I- Modulation Scheme: PROTOCOL_DBPSK\n\r");
		break;

	case PROTOCOL_DQPSK:
		printf("-I- Modulation Scheme: PROTOCOL_DQPSK\n\r");
		break;

	case PROTOCOL_D8PSK:
		printf("-I- Modulation Scheme: PROTOCOL_D8PSK\n\r");
		break;

	case PROTOCOL_DBPSK_VTB:
		printf("-I- Modulation Scheme: PROTOCOL_DBPSK_VTB\n\r");
		break;

	case PROTOCOL_DQPSK_VTB:
		printf("-I- Modulation Scheme: PROTOCOL_DQPSK_VTB\n\r");
		break;

	case PROTOCOL_D8PSK_VTB:
		printf("-I- Modulation Scheme: PROTOCOL_D8PSK_VTB\n\r");
		break;

	case PROTOCOL_DBPSK_ROBO:
		printf("-I- Modulation Scheme: PROTOCOL_DBPSK_ROBO\n\r");
		break;

	case PROTOCOL_DQPSK_ROBO:
		printf("-I- Modulation Scheme: PROTOCOL_DQPSK_ROBO\n\r");
		break;

	default:
		printf("-I- Modulation Scheme: ERROR CFG\n\r");
	}
	printf("-I- Disable Rx: %u\n\r", (uint32_t)xAppPhyCfgTx.xPhyMsg.disable_rx);
	switch (xAppPhyCfgTx.xPhyMsg.mode) {
	case MODE_TYPE_A:
		printf("-I- PRIME mode: MODE_TYPE_A\n\r");
		break;

	case MODE_TYPE_B:
		printf("-I- PRIME mode: MODE_TYPE_B\n\r");
		break;

	case MODE_TYPE_BC:
		printf("-I- PRIME mode: MODE_TYPE_BC\n\r");
		break;

	default:
		printf("-I- PRIME mode: ERROR CFG\n\r");
	}
	printf("-I- Time Period: %u\n\r", xAppPhyCfgTx.ul_tx_period);
	printf("-I- Data Len: %u\n\r", (uint32_t)xAppPhyCfgTx.xPhyMsg.data_len);
	if (xAppPhyCfgTx.uc_autodetect) {
		printf("-I- Impedance: Autodetect mode\n\r");
	} else {
		switch (xAppPhyCfgTx.uc_impedance) {
		case HI_STATE:
			printf("-I- Impedance: Fix High\n\r");
			break;

		case LO_STATE:
			printf("-I- Impedance: Fix Low\n\r");
			break;

		case VLO_STATE:
			printf("-I- Impedance: Fix Vlow\n\r");
			break;

		default:
			printf("-I- Impedance: ERROR CFG\n\r");
		}
	}

	printf(MENU_CONSOLE);
	fflush(stdout);
}

/**
 * Get ID of transmission buffer.
 */
static void get_transmission_buffer_id(void)
{
	uint8_t uc_char;

	printf("Enter the buffer to use in tx [0,1,2,3] : ");
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '0':
		case '1':
		case '2':
		case '3':
			printf("%c\r\n", uc_char);
			printf("->Buffer ID %c\r\n", uc_char);
			xAppPhyCfgTx.xPhyMsg.uc_buff_id = uc_char - 0x30;
			printf(MENU_CONSOLE);
			fflush(stdout);
			return;

		default:
			continue;
		}
	}
}

/**
 * Get ID of attenuation level.
 */
static void get_transmission_att_level(void)
{
	uint8_t uc_char;
	uint16_t us_att;

	printf("Enter attenuation level using 2 digits [00..21] : ");
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		printf("%c", uc_char);
		us_att = (uc_char - 0x30) * 10;
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		printf("%c\r\n", uc_char);
		us_att += (uc_char - 0x30);
		if (us_att < 22) {
			printf("->Attenuation level %u ok\r\n", (uint32_t)us_att);
			xAppPhyCfgTx.xPhyMsg.att_level = us_att;
			printf(MENU_CONSOLE);
			fflush(stdout);
			return;
		} else {
			printf("ERROR: Attenuation level not permitted [0..21]. Try again.\n\r");
		}
	}
}

/**
 * Get scheme of modulation.
 */
static void get_transmission_scheme(void)
{
	uint8_t uc_char;
	uint8_t uc_scheme;

	puts(MENU_SCHEME);
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '0':
		case '1':
		case '2':
			uc_scheme = uc_char - 0x30;
			break;

		case '3':
		case '4':
		case '5':
			uc_scheme = uc_char - 0x30 + 1;
			break;

		case '6':
		case '7':
			uc_scheme = uc_char - 0x30 + 6;
			break;

		default:
			continue;
		}
		printf("->Scheme %c ok\r\n", uc_char);
		xAppPhyCfgTx.xPhyMsg.scheme = uc_scheme;
		printf(MENU_CONSOLE);
		fflush(stdout);
		break;
	}
}

/**
 * Get disable RX configuration.
 */
static void get_transmission_disable_rx(void)
{
	uint8_t uc_char;
	uint8_t uc_disable;

	printf("Force disable Rx in tx [Y/N] : ");
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case 'y':
		case 'Y':
			uc_disable = true;
			printf("Disable RX in tx\r\n");
			break;

		case 'n':
		case 'N':
			uc_disable = false;
			printf("Enable RX in tx\r\n");
			break;

		default:
			continue;
		}

		xAppPhyCfgTx.xPhyMsg.disable_rx = uc_disable;
		printf(MENU_CONSOLE);
		fflush(stdout);
		break;
	}
}

/**
 * Get Transmission Mode
 */
static void get_transmission_mode(void)
{
	uint8_t uc_char;
	uint8_t uc_mode;

	puts(MENU_MODE);
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '0':
			uc_mode = MODE_TYPE_A;
			printf("MODE_TYPE_A\r\n");
			break;

		case '2':
			uc_mode = MODE_TYPE_B;
			printf("MODE_TYPE_B\r\n");
			break;

		case '3':
			uc_mode = MODE_TYPE_BC;
			printf("MODE_TYPE_BC\r\n");
			break;

		default:
			continue;
		}

		xAppPhyCfgTx.xPhyMsg.mode = uc_mode;
		printf(MENU_CONSOLE);
		fflush(stdout);
		break;
	}
}

/**
 * Get ID of channel in transmission
 */
static void get_transmission_channel(void)
{
	uint8_t uc_char;

	printf("-- Enter the channel [1,2,3,4,5,6,7,8] : ");
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			printf("%c\r\n", uc_char);
			printf("->Channel %c selected\r\n", uc_char);
			xAppPhyCfgTx.uc_channel = uc_char - 0x30;
			printf(MENU_CONSOLE);
			fflush(stdout);
			return;

		default:
			continue;
		}
	}
}

/**
 * Get ID of coupling board
 */
static void get_transmission_coupling(void)
{
	uint8_t uc_char;
	uint8_t uc_coupling_cfg = false;

	puts(MENU_COUPLING);
	fflush(stdout);
	while (!uc_coupling_cfg) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
		case '0':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP000_v1;
			uc_coupling_cfg = true;
			break;

		case '1':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP000_v2;
			uc_coupling_cfg = true;
			break;

		case '2':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP001_v1;
			uc_coupling_cfg = true;
			break;

		case '3':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP002_v1;
			uc_coupling_cfg = true;
			break;

		case '4':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP002_v2;
			uc_coupling_cfg = true;
			break;

		case '5':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP003_v1;
			uc_coupling_cfg = true;
			break;

		case '6':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP004_v1;
			uc_coupling_cfg = true;
			break;

		case '7':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP005_v1;
			uc_coupling_cfg = true;
			break;

		case '8':
			xAppPhyCfgTx.uc_coupling = ATPLCOUP006_v1;
			uc_coupling_cfg = true;
			break;

		default:
			continue;
		}
	}
	printf("Coupling board %c selected\r\n", uc_char);
	printf(MENU_CONSOLE);
	fflush(stdout);
}

/**
 * Get Period of transmission.
 */
static void get_transmission_period(void)
{
	uint8_t uc_char;
	uint8_t ucv_period[10];
	uint8_t i, c, uc_unit;
	uint16_t us_mul;

	printf("Enter transmission period in ms. (max. 10 digits): ");
	fflush(stdout);
	while (1) {
		for (i = 0; i < 10; i++) {
			while (uart_read(CONSOLE_UART, &uc_char)) {
				/* Restart watchdog */
				wdt_restart(WDT);
			}

			if (uc_char == 0x0D) {
				xAppPhyCfgTx.ul_tx_period = 0;
				for (c = i; c > 0; c--) {
					us_mul = (uint16_t)pow(10, (i - c));
					uc_unit = ucv_period[c - 1];
					xAppPhyCfgTx.ul_tx_period += uc_unit * us_mul;
				}
				printf("\r\n->Transmission period %u ms\r\n", (uint32_t)xAppPhyCfgTx.ul_tx_period);
				printf(MENU_CONSOLE);
				return;
			} else if ((uc_char >= '0') && (uc_char <= '9')) {
				printf("%c", uc_char);
				ucv_period[i] = (uc_char - 0x30);
			} else {
				printf("Error. Try again\r\n");
				break;
			}

			fflush(stdout);
		}
	}
}

/**
 * Get Length of data to transmit.
 */
static void get_data_len(void)
{
	uint8_t uc_char;
	uint8_t ucv_len[4];
	uint8_t i, c, uc_unit;
	uint16_t us_mul;

	printf("Enter length of data to transmit in bytes. (max. 500 bytes): ");
	fflush(stdout);

	xAppPhyCfgTx.xPhyMsg.data_len = 0;
	while (1) {
		for (i = 0; i < 4; i++) {
			while (uart_read(CONSOLE_UART, &uc_char)) {
				/* Restart watchdog */
				wdt_restart(WDT);
			}

			if (uc_char == 0x0D) {
				xAppPhyCfgTx.xPhyMsg.data_len = 0;
				for (c = i; c > 0; c--) {
					us_mul = (uint16_t)pow(10, (i - c));
					uc_unit = ucv_len[c - 1];
					xAppPhyCfgTx.xPhyMsg.data_len += uc_unit * us_mul;
				}
				printf("\r\n->Message Data length %u bytes\r\n", (uint32_t)xAppPhyCfgTx.xPhyMsg.data_len);
				fflush(stdout);
				return;
			} else if ((uc_char >= '0') && (uc_char <= '9')) {
				printf("%c", uc_char);
				ucv_len[i] = (uc_char - 0x30);
			} else {
				printf("Error. Try again\r\n");
				break;
			}

			fflush(stdout);
		}
	}
}

#if (SAM4C)

/**
 * Fill data message in random mode.
 */
static void fill_msg_random(void)
{
	uint8_t *p_data_buf;
	uint16_t us_len;
	uint32_t ul_random_num;

	/* Asgin pointer to tx data buffer */
	p_data_buf = ucv_tx_data_buffer;

	/* init vars */
	us_len = xAppPhyCfgTx.xPhyMsg.data_len;

	/* Configure PMC */
	pmc_enable_periph_clk(ID_TRNG);

	/* Enable TRNG */
	trng_enable(TRNG);

	/* fill message */
	while (us_len) {
		while ((trng_get_interrupt_status(TRNG) & TRNG_ISR_DATRDY) != TRNG_ISR_DATRDY) {
		}
		ul_random_num = trng_read_output_data(TRNG);
		*p_data_buf++ = (uint8_t)ul_random_num;
		if (!us_len--) {
			break;
		}

		*p_data_buf++ = (uint8_t)(ul_random_num >> 8);
		if (!us_len--) {
			break;
		}

		*p_data_buf++ = (uint8_t)(ul_random_num >> 16);
		if (!us_len--) {
			break;
		}

		*p_data_buf++ = (uint8_t)(ul_random_num >> 24);
		if (!us_len--) {
			break;
		}
	}

	/* set header type to generic message */
	xAppPhyCfgTx.xPhyMsg.data_buf[0] = 0;

	/* store the content of message in flash memory */
	flash_unlock((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, \
			(uint32_t)ADDR_APP_PHY_MESSAGE_DATA + xAppPhyCfgTx.xPhyMsg.data_len, \
			0, 0);
	flash_erase_page((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, 2);
	flash_write((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, ucv_tx_data_buffer, \
			xAppPhyCfgTx.xPhyMsg.data_len, 0);

	printf("->Random message ready\r\n");
	printf(MENU_CONSOLE);
	fflush(stdout);
}

#endif

/**
 * Fill data message in fixed mode.
 */
static void fill_msg_fixed(void)
{
	uint8_t uc_i;
	uint8_t *p_data_buf;
	uint16_t us_len;

	/* Asgin pointer to tx data buffer */
	p_data_buf = ucv_tx_data_buffer;

	/* init vars */
	us_len = xAppPhyCfgTx.xPhyMsg.data_len;
	uc_i = 0;

	/* fill message */
	while (us_len--) {
		*p_data_buf++ = 0x30 + uc_i++;
		if (uc_i == 10) {
			uc_i = 0;
		}
	}

	/* set header type to generic message */
	xAppPhyCfgTx.xPhyMsg.data_buf[0] = 0;

	/* store the content of message in flash memory */
	flash_unlock((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, \
			(uint32_t)ADDR_APP_PHY_MESSAGE_DATA + xAppPhyCfgTx.xPhyMsg.data_len, \
			0, 0);
	flash_erase_page((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, 2);
	flash_write((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, ucv_tx_data_buffer, \
			xAppPhyCfgTx.xPhyMsg.data_len, 0);

	printf("->Fixed message ready\r\n");
	printf(MENU_CONSOLE);
	fflush(stdout);
}

/**
 * Fill data message in manual mode.
 */
static void fill_msg_manual(void)
{
	uint16_t uc_i;
	uint8_t uc_char;
	uint8_t *p_data_buf;

	printf("Enter data message to transmit (max. 500 bytes): ");
	fflush(stdout);

	/* Asgin pointer to tx data buffer */
	p_data_buf = ucv_tx_data_buffer;

	xAppPhyCfgTx.xPhyMsg.data_len = 0;
	while (1) {
		for (uc_i = 0; uc_i < 500; uc_i++) {
			while (uart_read(CONSOLE_UART, &uc_char)) {
				/* Restart watchdog */
				wdt_restart(WDT);
			}

			if (uc_char == 0x0D) {
				/* set header type to generic message */
				xAppPhyCfgTx.xPhyMsg.data_buf[0] = 0;

				/* store the content of message in flash memory */
				flash_unlock((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, \
						(uint32_t)ADDR_APP_PHY_MESSAGE_DATA + xAppPhyCfgTx.xPhyMsg.data_len, \
						0, 0);
				flash_erase_page((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, 2);
				flash_write((uint32_t)ADDR_APP_PHY_MESSAGE_DATA, ucv_tx_data_buffer, \
						xAppPhyCfgTx.xPhyMsg.data_len, 0);

				printf("\r\n->Manual message ready.\r\n");
				printf(MENU_CONSOLE);
				fflush(stdout);
				return;
			} else {
				printf("%c", uc_char);
				*p_data_buf++ = uc_char;
				xAppPhyCfgTx.xPhyMsg.data_len++;
			}
		}
		printf("\r\n->End: Maximum Length is 500 bytes\r\n");
		fflush(stdout);
	}
}

/**
 * Get Transmission Data.
 */
static void get_transmission_data(void)
{
	uint8_t uc_char;

	puts(MENU_DATA_MODE);
	fflush(stdout);

	while (1) {
		while (uart_read(CONSOLE_UART, &uc_char)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		switch (uc_char) {
#if (SAM4C)
		case '0':
			printf("%c\r\n", uc_char);
			get_data_len();
			fill_msg_random();
			break;
#endif
		case '1':
			printf("%c\r\n", uc_char);
			get_data_len();
			fill_msg_fixed();
			break;

		case '2':
			printf("%c\r\n", uc_char);
			fill_msg_manual();
			break;

		default:
			continue;
		}
		break;
	}
}

/**
 * Execute TX test
 */
static void execute_tx_test(void)
{
	printf("Press 'x' to finish transmission...\r\n");
	fflush(stdout);
	/* Init Phy Embedded App */
	vPhyEmbeddedAppTask(&xAppPhyCfgTx);
	/* Start the tasks and timer running. */
	vTaskStartScheduler();
}

/**
 * \brief Main code entry point.0
 */
int main( void )
{
	xTimerHandle xMonitorTimer;
	uint8_t uc_choice;
	uint8_t uc_start_mode;
	uint32_t timeout_value;
	uint32_t wdt_mode;

	/* Prepare the hardware */
	prvSetupHardware();

	/* get value to init wdog from time in us. */
	timeout_value = wdt_get_timeout_value(WATCHDOG_5s, BOARD_FREQ_SLCK_XTAL);
	/* Configure WDT to trigger a reset. */
	wdt_mode = WDT_MR_WDRSTEN | /* Enable WDT reset. */
			WDT_MR_WDRPROC | /* WDT fault resets processor only. */
			WDT_MR_WDDBGHLT | /* WDT stops in debug state. */
			WDT_MR_WDIDLEHLT; /* WDT stops in idle state. */
	/* Initialize WDT with the given parameters. */
	wdt_init(WDT, wdt_mode, timeout_value, timeout_value);

	/* Init Phy Layer */
	phy_init(false);

	/* Create timer to monitor tasks execution */
	xMonitorTimer = xTimerCreate((const signed char *const)"Monitor timer",
			mainMONITOR_TIMER_RATE,
			pdTRUE,
			NULL,
			prvProcessMonitorTasks
			);
	configASSERT(xMonitorTimer);
	xTimerStart(xMonitorTimer, mainMONITOR_BLOCK_TIME);

	/* UART debug */
	configure_dbg_console();
	puts(STRING_HEADER);

	/* Configuration management */
	xAppPhyCfgTx.xPhyMsg.data_buf = ucv_tx_data_buffer;

	uc_start_mode = load_config();
	if (uc_start_mode == PHY_APP_CMD_DEFAULT_MODE) {
		xAppPhyCfgTx.uc_channel = atpl230Cfg.txrxChannel;
		xAppPhyCfgTx.uc_coupling = atpl230Cfg.atplCoupling;
		xAppPhyCfgTx.ul_tx_period = 1000;
		xAppPhyCfgTx.xPhyMsg.att_level = 0;
		xAppPhyCfgTx.xPhyMsg.disable_rx = false;
		xAppPhyCfgTx.xPhyMsg.mode = MODE_TYPE_A;
		xAppPhyCfgTx.xPhyMsg.scheme = PROTOCOL_DBPSK_VTB;
		xAppPhyCfgTx.xPhyMsg.tdelay = 0;
		xAppPhyCfgTx.xPhyMsg.tmode = PHY_TX_SCHEDULING_MODE_RELATIVE;
		xAppPhyCfgTx.xPhyMsg.uc_buff_id = 0;
		xAppPhyCfgTx.xPhyMsg.data_len = 64;
		/* get impedance mode */
		phy_get_cfg_param(PHY_ID_CFG_AUTODETECT_BRANCH, &xAppPhyCfgTx.uc_autodetect, 1);
		phy_get_cfg_param(PHY_ID_CFG_IMPEDANCE, &xAppPhyCfgTx.uc_impedance, 1);
		/* Fill Data of message: Fixed by default */
		fill_msg_fixed();
	} else if (uc_start_mode == PHY_APP_CMD_TX_START_MODE) {
		/* execute test */
		execute_tx_test();
	}

	/* Console Application menu */
	puts(MENU_HEADER);
	printf(MENU_CONSOLE);
	fflush(stdout);
	while (1) {
		while (uart_read(CONSOLE_UART, &uc_choice)) {
			/* Restart watchdog */
			wdt_restart(WDT);
		}
		printf("%c\r\n", uc_choice);
		fflush(stdout);

		switch (uc_choice) {
		case '0':
			get_transmission_buffer_id();
			break;

		case '1':
			get_transmission_att_level();
			break;

		case '2':
			get_transmission_scheme();
			break;

		case '3':
			get_transmission_disable_rx();
			break;

		case '4':
			get_transmission_mode();
			break;

		case '5':
			get_transmission_period();
			break;

		case '6':
			get_transmission_data();
			break;

		case '7':
			get_transmission_channel();
			break;

		case '8':
			get_transmission_coupling();
			break;

		case '9':
			get_impedance_mode();
			break;

		case 'v':
		case 'V':
			display_config();
			break;

		case 'e':
		case 'E':
			/* save configuration parameters */
			save_config(PHY_APP_CMD_TX_START_MODE);
			/* execute test */
			execute_tx_test();
			break;

		default:
			puts(MENU_HEADER);
			printf(MENU_CONSOLE);
			fflush(stdout);
			break;
		}
	}
}

/**
 * \brief Configure the hardware.
 */
static void prvSetupHardware(void)
{
	uint8_t uc_num_blinks;
	uint32_t ul_wait_counter;

	/* ASF function to setup clocking. */
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);

#ifdef CONF_BOARD_LCD_EN
	/* Clear Pinout */
	ioport_set_port_dir(IOPORT_PIOA, 0xFFFFFFFF, IOPORT_DIR_OUTPUT);
	ioport_set_port_dir(IOPORT_PIOB, 0xFFFFFFFF, IOPORT_DIR_OUTPUT);
	ioport_set_port_dir(IOPORT_PIOC, 0xFFFFFFFF, IOPORT_DIR_OUTPUT);
	ioport_set_port_level(IOPORT_PIOA, 0xFFFFFFFF, IOPORT_PIN_LEVEL_LOW);
	ioport_set_port_level(IOPORT_PIOB, 0xFFFFFFFF, IOPORT_PIN_LEVEL_LOW);
	ioport_set_port_level(IOPORT_PIOC, 0xFFFFFFFF, IOPORT_PIN_LEVEL_LOW);
#endif

	/* Atmel library function to setup for the evaluation kit being used. */
	board_init();

	/* Initialize flash: 6 wait states for flash writing. */
	flash_init(FLASH_ACCESS_MODE_128, CHIP_FLASH_WRITE_WAIT_STATE);

	/* LED signalling */
	for (uc_num_blinks = 0; uc_num_blinks < 30; uc_num_blinks++) {
		ul_wait_counter = 0xFFFFF;
		while (ul_wait_counter--) {
		}
#if (BOARD != SAM4CMP_DB && BOARD != SAM4CMS_DB)
		LED_Toggle(LED0);
		LED_Toggle(LED1);
#endif
	}
}

/**
 * \brief Display scheduler activity led.
 */
static void prvProcessMonitorTasks( xTimerHandle pxTimer )
{
	UNUSED(pxTimer);
#if (BOARD != SAM4CMP_DB && BOARD != SAM4CMS_DB)
	LED_Toggle(LED0);
#endif
}

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	 * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	 * function that will get called if a call to pvPortMalloc() fails.
	 * pvPortMalloc() is called internally by the kernel whenever a task, queue,
	 * timer or semaphore is created.  It is also called by various parts of the
	 * demo application.  If heap_1.c or heap_2.c are used, then the size of the
	 * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	 * to query the size of free heap space that remains (although it does not
	 * provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for (;;) {
		while (1) {
		}
	}
}

/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	 * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	 * task.  It is essential that code added to this hook function never attempts
	 * to block in any way (for example, call xQueueReceive() with a block time
	 * specified, or call vTaskDelay()).  If the application makes use of the
	 * vTaskDelete() API function (as this demo application does) then it is also
	 * important that vApplicationIdleHook() is permitted to return to its calling
	 * function, because it is the responsibility of the idle task to clean up
	 * memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	 * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 * function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;) {
		while (1) {
		}
	}
}

/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	 * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	 * added here, but the tick hook is called from an interrupt context, so
	 * code must not attempt to block, and only the interrupt safe FreeRTOS API
	 * functions can be used (those that end in FromISR()). */
}
