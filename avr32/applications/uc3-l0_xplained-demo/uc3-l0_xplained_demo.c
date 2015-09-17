/**
 * \file
 *
 * \brief UC3-L0 Xplained demo application that reads the board temperature
 * sensor and changes the color of the board RGB LED based on touch input.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 *
 * This application demonstrates the QMatrix functionality of the UC3-L0
 * Xplained board. On power-on, the board will initialize the required touch
 * libraries, showing the current touch state of the slider and touch button on
 * the board LEDs. The temperature sensor data will be sent to the USART connected
 * to the board USB Virtual Com Port.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/avr/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "scif_uc3l.h"
#include "power_clocks_lib.h"
#include "pm_uc3l.h"
#include "flashcdw.h"
#include "adcifb.h"
#include "pwma.h"
#include "print_funcs.h"
#include "usart.h"
#include "uc3-l0_xplained_demo.h"

/**
 * \brief Initialize ADC driver to read the board temperature sensor.
 *
 * Initializes the board's ADC driver module and configures the ADC channel
 * connected to the onboard NTC temperature sensor ready for conversions.
 */
static void init_adc(void)
{
	// Assign and enable GPIO pin to the ADC function.
	gpio_enable_module_pin(ADC_TEMPERATURE_PIN, ADC_TEMPERATURE_FUNCTION);

	const adcifb_opt_t adcifb_opt = {
		.resolution             = AVR32_ADCIFB_ACR_RES_12BIT,
		.shtim                  = 15,
		.ratio_clkadcifb_clkadc = 2,
		.startup                = 3,
		.sleep_mode_enable      = false
	};

	// Enable and configure the ADCIFB module
	sysclk_enable_peripheral_clock(&AVR32_ADCIFB);
	adcifb_configure(&AVR32_ADCIFB, &adcifb_opt);

	// Configure the trigger (No trigger, only software trigger)
	adcifb_configure_trigger(&AVR32_ADCIFB, AVR32_ADCIFB_TRGMOD_NT, 0);

	// Enable the ADCIFB channel to NTC temperature sensor
	adcifb_channels_enable(&AVR32_ADCIFB, ADC_TEMPERATURE_CHANNEL);
}

/**
 * \brief Initializes the USART.
 *
 * Initializes the board USART ready for serial data to be transmitted and
 * received.
 */
static void init_usart(void)
{
	const usart_options_t usart_options = {
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Initialize USART in RS232 mode with the requested settings.
	sysclk_enable_peripheral_clock(USART);
	usart_init_rs232(USART, &usart_options, sysclk_get_pba_hz());
}

/**
 * \brief Initializes the PWM subsystem ready to generate the RGB LED PWM
 * waves.
 *
 * Initializes the on-chip PWM module and configures the RGB LED PWM outputs so
 * the the brightness of the three individual channels can be adjusted.
 */
static void init_pwm(void)
{
	// GPIO pin/function map for the RGB LEDs.
	gpio_enable_module_pin(LED_RED_PWMA,   LED_PWMA_FUNCTION);
	gpio_enable_module_pin(LED_GREEN_PWMA, LED_PWMA_FUNCTION);
	gpio_enable_module_pin(LED_BLUE_PWMA,  LED_PWMA_FUNCTION);

	const scif_gclk_opt_t genclk3_opt = {
		.clock_source = SCIF_GCCTRL_CPUCLOCK,
		.divider      = 8,
		.diven        = true,
	};

	// Start generic clock 3 for the PWM outputs.
	scif_start_gclk(AVR32_PM_GCLK_GCLK3, &genclk3_opt);

	// Enable RGB LED PWM.
	sysclk_enable_peripheral_clock(&AVR32_PWMA);
	pwma_config_enable(&AVR32_PWMA,EXAMPLE_PWMA_FREQUENCY,EXAMPLE_PWMA_GCLK_FREQUENCY,0); 
	pwma_set_channels_value(&AVR32_PWMA,PWM_CHANNEL_RED | PWM_CHANNEL_BLUE| PWM_CHANNEL_GREEN,255);

}

/**
 * \brief Application main loop.
 */
int main(void)
{
	board_init();
	sysclk_init();

	sysclk_enable_peripheral_clock(USART);

	// Initialize touch, ADC, USART and PWM
	init_adc();
	init_usart();
	init_pwm();
	init_touch();

	while (true) {
		uint32_t adc_data;

		// Read slider and button and update RGB led
		touch_handler();

		// Wait until the ADC is ready to perform a conversion.
		do { } while (!adcifb_is_ready(&AVR32_ADCIFB));

		// Start an ADCIFB conversion sequence.
		adcifb_start_conversion_sequence(&AVR32_ADCIFB);

		// Wait until the converted data is available.
		do { } while (!adcifb_is_drdy(&AVR32_ADCIFB));

		// Get the last converted data.
		adc_data = (adcifb_get_last_data(&AVR32_ADCIFB) & 0x3FF);

		// Write temperature data to USART
		do { } while (!usart_tx_empty(USART));
		usart_write_char(USART, (adc_data >> 8));
		do { } while (!usart_tx_empty(USART));
		usart_write_char(USART, (adc_data & 0xFF));
	}
}
