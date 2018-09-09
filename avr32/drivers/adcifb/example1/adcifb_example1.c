/**
 * \file
 *
 * \brief ADCIFB example driver for AVR32 UC3.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the ADCIFB software driver.
 *
 * It also gives an example of usage of the ADCIFB module, eg:
 * - On the AT32UC3L-EK: Display on a serial terminal the battery voltage
 *   converted value.
 * - On the UC3-L0-Xplained: Display on a serial terminal the voltage from NTC
 *   temperature sensor.
 *
 * \section files Main Files
 * - adcifb_example1.c : ADCIFB example1
 * - conf_example.h : Configuration for ADCIFB example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with an ADCIFB module can be used. This example has been
 * tested with the following setup:<BR>
 * - AT32UC3L-EK evaluation kit
 * - UC3-L0-Xplained kit
 *
 * \section setupinfo Setup Information for the AT32UC3L-EK kit
 * CPU speed: <i> 48 MHz </i>
 * - Insert a V450HR battery into the socket of the AT32UC3L-EK
 * - Connect a PC USB cable to the USB VCP plug of the AT32UC3L-EK
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section setupinfo Setup Information for the UC3-L0 Xplained kit
 * CPU speed: <i> 48 MHz </i>
 * - Connect a PC USB cable to the USB VCP plug of the UC3-L0 Xplained
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "conf_example.h"

/** Internal ADCIFB CLK_ADC clock frequency, in Hz */
#define EXAMPLE_TARGET_CLK_ADC_FREQ_HZ  1500000

/** \brief Main function - init and loop to display ADC values */
int main(void)
{
	/* GPIO pin/ADC-function map. */
	const gpio_map_t ADCIFB_GPIO_MAP = {
		{EXAMPLE_ADCIFB_PIN, EXAMPLE_ADCIFB_FUNCTION}
	};
	
	/* ADCIFB Configuration */
	adcifb_opt_t adcifb_opt = {
		/* Resolution mode */
		.resolution = AVR32_ADCIFB_ACR_RES_12BIT,

		/* Channels Sample & Hold Time in [0,15] */
		.shtim  = 15,
		.ratio_clkadcifb_clkadc =
				(sysclk_get_pba_hz() / EXAMPLE_TARGET_CLK_ADC_FREQ_HZ),

		/*
		 * Startup time in [0,127], where
		 * Tstartup = startup * 8 * Tclk_adc (assuming Tstartup ~ 15us max)
		 */
		.startup = 3,
		
		/* ADCIFB Sleep Mode disabled */
		.sleep_mode_enable = false
	};
	
	uint32_t adc_data;

	/* Initialize the system clocks */
	sysclk_init();

	/* Init debug serial line */
	init_dbg_rs232(sysclk_get_pba_hz());

	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADCIFB_GPIO_MAP,
			sizeof(ADCIFB_GPIO_MAP) / sizeof(ADCIFB_GPIO_MAP[0]));

	/* Enable and configure the ADCIFB module */
	if (adcifb_configure(&AVR32_ADCIFB, &adcifb_opt) != PASS) {
		/* Config error. */
		while (true) {
			gpio_tgl_gpio_pin(LED0_GPIO);
			
			delay_ms(100);
		}
	}

	/* Configure the trigger mode */
	/* "No trigger, only software trigger can start conversions". */
	if (adcifb_configure_trigger(&AVR32_ADCIFB, AVR32_ADCIFB_TRGMOD_NT, 0)
			!= PASS) {
		/* Config error. */
		while (true) {
			gpio_tgl_gpio_pin(LED1_GPIO);
			
			delay_ms(100);
		}
	}

	/* Enable the ADCIFB channel the battery is connected to. */
	adcifb_channels_enable(&AVR32_ADCIFB, EXAMPLE_ADCIFB_CHANNEL_MASK);

	while (true) {
		while (adcifb_is_ready(&AVR32_ADCIFB) != true) {
			/* Wait until the ADC is ready to perform a conversion. */
		}

		/* Start an ADCIFB conversion sequence. */
		adcifb_start_conversion_sequence(&AVR32_ADCIFB);

		while (adcifb_is_drdy(&AVR32_ADCIFB) != true) {
			/* Wait until the converted data is available. */
		}

		/* Get the last converted data. */
		adc_data = adcifb_get_last_data(&AVR32_ADCIFB);

		/* Display the current voltage of the battery. */
		print_dbg("\x1B[2J\x1B[H\r\nADCIFB Example\r\nHEX Value for "
				EXAMPLE_ADCIFB_CHANNEL_NAME " : 0x");
		print_dbg_hex(adc_data & AVR32_ADCIFB_LCDR_LDATA_MASK);
		print_dbg("\r\n");
		
		delay_ms(500);

		/*
		 * Note1: there is a resistor bridge between the battery and the
		 * ADC pad on the AT32UC3L-EK. The data converted is thus half
		 * of
		 * the battery voltage.
		 */

		/*
		 * Note2: if the battery is not in place, the conversion is out
		 * of
		 * spec because the ADC input is then higher than ADVREF.
		 */
	}
}
