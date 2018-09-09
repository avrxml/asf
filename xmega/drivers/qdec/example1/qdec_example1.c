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
 *
 * The application checks continuously the QDec position.
 * When the QDec position changes, the QDec position, direction and frequency
 * is sent on serial COM, as bellow:
 * \code
	04 ++   317mHz
	05 ++  1436mHz
\endcode
 *
 * \note
 * All AVR XMEGA devices can be used.
 * The QDec driver API can be found \ref qdec_group "here".
 *
 * \section board_setup Board setup
 * For XMEGA-E5 Xplained board:
 * - The switch close to Quadrature Encoder must be down,
 *   thus on "PA6 & PA7" position.
 * - Connect USB (Virtual port COM) to a computer and
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

/**
 * \brief Main application routine
 *  - Configure system clock
 *  - Call QDec driver API to configure it and enable it
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
	qdec_config_phase_pins(&config, &PORTA, 6, false, 500);
	qdec_config_enable_rotary(&config);

	/* Note: XMEGA E5 Xplained includes a quadrature encoder
	 * with 40 positions for 20 hard ("clic") positions */
	qdec_config_revolution(&config, 40);
	qdec_config_enable_freq(&config, 1);
	/* QDec enable */
	qdec_enabled(&config);

	while (1) {
		/* read position */
		qdec_position = qdec_get_position(&config) / 2;

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
	}
}
