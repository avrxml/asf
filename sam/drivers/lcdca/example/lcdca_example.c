/**
 * \file
 *
 * \brief SAM4L LCDCA example.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <string.h>
#include "conf_example.h"

/**
 *  Configure serial console.
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

	/* Configure console. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Write string to C42048A lcd glass alphanumeric field.
 *
 * This function will write the input string to the alphanumeric field of the
 * lcd glass.
 *
 * \param data Pointer to the data input string
 */
static inline void c42364a_write_alpha_packet(const uint8_t *data)
{
	//! [lcd_example_write_alphanumeric]
	// Display in alphanumeric field.
	lcdca_write_packet(LCDCA_TDG_14SEG4COM, FIRST_14SEG_4C, data, \
			WIDTH_14SEG_4C, DIR_14SEG_4C);
	//! [lcd_example_write_alphanumeric]
}

/**
 * \brief Write string to c42048a lcd glass numeric field.
 *
 * This function will write the input string to the numeric field of the
 * lcd glass.
 *
 * \param  data Pointer to the data input string
 */
static inline void c42364a_write_num_packet(const uint8_t *data)
{
	//! [lcd_example_write_numeric]
	// Display in numeric field.
	lcdca_write_packet(LCDCA_TDG_7SEG4COM, FIRST_7SEG_4C, data, \
			WIDTH_7SEG_4C, DIR_7SEG_4C);
	//! [lcd_example_write_numeric]
}

/**
 * \brief Wait for push button been pressed.
 */
static void wait_for_switches(void)
{
	do { } while (ioport_get_pin_level(NEXT_BUTTON));
	delay_ms(1);
	do { } while (!ioport_get_pin_level(NEXT_BUTTON));
	delay_ms(1);
}

/**
 * \brief Callback handler for LCDCA Interrupt.
 */
static void lcdca_callback(void)
{
	/* Do nothing here */
}

/**
 * \brief Application entry point for LCDCA example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* LCDCA configuration */
	struct lcdca_config lcdca_cfg;
	/* Automated display configuration */
	//! [lcd_example_scroll_config_1]	
	struct lcdca_automated_char_config automated_char_cfg;
	//! [lcd_example_scroll_config_1]	
	/* String for sequential display */
	uint8_t const sequential_str[] = \
		"Sequen tial   string display,Press PB0 to Cont.  ";
	/* String for scrolling display */
	//! [lcd_example_scroll_config_2]	
	uint8_t const scrolling_str[] = \
		"Scrolling string display, Press PB0 to cont.  ";
	//! [lcd_example_scroll_config_2]	

	/* String for lowpower display */
