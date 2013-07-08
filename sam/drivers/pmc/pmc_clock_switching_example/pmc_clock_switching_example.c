/**
 * \file
 *
 * \brief PMC Clock Switching example for SAM.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
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
 * \mainpage PMC Clock Switching example
 *
 * \section intro Introduction
 *
 * This example shows how to switch from a clock to another (PLL, SLCK, MAINCK)
 * or change divider.
 *
 * \section Requirements
 *
 * This package can be used with SAM evaluation kits.
 *
 * \section files Main files:
 *  - pmc.c PMC SAM Power Management Controller driver implementation.
 *  - pmc.h PMC SAM Power Management Controller driver definitions.
 *  - pmc_clock_switching_example.c Example application.
 *
 * \section exampledescription Description of the Example
 * Upon startup, the program configures PIOs for UART, PCK and buttons. The
 * baud rate of UART is configured as 2400 bps. The application performs some
 * printf actions with the current configuration (except 32KHz slow clock )
 * and waits for the button pressed to switch to the next configuration. The
 * PCK clock can be selected among PLLA, UPLL, SLCK, MAINCK and driven on the
 * PCK0 pin. After the clock switches, the PCK output signal can be measured by
 * oscilloscope and compared with the clock configuration.
 *
 * <ul>
 * <li> The Clock Generator integrates a 32,768 Hz low-power oscillator.
 * In order to use this oscillator, the XIN32 and XOUT32 pins must be connected
 * to a 32,768 Hz crystal. The user can select the crystal oscillator to be the
 * source of the slow clock, as it provides a more accurate frequency. The
 * switch is made by function pmc_switch_sclk_to_32kxtal().</li>
 * <li> MAINCK is the output of the Main Clock Oscillator selection: either
 * the Crystal Oscillator or 4/8/12 MHz Fast RC Oscillator. The user can
 * select the output frequency of the Fast RC Oscillator: either 4 MHz, 8 MHz
 * or 12 MHz is available. It can be done through function
 * pmc_switch_mainck_to_fastrc(). The user can also select the 3 to 20 MHz
 * Crystal oscillator to be the source of MAINCK, as it provides a more
 * accurate frequency. The function pmc_switch_mainck_to_xtal() shows how to
 * enable the oscillator. </li>
 * <li> The PMC features a PLL block that permits a wide range of frequencies
 * to be selected on either the master clock, the processor clock or the
 * programmable clock. Function pmc_switch_mck_to_pllack() can be used to
 * configure the PLLA.</li>
 * </ul>
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR EWARM.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 * Support and FAQ: http://support.atmel.no/
 *
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_pmc_clock_switching_example.h"

/** User push button activated flag */
static volatile uint8_t gs_uc_wait_button = 0;

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/** PMC External Xtal 12Mhz */
#define PMC_CLOCK_SWITCHING_EXAMPLE_BAUDRATE (2400)

/** Fixed PLLA test clock, 128Mhz */
#define PMC_CLOCK_SWITCHING_EXAMPLE_FIXED_PLLA	(128000000)

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Pmc Clock Switching Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#if !defined(PMC_PCK_PRES_CLK_1)
#define PMC_PCK_PRES_CLK_1   PMC_PCK_PRES(0)
#define PMC_PCK_PRES_CLK_2   PMC_PCK_PRES(1)
#define PMC_PCK_PRES_CLK_4   PMC_PCK_PRES(2)
#define PMC_PCK_PRES_CLK_8   PMC_PCK_PRES(3)
#define PMC_PCK_PRES_CLK_16  PMC_PCK_PRES(4)
#define PMC_PCK_PRES_CLK_32  PMC_PCK_PRES(5)
#define PMC_PCK_PRES_CLK_64  PMC_PCK_PRES(6)
#endif

/**
 * \brief Handler for Button 1 rising edge interrupt.
 *
 * Handle process led1 status change.
 */
static void button1_handler(uint32_t ul_id, uint32_t ul_mask)
{
	if (PIN_PUSHBUTTON_1_ID == ul_id && PIN_PUSHBUTTON_1_MASK == ul_mask)
		gs_uc_wait_button = 0;
}

/**
 * \brief Configure the Push buttons
 *
 * Configure the PIO as inputs and generate corresponding interrupt when
 * pressed or released.
 */
static void configure_buttons(void)
{
	/* Enable the peripheral clock for PCK and push button */
	pmc_enable_periph_clk(PIN_PUSHBUTTON_1_ID);

	/* Adjust PIO debounce filter parameters, using 10 Hz filter. */
	pio_set_debounce_filter(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK, 10);

	/* Initialize PIOs interrupt handlers, see PIO definition in board.h. */
	/* Interrupt on rising edge  */
	pio_handler_set(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_ID,
			PIN_PUSHBUTTON_1_MASK, PIN_PUSHBUTTON_1_ATTR, button1_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type) PIN_PUSHBUTTON_1_ID);

	/* Enable PIO line interrupts. */
	pio_enable_interrupt(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK);
}

