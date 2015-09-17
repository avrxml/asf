/**
 * \file
 *
 * \brief AVR UC3 IISC TDM example
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
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for the IISC driver.
 *
 * This example shows data streaming of 4 buffers, using the IISC in TDM (two
 * time-division multiplexing) mode for I2S.
 * The 4 buffers are sent in two times. First elements of each buffer are sent
 * in the first time slot and the second elements of each buffer are sent in
 * the second time slot.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section files Main Files
 * - iisc_example.c: Main file.
 * - iisc.c: IISC driver source file.
 * - iisc.h: IISC driver header file.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - STK600 evaluation kit with UC3C0 routing card;
 * - CPU clock: 16 MHz;
 * - Connection: For the UC3C0: MCK (PB21), SCK (PB22), IWS (PB23), SDO (PB19), SDI (PB20)
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "iisc.h"
#include "pdca.h"
#include "power_clocks_lib.h"

//! \brief  TDM Output buffer 0
uint32_t tdm_tx_buffer0[2] = { 0xF0F0F0F0, 0xFFFFFFFF };

//! \brief  TDM Output buffer 1
uint32_t tdm_tx_buffer1[2] = { 0xF0F0F0F0, 0xFFFFFFFF };

//! \brief  TDM Output buffer 2
uint32_t tdm_tx_buffer2[2] = { 0xF0F0F0F0, 0xFFFFFFFF };

//! \brief  TDM Output buffer 3
uint32_t tdm_tx_buffer3[2] = { 0xFFFFFFFF, 0xF0F0F0F0 };

//! \brief  IISC Gpio Map Configuration
static const gpio_map_t IISC_GPIO_MAP = {
	{AVR32_IISC_IMCK_PIN, AVR32_IISC_IMCK_FUNCTION},	//! IISC Master Clock.
	{AVR32_IISC_ISCK_PIN, AVR32_IISC_ISCK_FUNCTION},	//! IISC Slave Clock.
	{AVR32_IISC_ISDI_PIN, AVR32_IISC_ISDI_FUNCTION},	//! IISC Serial Data Input.
	{AVR32_IISC_ISDO_PIN, AVR32_IISC_ISDO_FUNCTION},	//! IISC Serial Data Output.
	{AVR32_IISC_IWS_PIN, AVR32_IISC_IWS_FUNCTION}		//! IISC Word Select.
};

//! \brief  IISC Driver Configuration
static const iisc_opt_t IISC_OPT = {
		//! Mode I2S-mode
		.mode = IISC_MODE_TDM_MASTER,
		//! Option
		.option = IISC_DATALENGTH_32 | IISC_OVERSAMPLING_x256 | IISC_TXDMA_MULTIPLE,
		//! Number of TX Channels
		.num_tx_channels = 3,
		//! Number of RX Channels
		.num_rx_channels = 3,
};

volatile avr32_iisc_t *iisc = &AVR32_IISC;

void prepare_pdca_tdm_tx(void);

/**
 * \brief Manage interrupts for each one of transmission
 * completed by IISC: disable each one of transmission
 * until the last one is reached.
 * Once the last one is reached, disable the transmission.
 */
ISR(tdm_pdca_int_tx_handler, AVR32_PDCA_IRQ_GROUP0, 0)
{
	if (pdca_get_transfer_status(0) & PDCA_TRANSFER_COMPLETE) {
		pdca_disable_interrupt_transfer_complete(0);
	}
	if (pdca_get_transfer_status(1) & PDCA_TRANSFER_COMPLETE) {
		pdca_disable_interrupt_transfer_complete(1);
	}
	if (pdca_get_transfer_status(2) & PDCA_TRANSFER_COMPLETE) {
		pdca_disable_interrupt_transfer_complete(2);
	}
	if (pdca_get_transfer_status(3) & PDCA_TRANSFER_COMPLETE) {
		pdca_disable_interrupt_transfer_complete(3);
		iisc_disable_transmission(iisc);
	}
}

/**
 * \brief Register PDCA interrupts for each one of IISC TDM channels.
 */
