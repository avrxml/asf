/**
 * \file
 *
 * \brief XMEGA A3BU Class B demonstration
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

/**
 * \mainpage
 *
 * This application demonstrates the use of Atmel's certified Class B-test
 * algorithms for XMEGA in a simulation of an induction oven on an XMEGA-A3BU
 * Xplained board.
 *
 * For more information about class B compliance with XMEGA and the certified
 * test code, please refer to the application note "AVR1610: Guide to IEC 60730
 * Class B Compliance with XMEGA".
 *
 * \note To operate this demo, it is necessary to mount jumpers between pins
 * ADC2-ADC4 on header J2 and pins RXD-SS on header J1 of the XMEGA-A3BU
 * Xplained.
 *
 * \anchor how_to_enable_crc_test
 * \note The Flash CRC test is commented out in \ref oven_classb_run_tests() in
 * \ref oven_classb.c because the result depends on the compiled code, i.e., it
 * is affected by changes in the toolchain and project settings.
 * To use the test, a precalculated CRC must be retrieved by setting a
 * breakpoint in the function after \c checksum_test_flash is assigned, and its
 * content then copied into \c classb_precalculated_flash_crc, which is stored
 * in EEPROM. Recompile and reprogram the project, and the test will check
 * against the correct value.
 *
 * The application consists of several elements:
 * - The ASF framework: Used for device setup, graphical presentation and use of
 *   the Timer/Counter, ADC and DAC modules
 * - The Class B tests: The certified tests are distributed with the application
 *   note AVR1610
 *     - \ref tests_page
 *     - \ref classb_modules
 * - The oven application consisting of
 *     - main.c: The main function, with initialization and work loop.
 *     - oven_plant.c: A simple discrete model of heat transfer between a plate,
 *     a pot with water and the environment
 *     - oven_controller.c: Emulates the control logic of a real oven
 *     - oven_ui.c: Controls the user interface, draws graphics, reads button
 *     states.
 *     - oven_classb.c: Initialization and execution of Class B tests. Provides
 *     an error test-bench which can induce errors on the device to verify that
 *     the tests catch the errors they are designed to catch.
 *     - oven.h: Application timer module selection and global constants. If the
 *     WDT periods are changed in classb_wdt.h, the watchdog reset period must
 *     be updated in this file to reflect the changes.
 *
 * The Class B tests are set up at the start of the oven_application process,
 * and are called by the oven controller when the user "turns on" the simulated
 * oven by pressing SW1.
 *
 * When power is turned on, indicated by a power bar, some tests are performed
 * periodically.
 *
 * \section operation How to operate the application
 *
 * When running, the controller and simulation plant communicate through two
 * interfaces, DAC to ADC to simulate a temperature sensor, and frequency
 * generation and -capture to simulate the signal an induction cooker
 * would send to the induction elements.
 *
 * The control path is outlined below.
 *
 * \dot
   digraph clock_example {
       UI [label = "User interface"];
       CTL [label = "Oven controller"];
       SIM [label = "Simulation plant"];

       UI -> CTL [label = "Power level"];
       CTL -> SIM [label = "Freq signal"];
       SIM -> UI [label = "ADC reading"];
       SIM -> CTL [label = "ADC reading"];
   }
 * \enddot
 *
 * \section using Using the application.
 *
 * When the oven application is running, the display shows a continuously
 * updated graph of the pot temperature (1) and plate temperature (2), together
 * with an icon that indicates whether the pot is on or off the plate (3) and a
 * bar that visualizes the applied power (4), as exemplified in this screen
 * capture:
 * \image html class_b_demo.png
 * \image latex class_b_demo.png
 *
 * To turn on the power and cycle between the power levels, the user can press
 * SW1 on the upper right hand of the evaluation board. To simulate that a pot
 * is placed on the plate, the user can touch the QTouch button QTB0 on the
 * lower left hand of the board.
 *
 * \section errors Inducing errors in the application
 *
 * By pressing the upper left key, marked SW0, the user enters a menu, allowing
 * the user to choose an error to induce by pressing the Up/Down buttons and
 * selecting with the QTouch button.
 *
 * To recover from an induced error, the device must be reset. This can be done
 * by cycling power on the board or resetting the device via debugger.
 *
 * The entries in the error induction menu are:
 * - Change clock frequency: Changes the peripheral clock divider, simulating
 *   that the clock system has malfunctioned. This should be detected by the
 *   Class B frequency consistency test.
 *
 * - Mess with test timer: Changes how often periodic tests are performed,
 *   simulating an error with an interrupt timer. This should be detected by the
 *   Class B interrupt monitor.
 *
 * - Change a Flash section: Changes the string for the menu title stored in
 *   program memory to "Out of cheese", simulating Flash memory corruption.
 *   This can be changed back by selecting the menu item again. This should be
 *   detected by the Class B Flash CRC test, \ref how_to_enable_crc_test
 *   "if it has been enabled".
 *
 * - Scramble SRAM section: Starts a continuous DMA transfer in the background to
 *   a memory location, simulating transient SRAM corruption. This should be
 *   detected by the Class B SRAM test.
 *
 * - Change ADC reference: Enables an interrupt that will change the ADC voltage
 *   reference after the first successful conversion. This will result in an
 *   unexpected ADC reading being caught in the Analog IO plausibility check
 *   which is run when the "oven" is turned on by pressing the SW1-button on the
 *   board.
 *
 * - Enter infinite loop: Simulates a runaway program counter by looping
 *   forever. This should be detected by the watchdog timer system which is
 *   tested on device power-up.
 *
 * Last, an error can be induced by removing the jumper between ADC2 and ADC4
 * on the J2 header on the XMEGA-A3BU Xplained board.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "oven.h"
#include "oven_controller.h"
#include "oven_plant.h"
#include "oven_ui.h"
#include "oven_classb.h"

/* Class B includes */
#include <error_handler.h>
#include <classb_analog.h>
#include <classb_sram.h>
#include <classb_cpu.h>
#include <classb_freq.h>
#include <classb_interrupt_monitor.h>
#include <classb_rtc_common.h>
#include <classb_crc.h>

