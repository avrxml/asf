/**
 * \file
 *
 * \brief Smart Card Driver Quick Start
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

/**
 * \mainpage Smart Card Example
 * See \ref appdoc_sam0_smart_card_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 * \page appdoc_preface Overview
 * In this use case we will communicate with smart card via ISO7816, reset the
 * smart card, send the different command to the smart card through choice and
 * receive the responsible from the smart card.
 *
 * The iso7816 software provide in this examples is use to transform APDU
 * commands to TPDU commands for the smart card. The iso7816 provide here
 * is for the protocol T=0 only. The send and the receive of a character
 * is made under polling. User must change these pins according to his
 * environment.
 */

 /**
 * \page appdoc_sam0_smart_card_main Smart Card Example
 * Overview:
 * - \ref appdoc_sam0_smart_card_intro
 * - \ref appdoc_sam0_smart_card_setup
 * - \ref appdoc_sam0_smart_card_compinfo
 * - \ref appdoc_sam0_smart_card_contactinfo
 *
 * \section  appdoc_sam0_smart_card_intro Introduction
 * In this use case we will communicate with smart card via ISO7816, reset the
 * smart card, send the different command to the smart card through choice and
 * receive the responsible from the smart card.
 *
 * The iso7816 software provide in this examples is use to transform APDU
 * commands to TPDU commands for the smart card. The iso7816 provide here
 * is for the protocol T=0 only. The send and the receive of a character
 * is made under polling. User must change these pins according to his
 * environment.
 *
 *
 * This application has been tested on following boards:
 * - SAM L22 Xplained Pro
 *
 * \section appdoc_sam0_smart_card_setup Hardware Setup
 * The Smart Card Xplained Pro extension board must be connected to extension
 * header 3 on the SAM L22 Xplained Pro.
 *
 * This example uses the SIM smart card to test.
 *
 * To run the test:
 *  - Connect the SAM Xplained Pro board to the computer using a
 *    micro USB cable.
 *  - Open the virtual COM port in a terminal application.
 *    \note The USB composite firmware running on the Embedded Debugger (EDBG)
 *          will enumerate as debugger, virtual COM port and EDBG data
 *          gateway.
 *  - Build the project, program the target and run the application.
 *    The terminal shows the results of the unit test.
 *
 * \section appdoc_sam0_smart_card_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for ARM.
 * Other compilers may or may not work.
 *
 * \section appdoc_sam0_smart_card_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */

#include <asf.h>
#include <stdio_serial.h>
#include <string.h>
#include "conf_iso7816.h"

static struct usart_module cdc_uart_module, usart_instance;

#define STRING_EOL      "\r"
#define STRING_HEADER   "-- USART Smart Card(ISO7816) Example --\r\n" \
						"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Maximum uc_size in bytes of the smart card answer to a uc_command. */
#define MAX_ANSWER_SIZE         10

/* Maximum ATR uc_size in bytes. */
#define MAX_ATR_SIZE            55

#define CMD1_LEN                7
#define CMD2_LEN                7
#define CMD3_LEN                4
/* Test command #1. */
const uint8_t test_cmd1[CMD1_LEN] = {0xA0, 0xA4, 0x00, 0x00, 0x02, 0x7F, 0x20};
/* Test command #2. */
const uint8_t test_cmd2[CMD2_LEN] = {0xA0, 0xA4, 0x00, 0x00, 0x02, 0x6F, 0x07};
/* Test command #3. */
const uint8_t test_cmd3[CMD3_LEN] = {0x00, 0x10, 0x00, 0x00};

/**
 * \brief Initialize the USART for the example.
 */
static void cdc_uart_init(void)
{
	struct usart_config usart_conf;

	/* Configure USART for output */
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART, &usart_conf);
	usart_enable(&cdc_uart_module);
}

/**
 * \brief Detect smart card.
 */
static void smart_card_detect(void)
{
	struct port_config pin_conf;

	port_get_config_defaults(&pin_conf);

	/* Config smart card detect */
	pin_conf.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(CONF_ISO7816_PIN_DET, &pin_conf);

	if(port_pin_get_input_level(CONF_ISO7816_PIN_DET) == false) {
		printf("-Warning- Please insert Smart card. \r\n");
	}
	while (port_pin_get_input_level(CONF_ISO7816_PIN_DET) == false) {
		/* Wait insert smart card */
	}

	printf("-I- Smart card is detected. To begin testing... \r\n");
}

/**
 * \brief Initialize the ISO7816 for smart card.
 */