static void register_interrupt_pdca(void)
{
	// Register Interrupt for PDCA Transfer TX
	irq_register_handler(tdm_pdca_int_tx_handler,
			AVR32_PDCA_IRQ_0, 0);

	// Register Interrupt for PDCA Transfer TX
	irq_register_handler(tdm_pdca_int_tx_handler,
			AVR32_PDCA_IRQ_1, 0);

	// Register Interrupt for PDCA Transfer TX
	irq_register_handler(tdm_pdca_int_tx_handler,
			AVR32_PDCA_IRQ_2, 0);

	// Register Interrupt for PDCA Transfer TX
	irq_register_handler(tdm_pdca_int_tx_handler,
			AVR32_PDCA_IRQ_3, 0);

}

/**
 * \brief Configure each one of PDCA channels.
 */
void prepare_pdca_tdm_tx(void)
{
	// Local variable
	uint8_t i;

	// PDCA channel options for channel 0
	pdca_channel_options_t TDM_PDCA_OPTIONS0 = {
		.addr = &tdm_tx_buffer0,
		.pid = AVR32_IISC_PDCA_ID_TX_0,
		.size = 2,
		.r_addr = NULL,
		.r_size = 0,
		.transfer_size = PDCA_TRANSFER_SIZE_WORD
	};
	pdca_init_channel(0, &TDM_PDCA_OPTIONS0);
	pdca_enable_interrupt_transfer_complete(0);

	// PDCA channel options for channel 1
	pdca_channel_options_t TDM_PDCA_OPTIONS1 = {
		.addr = &tdm_tx_buffer1,
		.pid = AVR32_IISC_PDCA_ID_TX_1,
		.size = 2,
		.r_addr = NULL,
		.r_size = 0,
		.transfer_size = PDCA_TRANSFER_SIZE_WORD
	};
	pdca_init_channel(1, &TDM_PDCA_OPTIONS1);
	pdca_enable_interrupt_transfer_complete(1);

	// PDCA channel options for channel 2
	pdca_channel_options_t TDM_PDCA_OPTIONS2 = {
		.addr = &tdm_tx_buffer2,
		.pid = AVR32_IISC_PDCA_ID_TX_2,
		.size = 2,
		.r_addr = NULL,
		.r_size = 0,
		.transfer_size = PDCA_TRANSFER_SIZE_WORD
	};
	pdca_init_channel(2, &TDM_PDCA_OPTIONS2);
	pdca_enable_interrupt_transfer_complete(2);

	// PDCA channel options for channel 3
	pdca_channel_options_t TDM_PDCA_OPTIONS3 = {
		.addr = &tdm_tx_buffer3,
		.pid = AVR32_IISC_PDCA_ID_TX_3,
		.size = 2,
		.r_addr = NULL,
		.r_size = 0,
		.transfer_size = PDCA_TRANSFER_SIZE_WORD
	};
	pdca_init_channel(3, &TDM_PDCA_OPTIONS3);
	pdca_enable_interrupt_transfer_complete(3);

	// Enable the PDCA Channels for transmission
	for (i = 0; i < 4; i++) {
		pdca_enable(i);
	}
}

/**
 * \brief Initializes the MCU system clocks.
 */
static void init_sys_clocks(void)
{
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
}

/**
 * \brief Main function
 */
int main(void)
{
	/*
	 * Initialize basic features for the AVR UC3 family.
	 *  - GPIO Alternate feature for IISC module.
	 *  - Sysclk init for configuring clock speed.
	 *  - Configure and enable generic clock for IISC.
	 *  - Sysclk init for configuring clock speed and turning of unused
	 *    peripherals.
	 */
	gpio_enable_module(IISC_GPIO_MAP,
		sizeof(IISC_GPIO_MAP) / sizeof(IISC_GPIO_MAP[0]));
	init_sys_clocks();
	scif_gc_setup(AVR32_SCIF_GCLK_IISC,
			SCIF_GCCTRL_OSC0, AVR32_SCIF_GC_DIV_CLOCK, 16);
	scif_gc_enable(AVR32_SCIF_GCLK_IISC);

	/*
	 * Enable interrupts as the example is now configured to handle them
	 * properly.
	 */
	irq_initialize_vectors();
	cpu_irq_enable();

	/*
	 * Prepare DMA configuration.
	 *  - Register each one of the interrupts for PDCA.
	 *  - Prepare TDM buffer in transmission.
	 */
	register_interrupt_pdca();
	prepare_pdca_tdm_tx();

	/*
	 * Prepare IISC configuration.
	 *  - Initialize IISC, number of channels in transmissions/receptions.
	 *  - Prepare TDM buffer in transmission.
	 */
	iisc_init(iisc, IISC_OPT);
	iisc_enable(iisc);

	while (true);
}
