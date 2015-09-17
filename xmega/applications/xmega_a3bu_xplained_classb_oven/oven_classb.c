/**
 * \file
 *
 * \brief Class B Error insertion
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "oven.h"
#include "oven_classb.h"
#include "oven_ui.h"
#include <avr/eeprom.h>
#include <string.h>

/* Class B includes */
#include <error_handler.h>
#include <classb_analog.h>
#include <classb_sram.h>
#include <classb_cpu.h>
#include <classb_freq.h>
#include <classb_interrupt_monitor.h>
#include <classb_rtc_common.h>
#include <classb_crc.h>

/**
 * \name Test-related variables
 *
 * @{
 */

/** Pre-calculated CRC32 value for the flash memory, stored in EEPROM. */
uint32_t classb_precalculated_flash_crc EEMEM = 0x8dbd81d8;

/** Memory location that will be scrambled on SRAM error insertion */
static volatile uint16_t variable_for_sram_error = 0xBEEF;

/** Page buffer used when changing Flash contents */
static uint16_t flash_scramble_buf[FLASH_PAGE_SIZE / 2] = {0xCC};

/**
 * \brief Flash CRC value calculated by Class B test.
 *
 * \note This has been made global and volatile so debuggers will display it
 * correctly.
 */
volatile uint32_t checksum_test_flash = 0;

/** @} */

/**
 * \name Error strings and menu
 *
 * @{
 */

/* Error insertion menu: strings are stored in FLASH,
 * the string pointer table and menu struct are stored in RAM.
 */
PROGMEM_DECLARE(char const, error_menu_title[]) = "-- Error insertion --";
PROGMEM_DECLARE(char const, error_menu_1[]) = "Change Clk frequency";
PROGMEM_DECLARE(char const, error_menu_2[]) = "Mess with test timer";
PROGMEM_DECLARE(char const, error_menu_3[]) = "Change a Flash sect.";
PROGMEM_DECLARE(char const, error_menu_4[]) = "Scramble SRAM sect.";
PROGMEM_DECLARE(char const, error_menu_5[]) = "Enter infinite loop";
PROGMEM_DECLARE(char const, error_menu_6[]) = "Change ADC reference";
PROGMEM_DECLARE(char const, error_menu_7[]) = "Go back";

PROGMEM_STRING_T error_menu_strings[] = {
	error_menu_1,
	error_menu_2,
	error_menu_3,
	error_menu_4,
	error_menu_5,
	error_menu_6,
	error_menu_7,
};

struct gfx_mono_menu error_menu = {
	/* Title */
	error_menu_title,
	/* Array with menu strings */
	error_menu_strings,
	/* Number of menu elements */
	6,
	/* Initial selection */
	0
};

/** Error cause string list enumerator */
enum error_string_ids {
	ERR_NONE, ERR_AIO, ERR_CRC, ERR_FRQ, ERR_INT,
	ERR_CPU, ERR_SRAM, ERR_WDT, ERR_TMP, ERR_OTHER
};

/** Class B error cause strings */
char *classb_error_messages[] = {
	"No error",
	"Analog IO",
	"CRC Checksum",
	"CPU Clock frequency",
	"Interrupt monitor",
	"Register file",
	"SRAM",
	"Watchdog Timer",
	"Temp sanity",
	"Other - unknown"
};

/** @} */

/**
 * \addtogroup oven_classb Class B related
 * \name Class B test initialization and functional test execution
 *
 * @{
 */

/**
 * \brief Initialize Class B tests
 *
 * Set up timers, timer callbacks, initialize interrupt monitor,
 * add interrupts to be monitored.
 */
void oven_classb_init_tests(void)
{
	/* == Frequency consistency test == */

	/* Use Timer D1 for frequency consistency test */
	tc_enable(&CLASSB_FREQTEST_TC);
	tc_set_overflow_interrupt_callback(&CLASSB_FREQTEST_TC,
			classb_freq_tc_callback);
	classb_freq_setup_timer();

	/* == Frequency consistency test and interrupt monitor timer == */

	/* Use relative RTC interrupt from ASF to execute periodic check for
	 * frequency consistency and interrupt monitor.
	 */
	rtc_set_alarm_relative(CLASSB_RTC_INT_PERIOD);
	rtc_set_callback(classb_rtc_callback);
	/* TODO: Workaround: RTC32_COMPINTLVL is reset to OFF for some reason */
	/* after set_alarm_relative is called the the first time. Set to LO. */
	RTC32.INTCTRL = RTC32_COMPINTLVL_LO_gc;

	/* == Interrupt monitor test == */

	/* Register the periodic temperature measurement sanity test in the
	 * monitor.
	 * It's run every 600ms => 3.41 times per RTC compare period of 2 secs.
	 * 25% tolerance.
	 */
	classb_intmon_reg_int(TEMP_SANITY_TEST, 4, 25);
	/* Register the periodic classb test execution in the monitor.
	 * It's run every 1000ms => 2 times per RTC compare period of 2 secs.
	 * 50% tolerance.
	 */
	classb_intmon_reg_int(PER_CLASSB_TESTS, 2, 50);
}