static void smart_card_init(void)
{
	struct port_config pin_conf;

	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(CONF_ISO7816_PIN_RST, &pin_conf);
	port_pin_set_output_level(CONF_ISO7816_PIN_RST, false);

	/* Config smart card vcc enable */
	port_pin_set_config(CONF_ISO7816_PIN_VCC, &pin_conf);
	port_pin_set_output_level(CONF_ISO7816_PIN_VCC, true);

	struct usart_config usart_conf;

	/* Configure USART for output */
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting               = CONF_ISO7816_MUX_SETTING;
	usart_conf.pinmux_pad0               = CONF_ISO7816_PINMUX_PAD0;
	usart_conf.pinmux_pad1               = CONF_ISO7816_PINMUX_PAD1;
	usart_conf.pinmux_pad2               = CONF_ISO7816_PINMUX_PAD2;
	usart_conf.pinmux_pad3               = CONF_ISO7816_PINMUX_PAD3;
	usart_conf.iso7816_config.enabled    = true;
	usart_conf.iso7816_config.protocol_t = ISO7816_PROTOCOL_T_0;
	usart_conf.transfer_mode             = USART_TRANSFER_SYNCHRONOUSLY;
	usart_conf.baudrate                  = CONF_ISO7816_BAUDRATE;
	usart_conf.parity                    = USART_PARITY_EVEN;
	usart_conf.generator_source          = GCLK_GENERATOR_1;
	usart_conf.iso7816_config.guard_time = ISO7816_GUARD_TIME_2_BIT;

	usart_init(&usart_instance, CONF_ISO7816_USART, &usart_conf);

	iso7816_init(&usart_instance, CONF_ISO7816_PIN_RST, \
				system_gclk_chan_get_hz(usart_conf.generator_source));

	/* Config pinmux as smart card clock */
	struct system_pinmux_config pin_clk_conf;
	system_pinmux_get_config_defaults(&pin_clk_conf);
	pin_clk_conf.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	pin_clk_conf.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	pin_clk_conf.mux_position = PINMUX_PA15H_GCLK_IO1 & 0xFFFF;
	system_pinmux_pin_set_config(PINMUX_PA15H_GCLK_IO1 >> 16, &pin_clk_conf);

	usart_enable(&usart_instance);
}

/**
 * \brief Displays a menu which enables the user to send several commands to the
 * smart card and check its answers.
 */
static void send_receive_cmd(void)
{
	uint8_t uc_message[MAX_ANSWER_SIZE];
	uint8_t  uc_size;
	uint16_t uc_key;
	uint8_t  uc_command;
	uint8_t  i;

	/* Clear message buffer. */
	for (i = 0; i < MAX_ANSWER_SIZE; i++) {
		uc_message[i] = 0;
	}

	/* Display menu. */
	puts("-I- The following three commands can be sent:\r\n");
	puts("  1. ");
	for (i=0; i < CMD1_LEN; i++) {
		printf("0x%02X ", test_cmd1[i]);
	}
	puts("\r\n  2. ");

	for (i=0; i < CMD2_LEN; i++) {
		printf("0x%02X ", test_cmd2[i]);
	}
	puts("\r\n  3. ");

	for (i=0; i < CMD3_LEN; i++) {
		printf("0x%02X ", test_cmd3[i]);
	}

	/* Get user input. */
	uc_key = 0;
	while (uc_key != 'q') {
		puts("\r\nChoice ? (q to quit): ");
		while (usart_read_wait(&cdc_uart_module, &uc_key)) {
		}
		printf("%c\r\n", uc_key);
		uc_command = (uint8_t)uc_key - '0';

		/* Check user input. */
		uc_size = 0;
		if (uc_command == 1) {
			puts("-I- Sending test command #1 : ");
			for (i = 0; i < CMD1_LEN; i++) {
				printf("0x%02X ", test_cmd1[i]);
			}
			puts("...");
			uc_size = iso7816_xfr_block_tpdu_t0(test_cmd1, uc_message, CMD1_LEN);
		} else {
			if (uc_command == 2) {
				puts("-I- Sending test command #2 : ");
				for (i = 0; i < CMD2_LEN; i++) {
					printf("0x%02X ", test_cmd2[i]);
				}
				puts("...");
				uc_size = iso7816_xfr_block_tpdu_t0(test_cmd2, uc_message, CMD2_LEN);
			} else {
				if (uc_command == 3) {
					puts("-I- Sending test command #3 : ");
					for (i = 0; i < CMD3_LEN; i++) {
						printf("0x%02X ", test_cmd3[i]);
					}
					puts("...");
					uc_size = iso7816_xfr_block_tpdu_t0(test_cmd3, uc_message, CMD3_LEN);
				}
			}
	   }

		/* Output smart card answer. */
		puts("Answer: ");
		if (uc_size > 0) {
			for (i=0; i < uc_size; i++) {
				printf("0x%02X ", uc_message[i]);
			}
		} else {
			puts("Receive error! \r\n");
		}
	}
	puts("Please waiting ... \r\n");
	port_pin_set_output_level(CONF_ISO7816_PIN_VCC, false);

	puts("The smart card can now be safely removed. \r\n");
}

int main(void)
{
	uint8_t p_atr[MAX_ATR_SIZE];
	uint8_t  uc_size;
	uint16_t i;

	/* Init system. */
	system_init();
	/* Module configuration. */
	cdc_uart_init();

	/* Output example information. */
	printf("%s \r\n", STRING_HEADER);

	/* Smart card detect and receive respond. */
	smart_card_detect();

	/* Smart card init. */
	smart_card_init();

	/* Smart card warm reset. */
	iso7816_warm_reset();

	memset(p_atr, 0, sizeof(p_atr));
	iso7816_data_block_atr(p_atr, &uc_size);

	if (uc_size != 0) {
		puts("Reset respond: ");
		for(i = 0; i < uc_size; i++) {
			printf("0x%02X ", p_atr[i]);
		}
		puts("\r\n");
	}

	/* Allow user to send some commands. */
	send_receive_cmd();

	while (1) {
		/* Infinite loop */
	}
}
