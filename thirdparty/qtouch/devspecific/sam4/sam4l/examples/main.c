/**
 * \file
 *
 * \brief SAM4L QTouch Example.
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
 * \mainpage SAM4L QTouch Example
 *
 * \section Purpose
 *
 * This example demonstrates how to use QTouch library on SAM4L devices.
 *
 * \section Requirements
 *
 * This example can be used on SAM4L series.
 *
 * \section Description
 *
 * The program configures the necessary modules for using QTouch library. After
 * it started, users can turn on and off the on-board LED by touching the QTouch
 * key.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board. Please
 *    refer to the
 *    <a href="http://ww1.microchip.com/downloads/en/appnotes/atmel-42438-sam-ba-overview-and-customization-process_applicationnote_at09423.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://ww1.microchip.com/downloads/en/appnotes/doc6310.pdf">
 *    GNU-Based Software Development</a>
 *    application note or the
 *    <a href="http://www.iar.com/website1/1.0.1.0/78/1/">
 *    IAR EWARM User and reference guides</a>,
 *    depending on the solutions that users choose.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
	-- SAM4L QTouch Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# the sent text should appear.
 */
#include "asf.h"
#include "conf_example.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- SAM4L QTouch Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

static void ast_per_callback(void)
{
	touch_sensors_update_time();
	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
}

static void example_qtouch_init(void)
{
	struct ast_config ast_conf;
	/* Enable Osc32 */
	osc_enable(OSC_ID_OSC32);
	while(!osc_is_ready(OSC_ID_OSC32));

	/* Initialize AST Controller */
	ast_enable(AST);

	ast_conf.mode = AST_COUNTER_MODE;
	ast_conf.osc_type = AST_OSC_32KHZ;
	ast_conf.psel = AST_PSEL_32KHZ_1HZ;
	ast_conf.counter = 0;
	ast_set_config(AST, &ast_conf);

	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
	ast_write_periodic0_value(AST, EXAMPLE_UPDATE_PERIOD);

	ast_set_callback(AST, AST_INTERRUPT_PER, ast_per_callback,
		AST_PER_IRQn, 0);

	/* CATB & Related clocks - for QTouch Sensors */
	sysclk_enable_peripheral_clock(CATB);
	sysclk_enable_peripheral_clock(PDCA);

	/* Initialize QTouch Library. */
	touch_sensors_init();
}

/**
 *  \brief SAM4L QTouch example entry point.
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	bool pressed = false;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the UART console */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Initialize Qtouch */
	example_qtouch_init();

	while (1) {
		touch_sensors_measure();
		if (GET_QT_SENSOR_STATE(EXAMPLE_KEY_ID) && !pressed) {
			pressed = true;
			LED_On(LED0);
		} else {
			if (!GET_QT_SENSOR_STATE(EXAMPLE_KEY_ID) && pressed) {
				LED_Off(LED0);
				pressed = false;
			}
		}
	}

}