/**
 * \brief Run all Class B tests
 *
 * Destructively test ADC and DAC; modules must be reinitialized for application
 * use. Then test the SRAM, CPU registers and CRC of Flash (if enabled).
 */
void oven_classb_run_tests(void)
{
	oven_ui_set_status_leds(S_ORANGE); /* Tests started */

	/* Test the DAC and ADC used in this application */
	dac_enable(&DACB);
	adc_enable(&ADCA);
	classb_analog_io_test(&DACB, &ADCA);

	/* Disable interrupts globally. Normally one would disable
	 * this also for the analog test, but to emulate an error
	 * we allow interrupts during the analog test.
	 */
	cli();
	for (uint8_t i = 0; i < CLASSB_NSECS; ++i) {
		classb_sram_test();
	}

	/* Test the register file */
	classb_register_test();

	/* Test CRC Checksum */
	/* Uncomment the code below and set a breakpoint on it, then press
	 * F10/Step over to read the calculated checksum. Insert the value in
	 * classb_precalculated_flash_crc at the top of this file and recompile
	 * to get a working CRC check.
	 */
	/* checksum_test_flash = CLASSB_CRC32_Flash_HW (CRC_APP, 0, 0,
	 * &classb_precalculated_flash_crc);
	 */

	sei();

	/* Update status LED according to test results */
	if (classb_error == CLASSB_ERROR_NONE) {
		oven_ui_set_status_leds(S_GREEN);
	} else {
		oven_ui_set_status_leds(S_RED);
	}
}

/** @} */

/**
 * \brief Simulate corruption of a part of Flash
 *
 * This function simulates corruption of the Flash by modifying one of the
 * strings in the error insertion menu. More specifically it replaces the
 * "-- Error insertion --" menu title with "Out of cheese!". If called again,
 * the original string is reverted.
 */
static void oven_classb_flash_corrupter(void)
{
	/* Flash page base address of the menu title string */
	flash_addr_t page_addr = ((uintptr_t)error_menu_title / FLASH_PAGE_SIZE)
			* FLASH_PAGE_SIZE;
	/* Address within the page */
	uintptr_t string_addr = (uintptr_t)error_menu_title % FLASH_PAGE_SIZE;
	/* New string that we will replace it with */
	const char newstring[] = "    Out of cheese!   ";
	const char oldstring[] = "-- Error insertion --";

	/* Load page with strings from flash into our buffer */
	nvm_flash_flush_buffer();
	for (uint16_t addr = 0; addr < FLASH_PAGE_SIZE / 2; addr++) {
		flash_scramble_buf[addr] = nvm_flash_read_word((addr * 2)
				+ page_addr);
		oven_wdt_periodic_reset();
	}

	/* Check which string is the current and change it accordingly */
	if (strcmp((char *)flash_scramble_buf + string_addr, oldstring) == 0) {
		strcpy((char *)flash_scramble_buf + string_addr, newstring);
	} else {
		strcpy((char *)flash_scramble_buf + string_addr, oldstring);
	}

	oven_wdt_periodic_reset();

	/* Fill page buffer with our buffered data, then write it to flash */
	for (uint16_t addr = 0; addr < FLASH_PAGE_SIZE / 2; addr++) {
		nvm_flash_load_word_to_buffer(addr * 2,
				flash_scramble_buf[addr]);
		oven_wdt_periodic_reset();
	}
	nvm_flash_atomic_write_app_page(page_addr);
}

/**
 * \brief Callback function that changes ADC settings
 *
 * This callback function for the ADC driver is used to change the ADC reference
 * after the next completed conversion, simulating a problem with the voltage
 * reference. It will be triggered after the next completed conversion.
 *
 * The fault will be detected when the analog IO test is run, i.e., when the
 * user turns up the power of the plate from 0.
 */
static void adc_foul_callback(ADC_t *adc, uint8_t ch_mask, adc_result_t res)
{
	struct adc_channel_config adcch_conf;

	adc->REFCTRL = ADC_REFSEL_INTVCC_gc;

	adcch_read_configuration(adc, ch_mask, &adcch_conf);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(adc, ch_mask, &adcch_conf);
}

/**
 * \brief Show menu for error insertion and handle user's selection.
 *
 * This function changes device configurations and does some hacks to make
 * the device behave incorrectly.
 *
 * The menu entries are
 *   - Change clock frequency: Changes the peripheral clock divider, simulating
 *     that the clock system has malfunctioned. This should be detected by the
 *     Class B frequency consistency test.
 *
 *   - Mess with test timer: Changes how often periodic tests are performed,
 *     simulating an error with an interrupt timer. This should be detected by
 *     the Class B interrupt monitor.
 *
 *   - Change a Flash section: Changes the string for the menu title stored in
 *     program memory to "Out of cheese", simulating Flash memory corruption.
 *     This can be changed back by selecting the menu item again. This should be
 *     detected by the Class B Flash CRC test.
 *
 *   - Scramble SRAM section: Starts a continuous DMA transfer in the background
 *     to a memory location, simulating transient SRAM corruption. This should
 *     be detected by the periodic and power-on Class B SRAM test.
 *
 *   - Enter infinite loop: Simulates a runaway program counter by looping
 *     forever. This should be detected by the watchdog timer system which is
 *     tested on device power-up.
 *
 *   - Change ADC reference: Enables a callback function for the ADC, which will
 *     change the voltage reference after the next completed conversion. This
 *     will cause the analog IO test to fail when user turns up the power to the
 *     plate.
 */
