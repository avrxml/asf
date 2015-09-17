/**
 * \file
 *
 * \brief AVR XMEGA EDMA example 2
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
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref edma_group.
 *
 * \section files Main Files
 * - edma_example_2.c example application
 * - conf_example.h example configuration
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with a EDMA controller can be used. This example have
 * been tested on the following board(s):
 * - XMEGA-E5-XPLAINED
 *
 * \section description Description of the example
 *
 * This example will show how to use together the data match and the buffer
 * (link) modes. A first EDMA channel transmits the a large flow ('source' SRAM
 * buffer) on Tx USART. This serial flow is checked by a second EDMA channel
 * configured in match mode on Rx USART. Once a match occurs, automatically a
 * third EDMA channel in buffer (link) mode is enabled and performs a transfer
 * of a short part of the flow to the 'destination' SRAM buffer. Upon completion
 * it will verify the content of the two SRAM buffers.
 *
 * This example works with an unique USART in one-wire mode where a loop-back on
 * Rx and Tx is enabled.
 *
 * During the progress of the application it will output some information to
 * the LCD screen, and the number displayed have the following assignment:
 * - <b>1</b>: EDMA Rx match completed successfully
 * - <b>2</b>: EDMA Rx transfer completed successfully
 * - <b>3</b>: EDMA Tx transfer completed successfully
 * - <b>4</b>: CPU verified destination buffer to be correct
 * - <b>5</b>: CPU verified that source buffer haven't been garbled
 *
 * In a successful operation <b>1</b>, <b>2</b>, <b>3</b>, <b>4</b> and <b>5</b>
 * should display in sequence. On EDMA or buffer errors, <b>5</b>, <b>6</b> or
 * <b>7</b> will replace <b>1</b>, <b>2</b> and <b>3</b>, respectively within
 * the sequence. Automatically, the example program will display the following
 * status:
 * - <b>->OK</b>: In case of successful operation
 * - <b>->E-Err</b>: In case of EDMA channel error(s)
 * - <b>->D-err</b>: In case of destination buffer error
 * - <b>->S-Err</b>: In case of source buffer error
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref edma_group
 * - \ref usart_xmegae_group for one-wire serial link handling
 * - \ref pmic_group for interrupt handling
 * - \ref gfx_mono for display
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <conf_example.h>
#include <string.h>
#include <asf.h>

/** \brief Source buffer to use for EDMA Tx transfer */
static uint8_t source[EDMA_SRC_BUFFER_SIZE];

/** \brief Destination buffer to use for EDMA Rx transfer */
static uint8_t destination[EDMA_DEST_BUFFER_SIZE];

/** \brief ISR flag count */
static volatile uint8_t isr_flag = 0;

/**
 * \brief Fill buffer with a incremental pattern
 *
 * \param buffer Buffer to fill with the specific pattern
 * \param first_c First pattern element
 * \param len Length of the buffer
 */
static void fill_buffer(uint8_t *buffer, uint8_t first_c, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		buffer[i] = first_c++;
	}
}

/**
 * \brief Verify if buffer if filled by the specific pattern generated by the
 * fill_buffer() function.
 *
 * \param buffer Buffer to verify
 * \param first_c First pattern element
 * \param len Length of the buffer
 *
 * \retval true Buffer is equal to specific pattern
 * \retval false Buffer is not equal to specific pattern
 */
static bool verify_buffer(uint8_t *buffer, uint8_t first_c, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (buffer[i] != first_c++) {
			return false;
		}
	}
	return true;
}

/**
 * \brief Callback functions for EDMA transfer complete
 *
 * \param status Status of a completed (or failed) EDMA transfer
 */
static void example_edma_tx_transfer_done(edma_channel_status_t status)
{
	/* Check EDMA Tx transfer status */
	if (status == EDMA_CH_TRANSFER_COMPLETED) {
		gfx_mono_draw_string("3", 30, 16, &sysfont);
		isr_flag += 3;
	} else {
		gfx_mono_draw_string("7", 30, 16, &sysfont);
		isr_flag += 9;
	}
}

static void example_edma_rx_match_done(edma_channel_status_t status)
{
	/* Check EDMA Rx match status */
	if (status == EDMA_CH_TRANSFER_COMPLETED) {
		gfx_mono_draw_string("1", 10, 16, &sysfont);
		isr_flag += 1;
	} else {
		gfx_mono_draw_string("5", 10, 16, &sysfont);
		isr_flag += 7;
	}
}

static void example_edma_rx_transfer_done(edma_channel_status_t status)
{
	/* Check EDMA Rx transfer status */
	if (status == EDMA_CH_TRANSFER_COMPLETED) {
		gfx_mono_draw_string("2", 20, 16, &sysfont);
		isr_flag += 2;
	} else {
		gfx_mono_draw_string("6", 20, 16, &sysfont);
		isr_flag += 8;
	}
}

/**
 * \brief main function
 */
