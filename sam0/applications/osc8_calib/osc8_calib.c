/**
 * \file
 *
 * \brief SAM0+ OSC8MHz Calibration Application
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
#include <asf.h>

/**
 * \mainpage SAM D20 OSC8M Calibration Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails preface
 *
 *
 * \page appdoc_preface Overview
 * This application calibrates the internal OSC8M (8MHz) oscillator of the
 * device against a known, accurate external clock or crystal frequency.
 */

/**
 * \page appdoc_main SAM D20 OSC8M Calibration Example
 *
 * Overview:
 * - \ref appdoc_samd20_osc8m_cal_intro
 * - \ref appdoc_samd20_osc8m_cal_usage
 * - \ref appdoc_samd20_osc8m_cal_compinfo
 * - \ref appdoc_samd20_osc8m_cal_contactinfo
 *
 * \section appdoc_samd20_osc8m_cal_intro Introduction
 * While some devices require exact timing, and therefore require an external
 * calibration crystal or other high-accuracy clock source, other applications
 * with looser accuracy requirements may use the internal RC Oscillator(s) for
 * space and/or cost reasons. However, this can lead to unstable communication
 * interfaces if the internal oscillator is not first calibrated against an
 * accurate reference clock.
 *
 * This application uses a known external reference frequency to calibrate the
 * internal 8MHz (nominal) RC Oscillator, OSC8M, so that it is as close as
 * possible to the desired 8MHz frequency.
 *
 * \section appdoc_samd20_osc8m_cal_usage Usage Instructions
 * On startup, the application will immediately begin calibration of the OSC8M
 * internal oscillator, against a 32.768KHz watch crystal attached to the device
 * XOSC32K pins (see device datasheet). As the possible calibration values are
 * cycled through, the board LED will turn on each time a better match is found,
 * and turn off when an equal or lesser calibration value is tested. Once the
 * best values are found, the results are printed to the device USART and the
 * board LED will flash rapidly to signal the end of the calibration sequence.
 *
 * \note The calibration values are \b not stored to the device's non-volatile
 *       memory.
 *
 * \section appdoc_samd20_osc8m_cal_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for ARM.
 * Other compilers may or may not work.
 *
 * \section appdoc_samd20_osc8m_cal_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */

/** Resolution of the calibration binary divider; higher powers of two will
 *  reduce the calibration resolution.
 */
#define CALIBRATION_RESOLUTION   13

/** Calibration reference clock generator index. */
#define REFERENCE_CLOCK          GCLK_GENERATOR_3

/** Frequency of the calibration reference clock in Hz */
#define REFERENCE_CLOCK_HZ       32768

/** Software instance of the USART upon which to transmit the results. */
static struct usart_module usart_edbg;

/** Software instance of the TC module used for calibration measurement. */
static struct tc_module tc_calib;

/** Software instance of the TC module used for calibration comparison. */
static struct tc_module tc_comp;


/** Set up the measurement and comparison timers for calibration.
 *   - Configure the measurement timer to run from the CPU clock, in capture
 *     mode.
 *   - Configure the reference timer to run from the reference clock, generating
 *     a capture every time the calibration resolution count is met.
 */
static void setup_tc_channels(void)
{
	struct tc_config config;
	tc_get_config_defaults(&config);

	/* Configure measurement timer to run from Fcpu and capture */
	config.counter_size    = TC_COUNTER_SIZE_32BIT;
	config.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	config.wave_generation = TC_WAVE_GENERATION_NORMAL_FREQ;
	config.enable_capture_on_channel[0] = true;
	tc_init(&tc_calib, TC0, &config);

	/* Configure reference timer to run from Fcpu and capture when the resolution count is met */
	config.counter_size    = TC_COUNTER_SIZE_16BIT;
	config.clock_source    = REFERENCE_CLOCK;
	config.enable_capture_on_channel[0] = false;
	config.size_specific.size_16_bit.compare_capture_channel[0] = (1 << CALIBRATION_RESOLUTION);
	tc_init(&tc_comp, TC2, &config);
}

/** Set up the measurement and comparison timer events.
 *   - Configure the reference timer to generate an event upon comparison
 *     match with channel 0.
 *   - Configure the measurement timer to trigger a capture when an event is
 *     received.
 */
static void setup_tc_events(void)
{
	/* Enable incoming events on on measurement timer */
	struct tc_events events_calib = { .on_event_perform_action = true };
	tc_enable_events(&tc_calib, &events_calib);

	/* Generate events from the reference timer on channel 0 compare match */
	struct tc_events events_comp = { .generate_event_on_compare_channel[0] = true };
	tc_enable_events(&tc_comp, &events_comp);

	tc_enable(&tc_calib);
	tc_enable(&tc_comp);
}

/** Set up the event system, linking the measurement and comparison timers so
 *  that events generated from the reference timer are linked to the measurement
 *  timer.
 */