void oven_classb_error_insertion(void)
{
	uint8_t menu_status;
	struct keyboard_event input;
	struct adc_channel_config adcch_conf;

	/* Initialize menu system */
	gfx_mono_menu_init(&error_menu);

	/* Wait for user to select something in the menu system */
	do {
		do {
			keyboard_get_key_state(&input);
			oven_wdt_periodic_reset();
			/* Wait for key release */
		} while (input.type != KEYBOARD_RELEASE);

		/* Send key to menu system */
		menu_status = gfx_mono_menu_process_key(&error_menu,
				input.keycode);

		oven_wdt_periodic_reset();
	} while (menu_status == GFX_MONO_MENU_EVENT_IDLE);

	/* Handle the user's selection */
	switch (menu_status) {
	case 0:
		/* Change cpu frequency by modifying the prescalers */
		sysclk_set_prescalers(CLK_PSADIV_4_gc, CLK_PSBCDIV_1_1_gc);
		break;

	case 1:
		/* Change timing of the periodic temperature tests */
		OVEN_PERIODIC_TEMPTEST_TC.CTRLA = TC_CLKSEL_DIV256_gc;
		break;

	case 2:
		/* Change flash section. */
		oven_classb_flash_corrupter();
		break;

	case 3:
		/* Disrupt SRAM by setting up the DMA to write to a location on
		 * the heap, triggered by the class B frequency monitor timer
		 */
		PR.PRGEN &= ~PR_DMA_bm;
		DMA.CTRL |= DMA_ENABLE_bm;
		DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_TCD1_CCA_gc;
		/* Address of Timer D1 CNT base is 0x0960. */
		DMA.CH0.SRCADDR0 = 0x60;
		DMA.CH0.SRCADDR1 = 0x09;
		DMA.CH0.SRCADDR2 = 0x00;

		DMA.CH0.DESTADDR0 = ((uint16_t)&variable_for_sram_error) & 0xFF;
		DMA.CH0.DESTADDR1 = (((uint16_t)&variable_for_sram_error) >> 8)
				& 0xFF;
		DMA.CH0.DESTADDR2 = 0x00;

		DMA.CH0.CTRLA = DMA_CH_BURSTLEN_2BYTE_gc | DMA_CH_REPEAT_bm
				| DMA_CH_ENABLE_bm;
		break;

	case 4:
		/* Enter infinite loop */
		while (1) {
		}
		break;

	case 5:
		/* Set up ADC channel interrupt */
		adc_set_callback(&ADCA, adc_foul_callback);
		adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
		adcch_enable_interrupt(&adcch_conf);
		adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

		adcch_read_configuration(&ADCA, ADC_CH2, &adcch_conf);
		adcch_enable_interrupt(&adcch_conf);
		adcch_write_configuration(&ADCA, ADC_CH2, &adcch_conf);
		break;

	case 6:
		/* Back */
		break;

	case GFX_MONO_MENU_EVENT_EXIT:
		/* Fall through to default */
	default:
		/* Nothing, go back. */
		break;
	}
}

/**
 * \brief Print error message depending on the value of classb_error.
 */
void oven_classb_display_error(void)
{
	char *error_string;

	/* Clear the screen */
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	/* Get the error message based on the error value */
	switch (classb_error) {
	case CLASSB_ERROR_NONE:
		error_string = classb_error_messages[ERR_NONE];
		break;

	case CLASSB_ERROR_WDT:
		error_string = classb_error_messages[ERR_WDT];
		break;

	case CLASSB_ERROR_ANALOG:
		error_string = classb_error_messages[ERR_AIO];
		break;

	case CLASSB_ERROR_CRC:
		error_string = classb_error_messages[ERR_CRC];
		break;

	case CLASSB_ERROR_FREQ:
		error_string = classb_error_messages[ERR_FRQ];
		break;

	case CLASSB_ERROR_INT:
		error_string = classb_error_messages[ERR_INT];
		break;

	case CLASSB_ERROR_CPU:
		error_string = classb_error_messages[ERR_CPU];
		break;

	case CLASSB_ERROR_SRAM:
		error_string = classb_error_messages[ERR_SRAM];
		break;

	case CLASSB_ERROR_OTHER:
		error_string = classb_error_messages[ERR_TMP];
		break;

	default:
		error_string = classb_error_messages[ERR_OTHER];
	}

	/* Print the error message on display */
	gfx_mono_draw_string("Error detected in", 0, 8, &sysfont);
	gfx_mono_draw_string(error_string, 0, 16, &sysfont);
}
