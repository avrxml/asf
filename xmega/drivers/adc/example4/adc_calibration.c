/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 4
 * ADC calibration with external voltage supply for reference voltage.
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
 * This example demonstrates an offset and gain calibration method for
 * XMEGA E ADC using hardware compensation with the \ref adc_group.
 *
 * The application continuously performs ADC conversions and send ADC values
 * on terminal.
 * The external ADC input voltage can be applied in 0V to 1.9V range.
 *
 * At startup the hardware correction is disabled.
 * To enable hardware correction press key 'c', and key 'd' to disable it.
 *
 * When the hardware correction is enabled for the first time, the
 * application requests to apply a 0 Volt on ADC input (PA0) to capture
 * and to load offset correction.
 * Next the application requests to apply a 1.9 Volts on ADC input (PA0)
 * to capture and to load gain correction.
 * After these actions, the ADC hardware correction is enabled.
 *
 * The "Calibration and correction" method is described in the datasheet
 * "XMEGA E Manual".
 *
 * ADC configuration used:
 * - 12-bit unsigned results
 * - 1x gain
 * - voltage reference = VCC / 1.6 = 3.3V / 1.6
 * - single-ended measurement
 * - V+ is connected on PA0 pin
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PC2 & PC3 pins should be respetively connected to RXD & TXD pins
 *   of RS232 SPARE header
 * - STK600 RS2232 port should be connected to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
 * - an external power supply should be connected on ADC input (PA0 pin)
 *   The AREF0 signal from STK600 can be used.
 *   AREF0 jumper can be mounted.
 *   It can be adjusted through Atmel Studio
 *   and menu "Tools / Device Programming".
 *   Select STK600 and in "Board settings" adjust AREF1 voltages.
 *
 * For XMEGA-E5 Xplained board:
 * - Connect USB (Virtual port COM) to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
 * - an external power supply should be connected on ADC input.
 *   PA0 pin on header J2-1.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <conf_example.h>

/* Internal ADC funtions */
static void main_adc_init(void);
static void main_adc_correction_start(void);
static void main_adc_correction_stop(void);
static uint16_t main_adc_input(void);

int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	printf("\x0C\n\r-- ADC Calibration Example --\n\r");
	printf("-- Compiled: %s %s --\n\r\n\r", __DATE__, __TIME__);

	/* ADC initializations */
	main_adc_init();

	printf("Commands:\n\r");
	printf("- key 'c' to enable correction\n\r");
	printf("- key 'd' to disable correction\n\r");
	main_adc_correction_stop();

	while (1) {
		if (usart_rx_is_complete(CONF_TEST_USART)) {
			char key = getchar();
			if (key == 'c') {
				main_adc_correction_start();
			}

			if (key == 'd') {
				main_adc_correction_stop();
			}
		}

		printf(" %4d mV\r", main_adc_input());
	}
}

/**
 * \brief Initialize ADC
 */
static void main_adc_init(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	/* Configure the ADC module:
	 * - signed, 12-bit results
	 * - voltage reference = VCC / 1.6 = 3.3V / 1.6
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure ADC channel:
	 * - Single ended mode
	 * - Input voltage on ADC0 pin (PA0 pin)
	 * - 1x gain
	 * - interrupt flag set on completed conversion
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC */
	adc_enable(&ADCA);

	/* Do useful conversion */
	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
}

/**
 * \brief Measures and enables offset and gain corrections
 */
static void main_adc_correction_start(void)
{
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;
	static bool correction_measures_done = false;
	static uint16_t offset_correction;

	/* Expected value for gain correction at 1.9V
	 * expected_value = Max. range (12 bits unsigned) * 1.9V / (VCC / 1.6)
	 */
	const uint16_t expected_value
		= ((1 << 12) * 1900UL) / (3300L * 1000L / 1600L);
	/* Captured value for gain correction */
	static uint16_t captured_value;

	if (correction_measures_done) {
		goto main_adc_correction_enable;
	}

	printf("\n\r*Measure offset correction\n\r");
	printf("Set PA0 pin to GND and press a key to trigge measurement.\n\r");
	printf("Warning on STK600: Remove AREF0 jumper to do it.\n\r");
	getchar();

	/* Capture value for 0 Volt */
	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	offset_correction = adc_get_unsigned_result(&ADCA, ADC_CH0);

	/* Enable offset correction */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_correction(&adcch_conf, offset_correction, 1, 1);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	printf("*Measure Gain correction\n\r");
	printf("Set PA0 pin to 1.9 Volt");
	printf(" and press a key to trigge measurement.\r\n");

	printf("Reminder on STK600: Set AREF0 jumper to do it.\n\r");
	getchar();

	/* Capture value for 1.9 Volts */
	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	captured_value = adc_get_unsigned_result(&ADCA, ADC_CH0);

	correction_measures_done = true;

main_adc_correction_enable:
	/* Enable offset & gain correction */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_correction(&adcch_conf, offset_correction, expected_value,
			captured_value);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	printf("\n\r* ADC correction enabled:  ");
	printf("Offset correction %d,   ", offset_correction);
	if (expected_value > captured_value) {
		printf("Gain correction 1.%03u\n\r", (uint16_t)
				((((uint32_t)expected_value - captured_value)
				* 1000) / captured_value));
	} else {
		printf("Gain correction 0.%03u\n\r", (uint16_t)
				(((uint32_t)expected_value
				* 1000) / captured_value));
	}
}

/**
 * \brief Disables offset and gain corrections
 */
static void main_adc_correction_stop(void)
{
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	printf("\n\r* ADC correction disabled\n\r");
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_disable_correction(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
}

/**
 * \brief Captures a values on ADC
 *
 * \return value on ADC pins (mV)
 */
static uint16_t main_adc_input(void)
{
	uint16_t sample;

	adc_start_conversion(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	sample = adc_get_unsigned_result(&ADCA, ADC_CH0);

	/* Conversion sample to mV :
	 * mV = sample * (VCC / 1.6) / Max. range
	 */
	return ((uint32_t)sample * (3300L * 1000L / 1600L)) / (1 << 12);
}
