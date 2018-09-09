/**
 * \file
 *
 * \brief AVR UC3 IISC I2S example 2
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
 * <A href="http://www.microchip.com/design-centers/32-bit/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