/** Default start time for RTC32: 01.01.2011 00:00:00 UTC */
#define FIRST_POSSIBLE_TIMESTAMP 1293840000

/** Global Class B error indicator */
NO_INIT volatile enum classb_error_value classb_error;

/** Global holder for last WDT reset time */
volatile uint32_t classb_last_wdt_reset = 0;

/**
 * \name Information display functions
 *
 * @{
 */

/**
 * \brief Show button names on display
 *
 * This function shows the user what the different on-board buttons do. When
 * the user presses a button, this function exits and the application can
 * continue.
 */
static void show_button_splash(void)
{
	struct keyboard_event input;

	/* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	gfx_mono_draw_filled_circle(6, 6, 3, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(6, 24, 3, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(122, 6, 3, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_filled_circle(122, 24, 3, GFX_PIXEL_SET, GFX_WHOLE);

	gfx_mono_draw_string("Oven power/", 50, 0, &sysfont);
	gfx_mono_draw_string("Up", 94, 8, &sysfont);
	gfx_mono_draw_string("Down", 90, 22, &sysfont);
	gfx_mono_draw_string("Menu/", 12, 0, &sysfont);
	gfx_mono_draw_string("Back", 12, 8, &sysfont);
	gfx_mono_draw_string("Pot/enter", 12, 22, &sysfont);

	/* Any key will exit the loop */
	while (true) {
		oven_wdt_periodic_reset();
		keyboard_get_key_state(&input);
		if (input.type == KEYBOARD_RELEASE) {
			break;
		}
	}
}

/**
 * \brief Show explanation screen
 *
 * This function draws an explanation screen, and is run if the applications are
 * started without the jumpers described in the documentation attached.
 */
static void show_explain_splash(void)
{
	struct keyboard_event input;

	/* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	gfx_mono_draw_string("Class B Demonstration", 1, 4, &sysfont);
	gfx_mono_draw_string("See Application note", 4, 12, &sysfont);
	gfx_mono_draw_string("AVR1610", 40, 20, &sysfont);

	/* Any key will exit the loop */
	while (true) {
		oven_wdt_periodic_reset();
		keyboard_get_key_state(&input);
		if (input.type == KEYBOARD_RELEASE) {
			break;
		}
	}

	/* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
	gfx_mono_draw_string("Attach jumpers betwe-", 0, 4, &sysfont);
	gfx_mono_draw_string("en ADC2-ADC4 on J2,", 0, 12, &sysfont);
	gfx_mono_draw_string("and RXD-SS on J1", 0, 20, &sysfont);

	/* Any key will exit the loop */
	while (true) {
		oven_wdt_periodic_reset();
	}
}

