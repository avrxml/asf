/**
 * \file
 *
 * \brief AST example driver for AVR32 UC3.
 *
 * Copyright (C) 2009-2015 Atmel Corporation. All rights reserved.
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
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the real time counter driver. It also comes
 * bundled with an application-example of usage.
 *
 * This example demonstrates how to use the AST driver with the 32 KHz
 * oscillator crystal, setup an alarm to wake-up from sleep mode every second.
 * <b>Operating mode: </b>It uses the USART driver to display a real time
 * counter incremented every second.
 * The display is triggered by the interrupt controller module.
 *
 * \section files Main Files
 * - ast.c: Asynchronous Timer driver;
 * - ast.h: Asynchronous Timer driver header file;
 * - ast_example2.c: Asynchronous Timer example application using sleep mode
 *                   function.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an AST, an INTC and a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK UC3L_EK and STK600+RCUC3D evaluation kit;
 * - CPU clock: 16 MHz;
 * - USART2 (on UC3C_EK) abstracted with a USB CDC connection to a PC, or RS232 Usart (on STK600);
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include <stdint.h>
#if defined (__GNUC__)
#  include "intc.h"
#endif
#include "board.h"
#include "compiler.h"
#include "ast.h"
#include "usart.h"
#include "gpio.h"
#include "power_clocks_lib.h"

/*! \name USART Settings
 */
/*! @{ */
#if BOARD == UC3C_EK
#  define EXAMPLE_USART               (&AVR32_USART2)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK    AVR32_USART2_CLK_PBA
#  define FPBA                        FOSC0
#  define EXAMPLE_AST_OSC             AST_OSC_32KHZ
#  define EXAMPLE_AST_PRESCALER       AST_PSEL_32KHZ_1HZ
#endif

#if BOARD == UC3L_EK
#  define EXAMPLE_USART                 (&AVR32_USART3)
#  define EXAMPLE_USART_RX_PIN          AVR32_USART3_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION     AVR32_USART3_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN          AVR32_USART3_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION     AVR32_USART3_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK      AVR32_USART3_CLK_PBA
#  define EXAMPLE_TARGET_DFLL_FREQ_HZ   96000000 /* DFLL target frequency Hz */
#  define EXAMPLE_TARGET_MCUCLK_FREQ_HZ 12000000 /* MCU clock target freq Hz */
#  define EXAMPLE_TARGET_PBACLK_FREQ_HZ 12000000 /* PBA clock target freq Hz */
#  define FPBA                          EXAMPLE_TARGET_PBACLK_FREQ_HZ
#  define EXAMPLE_AST_OSC               AST_OSC_32KHZ
#  define EXAMPLE_AST_PRESCALER         AST_PSEL_32KHZ_1HZ

/*! \name Parameters to pcl_configure_clocks().
 */
/*! @{ */
static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
static pcl_freq_param_t pcl_dfll_freq_param = {
	.main_clk_src = PCL_MC_DFLL0,
	.cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
	.pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
	.pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
	.dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
	.pextra_params = &gc_dfllif_ref_opt
};
/*! @} */
#endif

#if BOARD == STK600_RCUC3D
#  define EXAMPLE_USART               (&AVR32_USART1)
/* For the RX pin, connect STK600.PORTD.PD0 to STK600.RS232 SPARE.RXD */
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
/* For the TX pin, connect STK600.PORTC.PC7 to STK600.RS232 SPARE.TXD */
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_CLOCK_MASK    AVR32_USART1_CLK_PBA
#  define FPBA                        FOSC0
/* For the OSC32, connect STK600.PORTB.PB37 to STK600.AUX.32KHz */
#  define EXAMPLE_AST_OSC             AST_OSC_32KHZ
#  define EXAMPLE_AST_PRESCALER       AST_PSEL_32KHZ_1HZ
#endif

#if !defined(EXAMPLE_USART)             ||\
	!defined(EXAMPLE_USART_RX_PIN)      ||\
	!defined(EXAMPLE_USART_RX_FUNCTION) ||\
	!defined(EXAMPLE_USART_TX_PIN)      ||\
	!defined(EXAMPLE_USART_TX_FUNCTION)
#  error The USART configuration to use in this example is missing.
#endif
/*! @} */

/*!
 * \brief print_i function : convert the given number to an ASCII
 * decimal representation.
 */
static char *print_i(char *str, int n)
{
	int i = 10;

	str[i] = '\0';
	do {
		str[--i] = '0' + n % 10;
		n /= 10;
	} while (n);

	return &str[i];
}

/*!
 * \brief main function : do init and loop (poll if configured so)
 */