int main(void)
{
	/* Set XMEGAE USART options */
	static usart_xmegae_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
		.start_frame_detection = USART_SERIAL_SOF_DETECTION,
		.one_wire = USART_SERIAL_ONE_WIRE,
		.pec_length = USART_SERIAL_PEC_LENGTH,
		.pec_action = USART_SERIAL_PEC_ACTION,
		.encoding_type = USART_SERIAL_ENCODING_TYPE,
		.encoding_stream = USART_SERIAL_ENCODING_STREAM,
	};

	/* Set EDMA channel configuration stuctures */
	struct edma_channel_config my_tx_conf;
	struct edma_channel_config my_match_conf;
	struct edma_channel_config my_rx_conf;

	/*
	 * Initialize basic features for the AVR XMEGA E family.
	 *  - PMIC is needed to enable all interrupt levels.
	 *  - Board init for setting up GPIO and board specific features.
	 *  - Sysclk init for configuring clock speed and turning of unused
	 *    peripherals.
	 *  - Sleepmgr init for setting up the basics for the sleep manager,
	 *    although not used in this example, but the sleep manager is used
	 *    by the DMA driver.
	 *  - Gfx_mono init for configuring the display controller and create a
	 *    framebuffer in RAM if the controller lacks two-way communication
	 *  - USART init in one-wire RS 232 mode
	 */
	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();
	gfx_mono_init();
	usart_xmegae_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);

	/*
	 * Enable both Tx and Rx USART sub-modules to have a loopback.
	 */
	usart_tx_enable(USART_SERIAL);
	usart_rx_enable(USART_SERIAL);

	/*
	 * Configure:
	 * - The channel mode = 4 peripheral channels
	 * - The priority mode = Ch0 > Ch1 > Ch2 > Ch3
	 * - The buffer mode = Double buffer on Ch0 and Ch1
	 */
	edma_enable(EDMA_CHMODE);
	edma_set_priority_mode(EDMA_PRIMODE);
	edma_set_double_buffer_mode(EDMA_DBUFMODE);

	/*
	 * Fill source buffer with a specific pattern and clear destination so
	 * we are sure they are different.
	 */
	fill_buffer(source, ' ', EDMA_SRC_BUFFER_SIZE);
	memset(destination, 0, EDMA_DEST_BUFFER_SIZE);

	/*
	 * Make sure configs are all zeroed out so we don't get any stray bits
	 */
	memset(&my_tx_conf, 0, sizeof(my_tx_conf));
	memset(&my_match_conf, 0, sizeof(my_match_conf));
	memset(&my_rx_conf, 0, sizeof(my_rx_conf));

	/*
	 * In this example the example_edma_..._done() functions will be
	 * used as the callback function from completed EDMA block transfers.
	 * This function is called whenever an interrupt occurs on the EDMA
	 * channel this function is set to handle. A callback function is set
	 * by the edma_set_callback() function in the EDMA driver interface.
	 */
	edma_set_callback(EDMA_TX_CHANNEL, example_edma_tx_transfer_done);
	edma_set_callback(EDMA_MATCH_CHANNEL, example_edma_rx_match_done);
	edma_set_callback(EDMA_RX_CHANNEL, example_edma_rx_transfer_done);

	/*
	 * This code will configure EDMA_TX_CHANNEL channel with the following
	 * settings (c.f. 'my_tx_conf' configuration stucture):
	 *  - Low interrupt priority
	 *  - 1 byte burst length
	 *  - EDMA_SRC_BUFFER_SIZE bytes for the transfer
	 *  - Reload memory address at end of the transfer
	 *  - Increment memory address during transfer
	 *  - Memory address is set to \ref source
	 *  - Trigger source is USARTD0_DRE in single shot mode
	 *  - Single-shot transfer, for each trigger only one burst is transferred
	 */
	edma_channel_set_interrupt_level(&my_tx_conf, EDMA_INT_LVL_LO);
	edma_channel_set_burst_length(&my_tx_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count8(&my_tx_conf, EDMA_SRC_BUFFER_SIZE);
	edma_channel_set_memory_reload_mode(&my_tx_conf,
			EDMA_CH_RELOAD_TRANSACTION_gc);
	edma_channel_set_memory_dir_mode(&my_tx_conf, EDMA_CH_DIR_INC_gc);
	edma_channel_set_memory_address(&my_tx_conf,
			(uint16_t)(uintptr_t)source);
	edma_channel_set_trigger_source(&my_tx_conf,
			EDMA_CH_TRIGSRC_USARTD0_DRE_gc);
	edma_channel_set_single_shot(&my_tx_conf);
	edma_channel_write_config(EDMA_TX_CHANNEL, &my_tx_conf);

	/*
	 * This code will configure EDMA_MATCH_CHANNEL channel with the
	 * following settings (c.f. 'my_match_conf' configuration stucture):
	 *  - Low interrupt priority
	 *  - 1 byte burst length
	 *  - EDMA_SRC_BUFFER_SIZE bytes for the max lenght research
	 *  - Full mask match mode on data = EDMA_MATCH_PATTERN
	 *  - Trigger source is USARTD0_RXC in single shot mode
	 *  - Single-shot transfer, for each trigger only one burst is compared
	 */
	edma_channel_set_interrupt_level(&my_match_conf, EDMA_INT_LVL_LO);
	edma_channel_set_burst_length(&my_match_conf,
			EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_transfer_count8(&my_match_conf, EDMA_SRC_BUFFER_SIZE);
	edma_channel_set_memory_reload_mode(&my_match_conf,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_match_mode(&my_match_conf, EDMA_CH_DIR_MP1_gc);
	edma_channel_set_match_data(&my_match_conf, EDMA_MATCH_PATTERN, 0xFF);
	edma_channel_set_trigger_source(&my_match_conf,
			EDMA_CH_TRIGSRC_USARTD0_RXC_gc);
	edma_channel_set_single_shot(&my_match_conf);
	edma_channel_write_config(EDMA_MATCH_CHANNEL, &my_match_conf);

	/*
	 * This code will configure EDMA_RX_CHANNEL channel with the following
	 * settings (c.f. 'my_rx_conf' configuration stucture):
	 *  - Low interrupt priority
	 *  - 1 byte burst length and repeat(link)mode
	 *  - EDMA_DEST_BUFFER_SIZE bytes for the transfer
	 *  - Reload memory address at end of the transfer
	 *  - Increment memory address during transfer
	 *  - Memory address is set to \ref destination
	 *  - Trigger source is USARTD0_RXC in single shot mode
	 *  - Single-shot transfer, for each trigger only one burst is transferred
	 *  - The transfer will be enabled once a match occurs on
	 *    EDMA_MATCH_CHANNEL
	 */
	edma_channel_set_interrupt_level(&my_rx_conf, EDMA_INT_LVL_LO);
	edma_channel_set_burst_length(&my_rx_conf, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_repeat(&my_rx_conf);
	edma_channel_set_transfer_count8(&my_rx_conf, EDMA_DEST_BUFFER_SIZE);
	edma_channel_set_memory_reload_mode(&my_rx_conf,
			EDMA_CH_RELOAD_TRANSACTION_gc);
	edma_channel_set_memory_dir_mode(&my_rx_conf, EDMA_CH_DIR_INC_gc);
	edma_channel_set_memory_address(&my_rx_conf,
			(uint16_t)(uintptr_t)destination);
	edma_channel_set_trigger_source(&my_rx_conf,
			EDMA_CH_TRIGSRC_USARTD0_RXC_gc);
	edma_channel_set_single_shot(&my_rx_conf);
	edma_channel_write_config(EDMA_RX_CHANNEL, &my_rx_conf);

	/*
	 * Enable interrupts as the example is now configured to handle them
	 * properly.
	 */
	cpu_irq_enable();

	/* Display text header */
	gfx_mono_draw_string("EDMA ex-2:", 0, 0, &sysfont);

	/*
	 * Use the configurations above by enabling the EDMA channels:
	 * - 1st: EDMA_MATCH_CHANNEL enabled by software
	 * - 2nd: EDMA_TX_CHANNEL enabled by software
	 * - 3rd: EDMA_RX_CHANNEL will be enabled by EDMA_MATCH_CHANNEL match
	 *   completed
	 */
	edma_channel_enable(EDMA_MATCH_CHANNEL);
	edma_channel_enable(EDMA_TX_CHANNEL);

	while (isr_flag < 6) {
		/*
		 * Force a NOP instruction for an eventual placement of a debug
		 * session breakpoint.
		 */
		asm ("nop\n");
	}

	/*
	 * At the end of EDMA transfers or if an EDMA error occurs, disable EDMA
	 * and both Tx and Rx USART.
	 */
	edma_disable();
	usart_tx_disable(USART_SERIAL);
	usart_rx_disable(USART_SERIAL);

	/* Verify buffers and display the status of the EDMA operations */
	if (isr_flag == 6) {
		/* Check if destination is as expected */
		if (verify_buffer(destination, 'A', EDMA_DEST_BUFFER_SIZE)) {
			gfx_mono_draw_string("4", 40, 16, &sysfont);
			/* Check if source haven't been garbled */
			if (verify_buffer(source, ' ', EDMA_SRC_BUFFER_SIZE)) {
				/* No error found */
				gfx_mono_draw_string("5->OK", 50, 16, &sysfont);
			} else {
				/* Source buffer error */
				gfx_mono_draw_string("->S-Err", 50, 16,
						&sysfont);
				isr_flag |= 0xC0;
			}
		} else {
			/* Destination buffer error */
			gfx_mono_draw_string("->D-err", 40, 16, &sysfont);
			isr_flag |= 0x80;
		}
	} else {
		if (isr_flag < 0x80) {
			/* EDMA error */
			gfx_mono_draw_string("->E-Err", 40, 16, &sysfont);
		}
	}

	while (true) {
		/*
		 * Force a NOP instruction for an eventual placement of a debug
		 * session breakpoint.
		 */
		asm ("nop\n");
	}
}
