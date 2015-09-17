/**
 * \file
 *
 * \brief Audio Bitstream DAC Example for SAM4L.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 * \mainpage Audio Bitstream DAC Example
 *
 * \section Purpose
 *
 * The example demonstrates how to use ABDAC driver to
 * transfers an input buffer of audio samples to an output .
 *
 * \section Requirements
 *
 * This example can be used on SAM4L-EK boards with hardware rework. Please
 * see detail in document: ATSAM4L-EK User Guide.
 *
 * \section Description
 *
 * The example read the audio samples and output it.
 *
 * \section Usage
 *
 * - Build the program and download it to the evaluation board. Please
 *  refer to the SAM4L main page documentation 
 *  http://www.atmel.com/tools/SAM4L-EK.aspx
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
	-- ABDAC Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# the sent text should appear.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "sound.h"

#define PDCA_ABDAC_CHANNEL0  0
#define PDCA_ABDAC_CHANNEL1  1

#define SOUND_SAMPLES 0x100
int16_t samples_left[SOUND_SAMPLES];
int16_t samples_right[SOUND_SAMPLES];

volatile bool exit_flag = false;

/* PDCA channel options */
static const pdca_channel_config_t pdca_abdac_config0 = {
	.addr = 0,   /* memory address */
	.pid = ABDACB_PDCA_ID_TX_CH0,     /* select peripheral */
	.size = 0,   /* transfer counter */
	.r_addr = 0,                   /* next memory address */
	.r_size = 0,                   /* next transfer counter */
	.ring = false,                 /* disable ring buffer mode */
	.transfer_size = PDCA_MR_SIZE_HALF_WORD /* select size of the transfer */
};
static const pdca_channel_config_t pdca_abdac_config1 = {
	.addr = 0,   /* memory address */
	.pid = ABDACB_PDCA_ID_TX_CH1,     /* select peripheral */
	.size = 0,   /* transfer counter */
	.r_addr = 0,                   /* next memory address */
	.r_size = 0,                   /* next transfer counter */
	.ring = false,                 /* disable ring buffer mode */
	.transfer_size = PDCA_MR_SIZE_HALF_WORD /* select size of the transfer */
};

/** ABDAC instance */
struct abdac_dev_inst g_abdac_inst;

/** ABDAC configuration */
struct abdac_config   g_abdac_cfg;

/**
 * \brief EIC interrupt handler for push button interrupt
 */
static void eic_5_callback(void)
{
	if (eic_line_interrupt_is_pending(EIC, GPIO_PUSH_BUTTON_EIC_LINE)) {
		exit_flag = true;
		eic_line_clear_interrupt(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
	}
}

static void config_buttons(void)
{
	/* Initialize EIC for button wakeup. */
	struct eic_line_config eic_line_conf = {
		EIC_MODE_EDGE_TRIGGERED,
		EIC_EDGE_FALLING_EDGE,
		EIC_LEVEL_LOW_LEVEL,
		EIC_FILTER_DISABLED,
		EIC_ASYNCH_MODE
	};
	eic_enable(EIC);
	eic_line_set_config(EIC, GPIO_PUSH_BUTTON_EIC_LINE, 
		&eic_line_conf);
	eic_line_set_callback(EIC, GPIO_PUSH_BUTTON_EIC_LINE, eic_5_callback,
		EIC_5_IRQn, 1);
	eic_line_enable(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
}

/**
 * \brief Configure UART console.
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
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Display the user menu on the terminal.
 */
static void display_menu(void)
{
	printf("Menu :\r\n"
			"  -- Select the action:\r\n"
			"  s: Start output the audio with PDCA. \r\n"
			"  h: Display menu \r\n\r\n");
}

/**
 * \brief Application entry point for ABDAC example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t key;
	uint32_t i, count;
	status_code_t status;

	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();

	/* Output example information */
	printf("-- ABDAC Example --\r\n");
	printf("-- %s\r\n", BOARD_NAME);
	printf("-- Compiled: %s %s --\r\n", __DATE__, __TIME__);

	/* Config the push button. */
	config_buttons();

	/* Config the ABDAC. */
	abdac_get_config_defaults(&g_abdac_cfg);
	g_abdac_cfg.sample_rate_hz = ABDAC_SAMPLE_RATE_11025;
	g_abdac_cfg.data_word_format = ABDAC_DATE_16BIT;
	g_abdac_cfg.mono = false;
	g_abdac_cfg.cmoc = false;
	status = abdac_init(&g_abdac_inst, ABDACB, &g_abdac_cfg);
	if (status != STATUS_OK) {
		printf("-- Initialization fails! --\r\n");
		while (1) {
		}
	}
	abdac_enable(&g_abdac_inst);
	abdac_clear_interrupt_flag(&g_abdac_inst, ABDAC_INTERRUPT_TXRDY);
	abdac_clear_interrupt_flag(&g_abdac_inst, ABDAC_INTERRUPT_TXUR);

	/* Config PDCA module */
	pdca_enable(PDCA);
	pdca_channel_set_config(PDCA_ABDAC_CHANNEL0, &pdca_abdac_config0);
	pdca_channel_enable(PDCA_ABDAC_CHANNEL0);
	pdca_channel_set_config(PDCA_ABDAC_CHANNEL1, &pdca_abdac_config1);
	pdca_channel_enable(PDCA_ABDAC_CHANNEL1);

	/* Display menu. */
	display_menu();

	while (1) {
		scanf("%c", (char *)&key);

		switch (key) {
		case 'h':
			display_menu();
			break;

		case 's':
			printf("--Looped output audio, use push button to exit--\r\n");
			abdac_set_volume0(&g_abdac_inst, false, 0x7FFF);
			abdac_set_volume1(&g_abdac_inst, false, 0x7FFF);
			i = 0;
			/* output sample from the sound_table array */
			while(!exit_flag) {
				count = 0;
				// Store sample from the sound_table array
				while(count < (SOUND_SAMPLES)){
					samples_left[count] = ((uint8_t)sound_table[i]) << 8;
					samples_right[count] = ((uint8_t)sound_table[i]) << 8;
					i++;
					count++;
					if (i >= sizeof(sound_table)) i = 0;
				}

				pdca_channel_write_reload(PDCA_ABDAC_CHANNEL0,
						(void *)samples_left, SOUND_SAMPLES);
				pdca_channel_write_reload(PDCA_ABDAC_CHANNEL1,
						(void *)samples_right, SOUND_SAMPLES);
				/**
				 * Wait until the reload register is empty. This means that
				 * one transmission is still ongoing but we are already able
				 * to set up the next transmission.
				 */
				while(!(pdca_get_channel_status(PDCA_ABDAC_CHANNEL1)
					== PDCA_CH_COUNTER_RELOAD_IS_ZERO));
			}
			exit_flag = false;
			printf("--Exit the audio output--\r\n\r\n");
			/* Mute the volume */
			abdac_set_volume0(&g_abdac_inst, true, 0x7FFF);
			abdac_set_volume1(&g_abdac_inst, true, 0x7FFF);
			break;

		default:
			break;
		}
	}
}