/**
 *  \brief Configure the Console Uart
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}


/**
 * \brief Configure UART with the given master clock, and Configure PCK with
 *  the given divider source of master clock and prescaler.
 *
 * \param ul_clock_source  The master clock divider source.
 * \param ul_prescaler Master Clock prescaler.
 * \param ul_master_clock Frequency of the master clock (in Hz).
 */
static void config_uart_and_pck(uint32_t ul_clock_source,
		uint32_t ul_prescaler, uint32_t ul_master_clock)
{
	if (ul_master_clock > BOARD_FREQ_SLCK_XTAL) {
		const sam_uart_opt_t uart_console_settings = {
			ul_master_clock, PMC_CLOCK_SWITCHING_EXAMPLE_BAUDRATE,
			UART_MR_PAR_NO
		};

		/* Configure PMC */
		pmc_enable_periph_clk(CONSOLE_UART_ID);

		/* Configure UART */
		uart_init(CONSOLE_UART, &uart_console_settings);
	}

	/* Programmable clock 0 output disabled */
	pmc_disable_pck(PMC_PCK_0);

	/* Configure PMC Programmable Clock */
	switch (ul_clock_source) {
	case PMC_PCK_CSS_MAIN_CLK:
		pmc_switch_pck_to_mainck(PMC_PCK_0, ul_prescaler);
		break;

	case PMC_PCK_CSS_SLOW_CLK:
		pmc_switch_pck_to_sclk(PMC_PCK_0, ul_prescaler);
		break;

	case PMC_PCK_CSS_PLLA_CLK:
		pmc_switch_pck_to_pllack(PMC_PCK_0, ul_prescaler);
		break;

#if SAM3S
	case PMC_PCK_CSS_PLLB_CLK:
		pmc_switch_pck_to_pllbck(PMC_PCK_0, ul_prescaler);
		break;
#endif

	default:
		pmc_switch_pck_to_mainck(PMC_PCK_0, ul_prescaler);
	}

	/* Enable the PCK again */
	pmc_enable_pck(PMC_PCK_0);
}

/**
 * \brief Delay for a few ticks. Wait for the given number of milliseconds.
 *
 * \note The system tick is not used here because the system tick interrupt might
 * introduce instability to the system during clock switching.
 *
 * \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
static void delay_ticks(uint32_t ul_dly_ticks)
{
	/* SAM3U has a faster speed, needs more time to make the clock stable */
#if SAM3U
	ul_dly_ticks *= 2;
#endif

	/* the systick is not used for it is an output of the mck */
	while (ul_dly_ticks--) {
	}
}