/** @} */

/**
 * \name Hardware initialization functions
 *
 * @{
 */

/**
 * \brief Initialize ADC and DAC used to simulate a temperature sensor
 *
 * DACB is used by the simulation plant to output a temperature reading of the
 * oven plate. It is set up to output a voltage on pin B2 which is marked as
 * ADC2 on header J2.
 *
 * ADCA is used in the control step and graphical interface to show the current
 * temperature of the oven plate. It is set up to read a voltage on pin A4 which
 * is marked as ADC4 on header J2.
 *
 * ADC2 and ADC4 should be connected together, so that the ADC samples the DAC
 * directly.
 */
void main_init_adc_dac(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	struct dac_config dac_conf;

	/* Set up the DAC for the simulation to output "real" temperature */
	dac_read_configuration(&DACB, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0, 0);
	dac_write_configuration(&DACB, &dac_conf);
	dac_enable(&DACB);

	/* Set up the ADC for the controller to read "real" temperature */
	adc_read_configuration(&ADCA, &adc_conf);
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 20000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN4, ADCCH_NEG_NONE, 1);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adc_enable(&ADCA);
	adc_start_conversion(&ADCA, ADC_CH0);

	/* Enable pull-down, so an open circuit can be detected */
	ioport_set_pin_dir(J2_PIN4, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(J2_PIN4, IOPORT_MODE_PULLDOWN);
}

/**
 * \brief Initialize Timer/Counters used to simulate oven actuation signal
 *
 * TCC0 is set up to generate a dual variable frequency signal with dead-time
 * insertion using the AWEX module. This is similar to how heating elements are
 * actuated in real induction ovens. Its output is on pin C2 which is marked as
 * RXD on header J1.
 *
 * TCC1 is set up to capture a frequency signal via a pin change event using the
 * XMEGA Event System. Its input is pin C4 which is marked as SS on header J1.
 */
void main_init_tc(void)
{
	/* Set up timer for PWM output, used to actuate cooking element */
	tc_enable(&OVEN_FREQ_TC);

	/* Configures the waveform generator in Frequency generation mode */
	tc_set_wgm(&OVEN_FREQ_TC, TC_WG_FRQ);

	/* Configures the CCA level. This controls frequency generation */
	tc_write_cc(&OVEN_FREQ_TC, TC_CCA, FREQ_TIMER_PERIOD_INIT / 2);

	/* Enables and configures the deadtime of AWEX channel B outputs */
	tc_awex_enable_ccb_deadtime(&AWEXC);

	tc_awex_set_dti_high(&AWEXC, FREQ_TIMER_PERIOD_INIT / 4);
	tc_awex_set_dti_low(&AWEXC, FREQ_TIMER_PERIOD_INIT / 4);

	/* Output of AWEX channel B is on pins C2 and C3 */
	tc_awex_set_output_override(&AWEXC, 0x0C);

	/* Make sure that the output is initially turned off */
	tc_write_clock_source(&OVEN_FREQ_TC, TC_CLKSEL_OFF_gc);

	/* Set up timer for input capture for the simulation to read "real"
	 * power
	 */
	tc_enable(&OVEN_FREQ_CAPT_TC);
	/* Select Event Channel 1 as input to the timer, and perform frequency
	 * capture.
	 */
	tc_set_input_capture(&OVEN_FREQ_CAPT_TC, TC_EVSEL_CH1_gc,
			TC_EVACT_FRQ_gc);
	/* Enable Capture Channel A */
	tc_enable_cc_channels(&OVEN_FREQ_CAPT_TC, TC_CCAEN);

	/* Make sure pin C4 is configured for input and sensing on rise and fall
	 * and pin C2 is configured for output.
	 */
	ioport_configure_pin(J1_PIN4, IOPORT_DIR_INPUT | IOPORT_BOTHEDGES);
	ioport_configure_pin(J1_PIN2, IOPORT_DIR_OUTPUT);

	/* Turn on power to the event system */
	PR.PRGEN &= ~PR_EVSYS_bm;
	/* Use pin C4 as input to Event Channel 1 */
	EVSYS.CH1MUX = EVSYS_CHMUX_PORTC_PIN4_gc;

	/* Turn on timer used for input capture */
	tc_write_clock_source(&OVEN_FREQ_CAPT_TC, TC_CLKSEL_DIV256_gc);
}