static void setup_events(void)
{
	struct events_chan_config evch_conf;
	events_chan_get_config_defaults(&evch_conf);

	/* Event channel 0 detects rising edges of the reference timer output
	 * event */
	evch_conf.edge_detection = EVENT_EDGE_RISING;
	evch_conf.path           = EVENT_PATH_SYNCHRONOUS;
	evch_conf.generator_id   = EVSYS_ID_GEN_TC2_MCX_0;
	events_chan_set_config(EVENT_CHANNEL_0, &evch_conf);

	struct events_user_config evus_conf;
	events_user_get_config_defaults(&evus_conf);

	/* Measurement timer event user channel linked to Event channel 0 */
	evus_conf.event_channel_id = EVENT_CHANNEL_0;
	events_user_set_config(EVSYS_ID_USER_TC0_EVU, &evus_conf);
}

/** Set up the USART for transmit-only communication at a fixed baud rate. */
static void setup_usart_channel(void)
{
	struct usart_config cdc_uart_config;
	usart_get_config_defaults(&cdc_uart_config);

	/* Configure the USART settings and initialize the standard I/O library */
	cdc_uart_config.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	cdc_uart_config.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	cdc_uart_config.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	cdc_uart_config.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	cdc_uart_config.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	cdc_uart_config.baudrate    = 115200;
	stdio_serial_init(&usart_edbg, EDBG_CDC_MODULE, &cdc_uart_config);

	usart_enable(&usart_edbg);
	usart_enable_transceiver(&usart_edbg, USART_TRANSCEIVER_TX);
}

/** Set up the clock output pin so that the current system clock frequency can
 *  be monitored via an external frequency counter or oscilloscope. */
static void setup_clock_out_pin(void)
{
	struct system_pinmux_config pin_mux;
	system_pinmux_get_config_defaults(&pin_mux);

	/* MUX out the system clock to a I/O pin of the device */
	pin_mux.mux_position = MUX_PA30H_GCLK_IO0;
	system_pinmux_pin_set_config(PIN_PA30H_GCLK_IO0, &pin_mux);
}

/** Retrieves the current system clock frequency, computed from the reference
 *  clock.
 *
 * \return Current system clock frequency in Hz.
 */
static uint32_t get_osc_frequency(void)
{
	/* Clear any existing match status on the measurement timer */
	tc_clear_status(&tc_comp, TC_STATUS_CHANNEL_0_MATCH);

	/* Restart both measurement and reference timers */
	tc_start_counter(&tc_calib);
	tc_start_counter(&tc_comp);

	/* Wait for the measurement timer to signal a compare match */
	while (!(tc_get_status(&tc_comp) & TC_STATUS_CHANNEL_0_MATCH)) {
		/* Wait for channel 0 match */
	}

	/* Compute the real clock frequency from the measurement timer count and
	 * reference count */
	uint64_t tmp = tc_get_capture_value(&tc_calib, TC_COMPARE_CAPTURE_CHANNEL_0);
	return ((tmp * REFERENCE_CLOCK_HZ) >> CALIBRATION_RESOLUTION);
}

int main(void)
{
	/* System initialization */
	system_init();
	events_init();
	delay_init();

	/* Module initialization */
	setup_tc_channels();
	setup_tc_events();
	setup_events();
	setup_clock_out_pin();

	/* Variables to track the previous and best calibration settings */
	uint16_t comm_best   = -1;
	uint8_t  frange_best = -1;
	uint32_t freq_best   = -1;
	uint32_t freq_before = get_osc_frequency();

	/* Run calibration loop */
	for (uint8_t frange_cal = 1; frange_cal <= 2; frange_cal++) {
		for (uint16_t comm_cal = 0; comm_cal < 128; comm_cal++) {
			/* Set the test calibration values */
			system_clock_source_write_calibration(
					SYSTEM_CLOCK_SOURCE_OSC8M, (8 << 7) | comm_cal, frange_cal);

			/* Wait for stabilization */
			delay_cycles(1000);

			/* Compute the deltas of the current and best system clock
			 * frequencies, save current settings if they are closer to the
			 * ideal frequency than the previous best values
			 */
			uint32_t freq_current = get_osc_frequency();
			if (abs(freq_current - 8000000UL) < abs(freq_best - 8000000UL))
			{
				freq_best   = freq_current;
				comm_best   = comm_cal;
				frange_best = frange_cal;

				port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			}
			else
			{
				port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
			}
		}
	}

	/* Set the found best calibration values */
	system_clock_source_write_calibration(
			SYSTEM_CLOCK_SOURCE_OSC8M, (8 << 7) | comm_best, frange_best);

	/* Setup USART module to output results */
	setup_usart_channel();

	/* Write previous and current frequency and new calibration settings to the
	 * USART
	 */
	printf("Freq Before: %lu\r\n", freq_before);
	printf("Freq Before: %lu\r\n", freq_best);

	printf("Freq Range: %u\r\n", frange_best);
	printf("Calib Value: %u\r\n", (8 << 7) | comm_best);

	/* Rapidly flash the board LED to signal the calibration completion */
	while (1) {
		port_pin_toggle_output_level(LED_0_PIN);
		delay_ms(200);
	}
}