/**
 * \brief Application entry point for pmc_clock switch example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure PCK0 */
	ioport_set_pin_mode(PIN_PCK0, PIN_PCK0_MUX);
	ioport_disable_pin(PIN_PCK0);

	/* Configure the push button */
	configure_buttons();

	puts("-I- Press Button "BUTTON_NAME" to continue.\r\n");
	delay_ticks(5000);
	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("\n\r-I- Switch 8Mhz fast RC oscillator to be the source of the main clock \n\r"
			"-I- The master clock is main clock divided by 2\n\r"
			"-I- From now on, the UART baud rate is 2400bps. So please change the terminial setting before the next clock switch\r\n"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration... \r\n");
	delay_ticks(20000);

	/* First switch to slow clock */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

#if SAM3S
	/* Then cut the PLL B */
	pmc_disable_pllbck();
#endif

	/* Switch the mainck clock to the Fast RC, parameter '1' stands for 8Mhz */
	pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_8_MHz);

	/* And finalize by switching to Fast RC */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_2);

	/* The clock source for the UART is the PCK, so the uart needs re-configuration */
	config_uart_and_pck(PMC_PCK_CSS_MAIN_CLK, PMC_PCK_PRES_CLK_2,
			(CHIP_FREQ_MAINCK_RC_8MHZ / 2));

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("\n\r-I- Switch the XTAL 32K crystal oscillator to be the source of the slow clock\n\r"
			"-I- The master clock is slow clock\n\r"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration after it has been measured.\r\n");
	delay_ticks(8000);

	/* Enable the External 32K oscillator */
	pmc_switch_sclk_to_32kxtal(PMC_OSC_XTAL);

	/* If a new value for CSS field corresponds to Main Clock or Slow Clock,
	 * program the CSS field first.
	 */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

	/* The clock source for the UART is the PCK, so the uart needs
	 *re-configuration.
	 */
	config_uart_and_pck(PMC_PCK_CSS_SLOW_CLK, PMC_PCK_PRES_CLK_1,
			BOARD_FREQ_SLCK_XTAL);

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	/* Switch the mainck to the Fast RC, parameter '2' stands for 12Mhz */
	pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_12_MHz);

	/* If a new value for CSS field corresponds to Main Clock or Slow Clock,
	 * program the CSS field first.
	 */
	pmc_switch_mck_to_mainck(PMC_PCK_PRES_CLK_1);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration.
	 */
	config_uart_and_pck(PMC_PCK_CSS_MAIN_CLK, PMC_PCK_PRES_CLK_1,
			CHIP_FREQ_MAINCK_RC_12MHZ);
	puts("\n\r-I- Switch 12Mhz fast RC oscillator to be the source of the main clock\n\r"
			"-I- The master clock is the main clock\n\r"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration after it has been measured.\r\n");

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("-I- Switch to 128Mhz PLLA clock as the source of the master clock \n\r"
			"-I- The master clock is PLLA clock divided by 2 \n\r"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration... \r\n");
	delay_ticks(8000);

	/* Enable the PLLA clock, the mainck equals 12Mhz * (32-1+1) / 3 = 128Mhz */
	pmc_enable_pllack((32 - 1), 0x3f, 3);

	/* If a new value for CSS field corresponds to PLL Clock, Program the PRES
	 * field first.
	 */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_2);

	/* Delay for a while */
	delay_ticks(10000);

	/* Then program the CSS field. */
	pmc_switch_mck_to_pllack(PMC_MCKR_PRES_CLK_2);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration
	 */
	config_uart_and_pck(PMC_PCK_CSS_PLLA_CLK, PMC_PCK_PRES_CLK_2,
			PMC_CLOCK_SWITCHING_EXAMPLE_FIXED_PLLA/2);

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("\n\r-I- Switch the XTAL 32K crystal oscillator to be the source of the slow clock\n\r"
			"-I- The master clock is slow clock\n\r"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration...\r\n");
	delay_ticks(5000);

	/* Switch slow clck to extern 32k xtal */
	pmc_switch_sclk_to_32kxtal(PMC_OSC_XTAL);

	/* Delay for a while to make sure the clock is stable */
	delay_ticks(10000);

	/* If a new value for CSS field corresponds to Main Clock or Slow Clock,
	 * program the CSS field first.
	 */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_2);

	/* Switch the mck to sclk but keep the PRES field same */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_2);

	/* Then program the PRES field. */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration
	 */
	config_uart_and_pck(PMC_PCK_CSS_SLOW_CLK, PMC_PCK_PRES_CLK_1,
			BOARD_FREQ_SLCK_XTAL);

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	/* Switch mainck to external xtal */
	pmc_switch_mainck_to_xtal(0, BOARD_OSC_STARTUP_US);
	/* If a new value for CSS field corresponds to Main Clock or Slow Clock,
	 * program the CSS field first.
	 */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_1);
	/* Then program the PRES field. */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_16);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration.
	 */
	config_uart_and_pck(PMC_PCK_CSS_MAIN_CLK, PMC_PCK_PRES_CLK_16,
			(BOARD_FREQ_MAINCK_XTAL / 16));
	puts("\n\r-I- Switch the external 12MHz crystal oscillator to be the source of the main clock\n\r"
			"-I- The master clock is main  clock divided by 16\n\r"
			"-I- Press Button "BUTTON_NAME" to switch next clock configuration...\r\n");

#if SAM3S
	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("-I- Switch to 96Mhz PLLB clock as the source of the master clock\n\r"
			"-I- The master clock is PLLB clock divided by 2 \r\n");
	delay_ticks(5000);

	/* Enable the PLLB clock, the mainck equals (12Mhz * (7+1) / 1) = 96Mhz
	 * with Initialize counter 0x3f.
	 */
	pmc_enable_pllbck(7, 0x3f, 1);

	/* If a new value for CSS field corresponds to PLL Clock, Program the PRES
	 * field first.
	 */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_2);

	/* Then program the CSS field. */
	pmc_switch_mck_to_pllbck(PMC_MCKR_PRES_CLK_2);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration.
	 */
	config_uart_and_pck(PMC_PCK_CSS_PLLB_CLK, PMC_PCK_PRES_CLK_2,
			(BOARD_FREQ_MAINCK_XTAL * 8 / 2));
	puts("-I- Press Button "BUTTON_NAME" to switch next clock configuration...\r\n");
#endif

	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("-I- Switch 8Mhz fast RC oscillator to be the source of the main clock\n\r"
			"-I- The master clock is main clock \r\n");
	delay_ticks(1000);

	/* Switch the mainck clock to the Fast RC, parameter '1' stands for 8Mhz */
	pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_8_MHz);

	/* If a new value for CSS field corresponds to Main Clock or Slow Clock,
	 * program the CSS field first.
	 */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_2);
	/* Delay for a while */
	delay_ticks(1000);
	/* Then program the PRES field. */
	pmc_switch_mck_to_mainck(PMC_MCKR_PRES_CLK_1);

	/* Delay for a while */
	delay_ticks(1000);

	/* The clock source for the UART is the PCK, so the uart needs
	 * re-configuration.
	 */
	config_uart_and_pck(PMC_PCK_CSS_MAIN_CLK, PMC_PCK_PRES_CLK_1,
			CHIP_FREQ_MAINCK_RC_8MHZ);
	for (gs_uc_wait_button = 1; gs_uc_wait_button;) {
	}

	puts("\r\n\n\r-I- Done.\r");

    /* Delay for a while */
	delay_ticks(1000);

	while (1) {
	}
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
