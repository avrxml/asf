/**
 * \file
 *
 * \brief FREQM example driver for AVR UC3.
 *
 * This file provides an example for the FREQM on AVR UC3 devices.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the FREQM driver.
 *
 * The Frequency Meter(FREQM) can be used to accurately measure the frequency
 * of a clock by comparing it to a known reference clock.
 *
 * \section files Main Files
 * - freqm.c : FREQM driver
 * - freqm.h : FREQM header file
 * - freqm_example.c : FREQM code example
 *
 * \section description Description of the example
 *
 * This example shows the user how to use frequency meter
 * - OSC32K set as reference clock(32768Hz)
 * - CPU,HSB,PBA,PBB set as measured clock
 * - Results output through usart:
 *   - UC3C_EK: main clock 16Mhz
 *     - CPU, HSB, PBA equals main clock 16MHz
 *     - PBB equals main clock 16Mhz
 *     - PBB equals main clock divided by 16 after set PBB clock division
 *   - UC3L_EK: main clock 96Mhz
 *     - CPU, HSB, PBA equals main clock divided by 8, 12Mhz
 *     - PBB equals main clock divided by 8, 12Mhz
 *     - PBB equals main clock divided by 16 after set PBB clock division
 *   - STK600-RCUC3D: main clock 12Mhz
 *     - CPU, HSB, PBA equals main clock 12MHz
 *     - PBB equals main clock 12Mhz
 *     - PBB equals main clock divided by 16 after set PBB clock division
 *
 * - Two mode support
 *   - Normal Mode: default mode
 *   - Interrupt Mode: open INTERRUPT_MODE_SUPPORT
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR UC3. Other compilers may or may not work.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK and UC3L_EK evaluation kit or STK600-RCUC3D;
 * - CPU clock: 16 MHz(UC3C_EK), 12MHz(UC3L_EK, STK600-RCUC3D);
 * - USART2 (UC3C_EK),USART3(UC3L_EK) or RS232 (STK600-RCUC3D) abstracted with a USB CDC connection
 * to a PC;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit">Atmel AVR UC3</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "usart.h"
#include "freqm.h"
#include "print_funcs.h"
#include "power_clocks_lib.h"
#include "conf_board.h"

#define OSC32K_FREQ_HZ                   32768

#ifdef INTERRUPT_MODE_SUPPORT
static uint32_t flag = 1;
static uint32_t value = 0;
#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void freqm_int_handler(void)
{
	value = AVR32_FREQM.value;
	flag = 0;
	AVR32_FREQM.icr = AVR32_FREQM_ICR_DONE_MASK;
}

#endif

/**
 * \brief Output result through usart
 *
 *  \param refhz Frequency of reference clock(Hz)
 *  \param duration Reference clock cycles
 *  \param msrhz Frequency of measured clock(Hz)
 */
void display_result(uint32_t refhz, uint32_t duration, uint32_t msrhz)
{
	print_dbg("Reference Clock: ");
	print_dbg_ulong(refhz);
	print_dbg(" Hz\r\n");

	print_dbg("Measured Time: ");
	print_dbg_ulong(duration);
	print_dbg(" reference clock cycle\r\n");

	print_dbg("Measured Clock: ");
	print_dbg_ulong(msrhz);
	print_dbg(" Hz\r\n\n");
}

/**
 * \brief Measured and get result after configuration
 *
 * \return Result of measured clock frequency(Hz)
 */
int clock_measure(void)
{
	uint32_t result;
	status_code_t status;
	freqm_start();
	status = freqm_get_result(&result, OSC32K_FREQ_HZ);
	if (status == ERR_TIMEOUT) {
		usart_write_line(EXAMPLE_USART, "FREQM Module Get Result Fail!");
		while (true);
	}
	return result;
}

/**
 * \brief main function
 */
