/**
 * \file
 *
 * \brief AVR UC3 IISC I2S example 2
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
 * This example shows basic I2S transmission in master mode.
 * The byte 0x55 is sent continuously. If a timeout is detected, the application
 * is stopped.
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
 * - STK600 evaluation kit and UC3C0 routing card;
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
#include "power_clocks_lib.h"

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
		.mode = IISC_MODE_MASTER,
		//! Option
		.option = IISC_DATALENGTH_8 | IISC_OVERSAMPLING_x256 | IISC_FORMAT_LJ,
		//! Number of TX Channels
		.num_tx_channels = 1,
		//! Number of RX Channels
		.num_rx_channels = 1,
};

volatile avr32_iisc_t *iisc = &AVR32_IISC;

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
	status_code_t status;

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
	 * Prepare IISC configuration.
	 *  - Initialize IISC in I2S mode.
	 */
	iisc_init(iisc, IISC_OPT);
	iisc_enable(iisc);

	while (true) {
		status = iisc_write(iisc,0x55);
		if (status == ERR_TIMEOUT){
			while(true);
		}
	}
}