/**
 * \brief Initialize RTC32 and VBAT
 *
 * This function checks the RTC32 and VBAT systems' status, initializing them if
 * necessary. If the status is OK, only the RTC32 is reset.
 *
 * To ensure that WDT resets occur with correct timing, the RTC32 is reset to
 * the last time which was read out from it.
 */
static void main_init_rtc32(void)
{
	/* RTC32 clock must be enabled after sysclk_init() to check VBAT */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
		/* Wait for synchronization */
	}

	/* Initialize the RTC32 and VBAT if its status is not OK */
	if (rtc_vbat_system_check(false) != VBAT_STATUS_OK) {
		rtc_init();
	/* Otherwise, just ensure the RTC32 is enabled with correct period */
	} else {
		RTC32.CTRL = ~RTC32_ENABLE_bm;
		while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
		}
		RTC32.PER = 0xffffffff;
		RTC32.CTRL = RTC32_ENABLE_bm;
	}

	/* Reset the time */
	rtc_set_time(classb_last_wdt_reset);
}

/**
 * \brief Check for presence of required jumpers
 *
 * This function checks that the two jumpers on header J1 and J2 are present.
 * The jumpers should connect:
 * - PC2 to PC4 (RXD to SS on header J1)
 * - PB2 to PA4 (ADC2 to ADC4 on header J2)
 *
 * \return \c true if both jumpers are present, \c false otherwise
 */