int main(void)
{
	static const gpio_map_t USART_GPIO_MAP = {
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	// USART options
	static const usart_options_t USART_OPTIONS = {
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = 0
	};

#if BOARD == UC3L_EK

	scif_osc32_opt_t  opt_osc32 = {
		// 2-pin Crystal and high current mode
		SCIF_OSC_MODE_2PIN_CRYSTAL_HICUR,
		// oscillator startup time
		AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC,
		/*
		 * select the alternate xin32_2 and xout32_2 for the
		 * 32kHz crystal oscillator
		 */
		true,
		// disable the 1kHz output
		false,
		// enable the 32kHz output
		true
	};

#else
	scif_osc32_opt_t opt_osc32 = {
		// 2-pin crystal mode
		.mode = SCIF_OSC_MODE_2PIN_CRYSTAL,
		// startup time
		.startup = AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC
	};
#endif
	// Set system clock
	sysclk_init();

	// Start OSC_32KHZ
	scif_start_osc32(&opt_osc32,true);

	// Assign GPIO pins to USART0.
	gpio_enable_module(USART_GPIO_MAP,
		sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FPBA);

	// Welcome message
	usart_write_line(EXAMPLE_USART, "\x1B[2J\x1B[H\r\nATMEL\r\n");
	usart_write_line(EXAMPLE_USART, "AVR UC3 - FREQM example\r\n");

#ifndef INTERRUPT_MODE_SUPPORT
	uint32_t result;
	status_code_t status;
	uint32_t duration = 128;
	usart_write_line(EXAMPLE_USART, "/*** Normal Mode ***/\r\n");
	freqm_enable();
	status = freqm_write_config(AVR32_FREQM_REF_OSC32, AVR32_FREQM_CPU,
			duration);
	if (status == ERR_TIMEOUT) {
		usart_write_line(EXAMPLE_USART, "FREQM Module Config Fail!");
		while(true);
	}
	result = clock_measure();
	usart_write_line(EXAMPLE_USART, "CPU Clock:\r\n");
	display_result(OSC32K_FREQ_HZ, duration, result);

	usart_write_line(EXAMPLE_USART, "HSB Clock:\r\n");
	freqm_set_clock_source(AVR32_FREQM_HSB);
	result = clock_measure();
	display_result(OSC32K_FREQ_HZ, duration, result);

	usart_write_line(EXAMPLE_USART, "PBA Clock:\r\n");
	freqm_set_clock_source(AVR32_FREQM_PBA);
	result = clock_measure();
	display_result(OSC32K_FREQ_HZ, duration, result);

	usart_write_line(EXAMPLE_USART, "PBB Clock:\r\n");
	freqm_set_clock_source(AVR32_FREQM_PBB);
	result = clock_measure();
	display_result(OSC32K_FREQ_HZ, duration, result);

	pm_set_clk_domain_div(PM_CLK_DOMAIN_3,PM_CKSEL_DIVRATIO_16);
	usart_write_line(EXAMPLE_USART, "PBB Clock(Main clock/16):\r\n");
	freqm_set_clock_source(AVR32_FREQM_PBB);
	result = clock_measure();
	display_result(OSC32K_FREQ_HZ, duration, result);

	usart_write_line(EXAMPLE_USART, "Test Complete!\r\n");
	while (true) {
		/*
		 * Force a NOP instruction for an eventual placement of a debug
		 * session breakpoint.
		 */
		asm("nop\n");
	}
#else
	INTC_init_interrupts();
	INTC_register_interrupt(&freqm_int_handler, AVR32_FREQM_IRQ,
			AVR32_INTC_INT0);
	freqm_enable_measurement_done_int();
	cpu_irq_enable();
	usart_write_line(EXAMPLE_USART, "/*** Interrupt Mode ***/\r\n");
	uint32_t duration = 128;
	uint32_t result;
	status_code_t status;
	freqm_enable();
	status = freqm_write_config(AVR32_FREQM_REF_OSC32, AVR32_FREQM_CPU,
			duration);
	if (status == ERR_TIMEOUT) {
		usart_write_line(EXAMPLE_USART, "FREQM Module Config Fail!");
		while(true);
	}
	freqm_start();
	while(true) {
		if(flag == 0) {
			result =(uint32_t)(((F32)value / duration)
					* OSC32K_FREQ_HZ);
			usart_write_line(EXAMPLE_USART, "CPU Clock:\r\n");
			display_result(OSC32K_FREQ_HZ, duration, result);
			usart_write_line(EXAMPLE_USART, "Test Complete!\r\n");
			flag = 1;
		}
		/*
		 * Force a NOP instruction for an eventual placement of a debug
		 * session breakpoint.
		 */
		asm("nop\n");
	};
#endif
  return 0;
}