#define MAX_NUM_LOWPOWER_STR    4
	uint8_t const lowpower_str[MAX_NUM_LOWPOWER_STR][7] = {
		"In     ",
		"Power  ",
		"Save   ",
		"Mode   ",
	};
	uint32_t str_id = 0;

	struct lcdca_blink_config blink_cfg;
	struct lcdca_circular_shift_config cs_cfg;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	sleepmgr_init();

	/* Initialize the console uart */
	configure_console();

	/* Output example information */
	printf("-- LCDCA Controller Example --\r\n");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	//! [lcd_example_init]
	// LCDCA Controller initialization
	// - Clock,
	// - Connect to C42364A glass LCD component,
	// - Timing:  64 Hz frame rate & low power waveform, FC0, FC1, FC2
	// - Interrupt: off.
	//! [lcd_example_init_clock]
	lcdca_clk_init();
	//! [lcd_example_init_clock]
	lcdca_cfg.port_mask = PORT_MASK;
	lcdca_cfg.x_bias = false;
	lcdca_cfg.lp_wave = true;
	lcdca_cfg.duty_type = LCD_DUTY;
	lcdca_cfg.lcd_pres = false;
	lcdca_cfg.lcd_clkdiv = 3;
	lcdca_cfg.fc0 = 16;
	lcdca_cfg.fc1 = 2;
	lcdca_cfg.fc2 = 6;
	lcdca_cfg.contrast = LCD_CONTRAST_LEVEL;
	//! [lcd_example_set_config]
	lcdca_set_config(&lcdca_cfg);
	//! [lcd_example_set_config]
	//! [lcd_example_enable_module]
	lcdca_enable();
	//! [lcd_example_enable_module]
	//! [lcd_example_enable_timers]
	lcdca_enable_timer(LCDCA_TIMER_FC0);
	lcdca_enable_timer(LCDCA_TIMER_FC1);
	lcdca_enable_timer(LCDCA_TIMER_FC2);
	//! [lcd_example_enable_timers]

	/* Turn on LCD back light */
	//! [lcd_example_backlight_on]
	ioport_set_pin_level(LCD_BL_GPIO, IOPORT_PIN_LEVEL_HIGH);
	//! [lcd_example_backlight_on]
	//! [lcd_example_init]
	
	/* Display some texts and icon on segment LCD */
	lcdca_set_pixel(ICON_ARM);
	c42364a_write_num_packet((const uint8_t *)"0123");
	c42364a_write_alpha_packet((const uint8_t *)"Welcome");

	/* Blink "error" icon */
	//! [lcd_example_blink_init]
	blink_cfg.lcd_blink_timer = LCDCA_TIMER_FC1;
	blink_cfg.lcd_blink_mode = LCDCA_BLINK_SELECTED;
	lcdca_blink_set_config(&blink_cfg);
	lcdca_set_pixel(ICON_ERROR);
	lcdca_set_blink_pixel(ICON_ERROR);
	lcdca_blink_enable();
	//! [lcd_example_blink_init]
	
	/* Autonomous segment animation of 7-pixels area of the LCD */
	//! [lcd_example_shift_enable]
	cs_cfg.lcd_csr_timer = LCDCA_TIMER_FC1;
	cs_cfg.lcd_csr_dir = LCDCA_CSR_RIGHT;
	cs_cfg.size = 7;    // Total 7-pixels.
	cs_cfg.data = 0x03; // Display 2 pixel at one time.
	lcdca_circular_shift_set_config(&cs_cfg);
	lcdca_circular_shift_enable();
	//! [lcd_example_shift_enable]

	/* Automated sequential character string display */
	automated_char_cfg.automated_mode = LCDCA_AUTOMATED_MODE_SEQUENTIAL;
	automated_char_cfg.automated_timer = LCDCA_TIMER_FC2;
	automated_char_cfg.lcd_tdg = LCDCA_TDG_14SEG4COM;
	automated_char_cfg.stseg = FIRST_14SEG_4C;
	automated_char_cfg.dign = WIDTH_14SEG_4C;
	automated_char_cfg.steps = 0;
	automated_char_cfg.dir_reverse = LCDCA_AUTOMATED_DIR_REVERSE;
	lcdca_automated_char_set_config(&automated_char_cfg);
	lcdca_automated_char_start(sequential_str, strlen((char const *)sequential_str));
	printf("Press PB0 to stop automated sequential mode and continue.\n\r");
	wait_for_switches();
	lcdca_automated_char_stop();

	/* Automated scrolling of character string display */
	//! [lcd_example_scrolling_enable]
	automated_char_cfg.automated_mode = LCDCA_AUTOMATED_MODE_SCROLLING;
	automated_char_cfg.automated_timer = LCDCA_TIMER_FC2;
	automated_char_cfg.lcd_tdg = LCDCA_TDG_14SEG4COM;
	automated_char_cfg.stseg = FIRST_14SEG_4C;
	automated_char_cfg.dign = WIDTH_14SEG_4C;
	/* STEPS = string length - DIGN + 1 */
	automated_char_cfg.steps = sizeof(scrolling_str) - WIDTH_14SEG_4C + 1;
	automated_char_cfg.dir_reverse = LCDCA_AUTOMATED_DIR_REVERSE;
	lcdca_automated_char_set_config(&automated_char_cfg);
	lcdca_automated_char_start(scrolling_str, strlen((char const *)scrolling_str));
	//! [lcd_example_scrolling_enable]
	printf("Press PB0 to stop automated scrolling mode and continue.\n\r");
	wait_for_switches();
	lcdca_automated_char_stop();

	/* Set callback for LCDCA interrupt */
	lcdca_set_callback(lcdca_callback, LCDCA_IRQn, 1);

	/* Enable LCDCA wake-up */
	lcdca_enable_wakeup();

	printf("Enter power save mode.\n\r");
	printf("It'll display power save string sequence and"
			"blink wireless icon when every wake up.\n\r");
	while (1) {
		/* Display lowpower string repeatly */
		c42364a_write_alpha_packet(lowpower_str[str_id]);
		str_id = (str_id + 1) % MAX_NUM_LOWPOWER_STR;

		/* Toggle wireless icon */
		lcdca_toggle_pixel(ICON_WLESS);

		/* Enter in sleep mode */
		sleepmgr_enter_sleep();
	}
}