static bool main_check_jumpers(void)
{
	/* Check jumper presence by driving one pin high, and setting the other
	 * as input with pull-down
	 */
	ioport_configure_pin(J1_PIN2, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(J1_PIN4, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN);

	ioport_configure_pin(J2_PIN2, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(J2_PIN4, IOPORT_DIR_INPUT | IOPORT_PULL_DOWN);

	if (!ioport_get_value(J1_PIN4) || !ioport_get_value(J2_PIN4)) {
		return false;
	} else {
		return true;
	}
}

/** @} */

/**
 * \brief Executes a simulation step
 *
 * This function executes one step in the simulation of the cooking plate
 * and updates the plot of temperature over time.
 *
 * As input to the plots, an analog reading with ADCA is used to
 * report the temperature of the plate, and the temperature of the
 * pot contents is taken directly from the simulation state vector.
 *
 * \param time RTC Time.
 * \param potstate Tell the simulation whether the pot is on the plate.
 */
static void main_execute_simulation_step(uint32_t time,
		const enum pot_t potstate)
{
	uint8_t pot_temp;
	uint8_t plate_temp;

	/* Simulation step */
	oven_plant_sim_step(potstate);

	/* Get pot temperature directly from simulation model */
	pot_temp = oven_plant_get_state_temp(1);

	/* Get plate temperature from analog reading of "temperature sensor".
	 * This is the value the control-step sees.
	 */
	plate_temp = ovenctl_get_plate_temperature();

	/* Draw temperature readings into the graphing window */
	oven_ui_draw_plot_step(pot_temp, plate_temp);
}

/**
 * \brief Main function.
 *
 * Initializes the board, displays splash screens, then launches application.
 *
 * If the application exits (fails), the error is written to display and an
 * infinite loop with watchdog resets is entered.
 */
int main(void)
{
	/* Holds state of the QTouch button */
	enum pot_t potstate      = POT_OFF;
	/* Last state of the QTouch button for change detection */
	enum pot_t last_potstate = POT_OFF;
	/* Holds user-selected power-setting */
	uint8_t wattage      = 0;
	/* Last power-setting for change detection */
	uint8_t last_wattage = 0;
	/* Whether the plate element is/should be actuated */
	bool power      = false;
	/* Last power setting for change detection */
	bool last_power = false;
	/* Last time a simulation step was executed */
	uint32_t last_sim_step;
	/* Last time a control step was executed */
	uint32_t last_ctl_step;

	/* The WDT was just reset by the WDT functional test*/
	classb_last_wdt_reset = rtc_get_time();

	last_sim_step = classb_last_wdt_reset;
	last_ctl_step = classb_last_wdt_reset;

	sysclk_init();
	board_init();
	pmic_init();
	gfx_mono_init();
	touch_init();
	main_init_rtc32();

	cpu_irq_enable();

	/* Enable display backlight */
	ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN,
			LCD_BACKLIGHT_ENABLE_LEVEL);

	/* If an error was detected, skip directly to displaying it */
	if (classb_error) {
		goto display_error;
	}


	/* Check if required jumpers are mounted, show explanation if not */
	if (!main_check_jumpers()) {
		show_explain_splash();
	}

	/* Display a splash screen showing button functions */
	show_button_splash();

	/* Initialize the ADC, DAC and Timer/Counter modules that are used to
	 * emulate real world objects.
	 */
	main_init_adc_dac();
	main_init_tc();

	/* Initialize subsystems used for Class B testing */
	oven_classb_init_tests();

	/* Reset the simulation states */
	oven_plant_init();

	/* Clear screen */
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	/* Draw the initial axis system */
	oven_ui_draw_axis();

	/* Draw the user interface */
	oven_ui_update_graphics(potstate, wattage, power);

	/* Run simulation as long as no error is detected in class B tests */
	while (!classb_error) {
		uint32_t current_time;

		oven_wdt_periodic_reset();
		current_time = rtc_get_time();

		/* Add power on SW1 press, wrap at 20 */
		if (oven_ui_power_button_is_pressed()) {
			wattage = (wattage + 5) % 20;
		}

		/* Check QTouch sensor and map this to `potstate`. This is
		 * needed both for simulation and for the control routine.
		 */
		potstate = (!touch_key_is_pressed()) ? POT_ON : POT_OFF;

		/* Execute control routine periodically */
		if (current_time > (last_ctl_step + OVEN_CTL_STEP_TIME)) {
			ovenctl_execute_control_step(current_time, &wattage,
					&power, potstate);
			last_ctl_step = current_time;
		}

		/* Execute simulation step periodically */
		if (current_time > (last_sim_step + OVEN_SIM_STEP_TIME)) {
			main_execute_simulation_step(current_time, potstate);
			last_sim_step = current_time;
		}

		/* Update graphics on wattage, power or pot on/off change */
		if ((potstate != last_potstate) || (power != last_power)
				|| (wattage != last_wattage)) {
			oven_ui_update_graphics(potstate, wattage, power);
		}

		/* Update variable states for change detection on next loop
		 * iteration.
		 */
		last_power    = power;
		last_wattage  = wattage;
		last_potstate = potstate;

		/* If back/menu button is pressed, pause simulation and test
		 * timers, and show menu.
		 */
		if (oven_ui_back_button_is_pressed()) {
			/* Disable interrupt monitoring, if enabled */
			classb_intmon_set_state(TEMP_SANITY_TEST, M_DISABLE);
			classb_intmon_set_state(PER_CLASSB_TESTS, M_DISABLE);

			OVEN_PERIODIC_TEMPTEST_TC.CTRLA &= ~TC1_CLKSEL_gm;
			OVEN_PERIODIC_CLASSB_TC.CTRLA &= ~TC0_CLKSEL_gm;

			/* Show the error insertion menu */
			oven_classb_error_insertion();

			/* Re-enable timers upon return */
			OVEN_PERIODIC_CLASSB_TC.CTRLA |= TC_CLKSEL_DIV1024_gc;
			/* If user did not induce an error in the temperature
			 * test timing, re-enable it correctly.
			 */
			if ((OVEN_PERIODIC_TEMPTEST_TC.CTRLA & TC1_CLKSEL_gm)
					== TC_CLKSEL_OFF_gc) {
				OVEN_PERIODIC_TEMPTEST_TC.CTRLA
						|= TC_CLKSEL_DIV1024_gc;
			}

			/* Re-enable interrupt monitoring if the oven is
			 * supposed to be on, i.e., they were enabled before.
			 */
			if (wattage > 0) {
				classb_intmon_set_state(TEMP_SANITY_TEST,
						M_ENABLE);
				classb_intmon_set_state(PER_CLASSB_TESTS,
						M_ENABLE);
			}

			/* Reset UI */
			gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
			oven_ui_draw_axis();
			oven_ui_update_graphics(potstate, wattage, power);
		}
	}

display_error:
	/* Show red status LED and write the error on display */
	oven_ui_set_status_leds(S_RED);
	oven_classb_display_error();

	/* Enter infinite loop of watchdog resets so user can read display */
	while (true) {
		oven_wdt_periodic_reset();
	}
}
