/**
 * \file
 *
 * \brief XMEGA E Quadrature decoder driver example 1 application
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 * XMEGA devices include quadrature decoder (QDEC) using the event system and
 * timer counter harware resources.
 *
 * This example project shows how to implement a QDec easly through the API
 * functions provided by XMEGA QDec driver.
 * Here, the Quadrature Encoder is simulated using a Timer Counter.
 *
 * The application checks continuously the QDec position.
 * When the QDec position change, the QDec position, direction and frequency
 * is sent on serial COM, as bellow:
 * \code
	04 ++   317mHz
	05 ++  1436mHz
\endcode
 *
 * To change the direction of Quadrature encoder, press 'd'.
 * To change increase speed of Quadrature encoder, press '+'.
 * To change decrease speed of Quadrature encoder, press '-'.
 *
 * \note
 * All AVR XMEGA devices can be used.
 * The QDec driver API can be found \ref qdec_group "here".
 *
 * \section board_setup Board setup
 * For XMEGA-E5 Xplained board:
 * - Connect USB (Virtual port COM) to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
 * For XMEGA-B1 Xplained board:
 * - Connect the UART RXD (PE2) and TXD (PE3) to a RS232 port.
 *   the communication terminal configuration should be 38400 baud, no parity.
 *
 * \section compinfo Compilation info
 *  This software was written for the GNU GCC and IAR for AVR. Other compilers
 *  may or may not work.
 *
 * \section contactinfo Contact Information
 *  For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <conf_example.h>
#include "generator_qenc.h"

#define QUADRATURE_RESOLUTION            (4 * 4)
#define QUADRATURE_GENERATOR_FREQ_STEP   (1000 / QUADRATURE_RESOLUTION)

/**
 * \brief Main application routine
 *  - Configure system clock
 *  - Call QDec driver API to configure it and enable it
 *  - Call generator of quadrature encoder
 *  - Get position, direction and frequency and display its
 */
int main( void )
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};
	qdec_config_t config;
	uint16_t qdec_position, qdec_position_prev = 0;
	uint32_t generator_qenc_freq = 2000 / QUADRATURE_RESOLUTION;
	bool generator_qenc_dir = true;

	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	printf("\x0C\n\r-- QDec Example --\n\r");
	printf("-- Compiled: %s %s --\n\r\n\r", __DATE__, __TIME__);

	/* QDec configuration
	 * Here, we use the default hardware ressources
	 * proposed by default configuration.
	 */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_freq(&config, 1);
	qdec_config_freq_tc(&config, CONF_QDEC_FREQ_TC);
	qdec_config_revolution(&config, QUADRATURE_RESOLUTION);
	/* QDec enable */
	qdec_enabled(&config);

	/* Generate quadrature encoder signals */
	generator_qenc_enable(CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			CONF_GENERATOR_QENC_TC, QUADRATURE_RESOLUTION,
			generator_qenc_freq, generator_qenc_dir);

	while (1) {
		/* read position */
		qdec_position = qdec_get_position(&config);

		if (qdec_position_prev != qdec_position) {
			/* New position then display it */
			qdec_position_prev = qdec_position;
			printf("%02u", qdec_position);
			/* display direction */
			if (qdec_get_direction(&config)) {
				printf(" ++");
			} else {
				printf(" --");
			}

			/* Display frequency */
			printf(" %5umHz\r\n", qdec_get_frequency(&config));
		}

		/* Manage Quadrature encoder generator through UART */
		if (usart_rx_is_complete(CONF_TEST_USART)) {
			char key = getchar();
			if (key == 'd') {
				generator_qenc_dir = !generator_qenc_dir;
				generator_qenc_set_direction(generator_qenc_dir);
			}

			if (key == '-') {
				if (generator_qenc_freq > QUADRATURE_GENERATOR_FREQ_STEP) {
					generator_qenc_freq -= QUADRATURE_GENERATOR_FREQ_STEP;
					generator_qenc_set_freq(generator_qenc_freq);
				}
			}

			if (key == '+') {
				generator_qenc_freq += QUADRATURE_GENERATOR_FREQ_STEP;
				generator_qenc_set_freq(generator_qenc_freq);
			}
		}
	}
}