int main(void)
{
	char temp[20];
	char *ptemp;
	uint32_t ast_alarm;

	static const gpio_map_t USART_GPIO_MAP = {
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	/* USART options */
	static const usart_options_t USART_OPTIONS = {
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = 0
	};

#if BOARD == UC3L_EK
	scif_osc32_opt_t opt = {
		/* 2-pin Crystal connected to XIN32/XOUT32 and high current
		 * mode. */
		SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR,
		/* oscillator startup time */
		AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC,
		/* select alternate xin32_2 and xout32_2 for 32kHz crystal
		 * oscillator */
		true,
		/* disable the 1kHz output */
		false,
		/* enable the 32kHz output */
		true
	};
#else
	scif_osc32_opt_t opt;
	opt.mode = SCIF_OSC_MODE_2PIN_CRYSTAL;
	opt.startup = AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC;
#endif

#if BOARD == UC3L_EK

	/*
	 * Note: on the AT32UC3L-EK board, there is no crystal/external clock
	 * connected to the OSC0 pinout XIN0/XOUT0. We shall then program the
	 * DFLL and switch the main clock source to the DFLL.
	 */
	pcl_configure_clocks(&pcl_dfll_freq_param);

	/*
	 * Note: since it is dynamically computing the appropriate field values
	 * of the configuration registers from the parameters structure, this
	 * function is not optimal in terms of code size. For a code size
	 * optimal solution, it is better to create a new function from
	 * pcl_configure_clocks_dfll0() and modify it to use preprocessor
	 * computation from pre-defined target frequencies.
	 */
#else
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
#endif

	/* Start OSC_32KHZ */
	scif_start_osc32(&opt, true);

	/* Assign GPIO pins to USART0. */
	gpio_enable_module(USART_GPIO_MAP,
			sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	/* Initialize USART in RS232 mode */
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FPBA);

	/* Welcome sentence // 2-pin Crystal and high current mode. */
	/* Crystal is connected to XIN32/XOUT32. */
	usart_write_line(EXAMPLE_USART, "\x1B[2J\x1B[H\r\nATMEL\r\n");
	usart_write_line(EXAMPLE_USART, "AVR32 UC3 - AST example 2\r\n");
	usart_write_line(EXAMPLE_USART,
			"AST 32 KHz oscillator counter example.\r\n");
	usart_write_line(EXAMPLE_USART,
			"Alarm0 wakeup from static sleep mode every second.\r\n");

	/* Using counter mode and set it to 0 */
	unsigned long ast_counter = 0;

	/* Initialize the AST */
	if (!ast_init_counter(&AVR32_AST,
			AST_OSC_32KHZ, AST_PSEL_32KHZ_1HZ, ast_counter)) {
		usart_write_line(EXAMPLE_USART,
				"Error initializing the AST\r\n");
		while (1) {
		}
	}

	/* Alarm 0 sends a wakeup signal to the Power manager */
	ast_enable_alarm_async_wakeup(&AVR32_AST, 0);

	/* Enable the AST */
	ast_enable(&AVR32_AST);

	while (1) {
		/* disable alarm 0 */
		ast_disable_alarm0(&AVR32_AST);

		/* ast_init_counter Set Alarm to current time+30 seconds */
		ast_alarm = ast_counter + 1;
		ast_set_alarm0_value(&AVR32_AST, ast_alarm);

		/* Enable alarm 0 */
		ast_enable_alarm0(&AVR32_AST);

		/*
		 * Precautions when entering a sleep mode
		 * Modules communicating with external circuits should normally
		 * be disabled before entering a sleep mode that will stop the
		 * module operation.
		 * Make sure the USART dumps the last message completely before
		 * turning it off.
		 */
		while (!usart_tx_empty(EXAMPLE_USART)) {
		}
		pcl_disable_module(EXAMPLE_USART_CLOCK_MASK);

		/*
		 * Since we're going into a sleep mode deeper than IDLE, all HSB
		 * masters must be stopped before entering the sleep mode.
		 * Note: since we're not using the PDCA, we don't have to stop
		 *it.
		 */

		/*
		 * If there is a chance that any PB write operations are
		 *incomplete,
		 * the CPU should perform a read operation from any register on
		 *the
		 * PB bus before executing the sleep instruction.
		 */
		AVR32_INTC.ipr[0];  /* Dummy read */

		/* Go into static sleep mode */
		SLEEP(AVR32_PM_SMODE_STATIC);

		/* We're out of the static sleep mode now => re-enable the USART
		 * module */
		pcl_enable_module(EXAMPLE_USART_CLOCK_MASK);

		/* After wake up, clear the Alarm0 */
		ast_clear_alarm_status_flag(&AVR32_AST, 0);

		/* Toggle Led0 */
		gpio_tgl_gpio_pin(LED0_GPIO);

		/* Set cursor to the position (1; 6) */
		usart_write_line(EXAMPLE_USART, "\x1B[6;1H");
		ast_counter = ast_get_counter_value(&AVR32_AST);
		usart_write_line(EXAMPLE_USART, "Timer: ");
		ptemp = print_i(temp, ast_counter);
		usart_write_line(EXAMPLE_USART, ptemp);
		usart_write_line(EXAMPLE_USART, " sec ");
	}
}
