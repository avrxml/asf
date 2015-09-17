/**
 * \file
 *
 * \brief Qtouch example for SAM.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 *  \mainpage Qtouch Example
 *
 *  \section Purpose
 *
 *  The Qtouch example demonstrates how to use the Qtouch on the SAM EK boards.
 *
 *  \section Requirements
 *
 *  This package can be used with SAM EK boards on which there is Qtouch keys or a slider.
 *
 *  \section Description
 *
 *  This application will configure the silder bar and Qtouch keys on the SAM EK boards.
 * Whenever touching the keys or the silder, the value will be output the debug terminal.
 *
 *  \section Usage
 *
 *  -# Build the program and download it into the evaluation board. Please
 *     refer to the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *     SAM-BA User Guide</a>, the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *     GNU-Based Software Development</a>
 *     application note or the
 *     <a href="http://www.iar.com/website1/1.0.1.0/78/1/">
 *     IAR EWARM User and reference guides</a>,
 *     depending on the solutions that users choose.
 *  -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# In the terminal window, the
 *     following text should appear (values depend on the board and the chip used):
 *     \code
	-- QTouch Project--
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	supports xxxxxxx
	Maximum number of Rotors/Sliders xxxxx
	Version xxxx
\endcode
 *  -# The application will output the Qtouch value when touching the slider or the key
 * on the EK boards.
 *
 */

#include "asf.h"
#include "conf_example.h"

/** Qtouch library type: Qtouch / QMatrix */
#define QTOUCH_LIB_TPYE_MASK    0x01

/** Qtouch library compiler type offset: GCC / IAR */
#define QTOUCH_LIB_COMPILER_OFFSET    2

/** Qtouch library compiler type mask */
#define QTOUCH_LIB_COMPILER_MASK    0x01

/** Qtouch library maximum channels offset */
#define QTOUCH_LIB_MAX_CHANNEL_OFFSET    3

/** Qtouch library maximum channels mask */
#define QTOUCH_LIB_MAX_CHANNEL_MASK   0x7F

/** Qtouch library supports keys only offset */
#define QTOUCH_LIB_KEY_ONLY_OFFSET    10

/** Qtouch library supports keys only mask */
#define QTOUCH_LIB_KEY_ONLY_MASK   0x01

/** Qtouch library maximum rotors/silders offset */
#define QTOUCH_LIB_ROTOR_NUM_OFFSET    11

/** Qtouch library maximum rotors/silders mask */
#define QTOUCH_LIB_ROTOR_NUM_MASK   0x1F

#define GET_SENSOR_STATE(SENSOR_NUMBER) (qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8)))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Qtouch Project --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/* Flag set by timer ISR when it's time to measure touch */
static volatile uint8_t time_to_measure_touch = 0u;

/* Current time, set by timer ISR */
static volatile uint16_t current_time_ms_touch = 0u;

/* Timer period in msec */
uint16_t qt_measurement_period_msec = 25u;

/**
 * \brief Handler for Sytem Tick interrupt.
 */
void SysTick_Handler(void)
{
	/* Set flag: it's time to measure touch */
	time_to_measure_touch = 1u;

	/* Update the current time */
	current_time_ms_touch += qt_measurement_period_msec;
}

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure PIO */
	gpio_configure_group(CONF_UART_PIO, CONF_PINS_UART, CONF_PINS_UART_FLAGS);

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief initialise pins, watchdog, etc.
 */
static void init_system(void)
{
	/* Disable the watchdog */
	wdt_disable(WDT);

	sysclk_init();

	configure_console();

	/* Output example information */
	printf(STRING_HEADER);

	/* Enable PMC clock for key/silder PIOs  */
#if SAM3XA
	pmc_enable_periph_clk(ID_PIOE);
	pmc_enable_periph_clk(ID_PIOF);
#elif SAM3N
	pmc_enable_periph_clk(ID_PIOC);
#else
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOC);
#endif

}

/**
 * \brief Configure timer ISR to fire regularly. Configure Timer Counter 0 to generate an interrupt every 25ms.
 */
static void init_timer_isr(void)
{
	printf("Configure sys tick to get %u ms tick period.\n\r",
			qt_measurement_period_msec);
	if (SysTick_Config((sysclk_get_cpu_hz() / 1000) *
					qt_measurement_period_msec)) {
		printf("-F- Systick configuration error\n\r");
	}
}

/**
 * \brief This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters.
 */
static void qt_set_parameters(void)
{
	qt_config_data.qt_di = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}

/**
 * \brief Configure the sensors.
 */
static void config_sensors(void)
{
#ifdef BOARD_SILDER_BEFOR_KEY
	qt_enable_slider(BOARD_SLIDER_START_CHANNEL, BOARD_SLIDER_END_CHANNEL,
			AKS_GROUP_1, 16u, HYST_6_25, RES_8_BIT, 0u);
#endif

#if defined BOARD_KEY_NUM_5
	qt_enable_key(BOARD_VALID_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
	qt_enable_key(BOARD_UP_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
	qt_enable_key(BOARD_DOWN_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
	qt_enable_key(BOARD_LEFT_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
	qt_enable_key(BOARD_RIGHT_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
#elif defined BOARD_KEY_NUM_2
	qt_enable_key(BOARD_LEFT_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
	qt_enable_key(BOARD_RIGHT_KEY_CHANNEL, AKS_GROUP_1, 18u, HYST_6_25);
#endif

#ifndef BOARD_SILDER_BEFOR_KEY
	qt_enable_slider(BOARD_SLIDER_START_CHANNEL, BOARD_SLIDER_END_CHANNEL,
			AKS_GROUP_1, 16u, HYST_6_25, RES_8_BIT, 0u);
#endif

}

/**
 * \brief Display information about library used.
 */
static void trace_library_info(void)
{
	qt_lib_siginfo_t plib_sig;

	qt_measure_sensors(current_time_ms_touch);
	qt_get_library_sig(&plib_sig);

	if ((plib_sig.lib_sig_lword & QTOUCH_LIB_TPYE_MASK) == 0) {
		printf("Library QTouch ");
	} else {
		printf("Library QMatrix ");
	}

	if (((plib_sig.lib_sig_lword >> QTOUCH_LIB_COMPILER_OFFSET) &
					QTOUCH_LIB_COMPILER_MASK) == 0) {
		printf("for GCC\n\r");
	} else {
		printf("for IAR\n\r");
	}

	printf("Max Channels %d\n\r",
			(plib_sig.lib_sig_lword >>
					QTOUCH_LIB_MAX_CHANNEL_OFFSET) &
			QTOUCH_LIB_MAX_CHANNEL_MASK);

	if (((plib_sig.lib_sig_lword >> QTOUCH_LIB_KEY_ONLY_OFFSET) &
					QTOUCH_LIB_KEY_ONLY_MASK) == 0) {
		printf("supports only keys \n\r");
	} else {
		printf("supports keys and rotors/sliders\n\r");
	}

	printf("Maximum number of Rotors/Sliders %d \n\r",
			(plib_sig.lib_sig_lword >> QTOUCH_LIB_ROTOR_NUM_OFFSET)
			& QTOUCH_LIB_ROTOR_NUM_MASK);

	printf("Version 0x%X\n\r", plib_sig.library_version);
	printf("\n\r");
}

/**
 * \brief Main code entry point.
 */
int main(void)
{
	/*Status flags to indicate the re-burst for library */
	uint16_t status_flag = 0u;
	uint16_t burst_flag = 0u;

#if defined BOARD_KEY_NUM_5
	uint8_t vld_pressed = 0;
	uint8_t up_pressed = 0;
	uint8_t dwn_pressed = 0;
	uint8_t lft_pressed = 0;
	uint8_t rgt_pressed = 0;
#elif defined BOARD_KEY_NUM_2
	uint8_t lft_pressed = 0;
	uint8_t rgt_pressed = 0;
#endif

	static uint8_t old_position = 0;

	/* Initialise host app, pins, watchdog, etc */
	init_system();

	/* Reset touch sensing */
	qt_reset_sensing();

	/* Configure the Sensors as keys or Keys With Rotor/Sliders in this function */
	config_sensors();

	/* Initialise touch sensing */
	qt_init_sensing();

	/* Set the parameters like recalibration threshold, Max_On_Duration etc in this function by the user */
	qt_set_parameters();

	/* Configure timer ISR to fire regularly */
	init_timer_isr();

	/* Address to pass address of user functions */
	/* This function is called after the library has made capacitive measurements,
	 * but before it has processed them. The user can use this hook to apply filter
	 * functions to the measured signal values.(Possibly to fix sensor layout faults) */
	qt_filter_callback = 0;

	trace_library_info();

	/* Loop forever */
	for (;;) {

		if (time_to_measure_touch) {

			/* Clear flag: it's time to measure touch */
			time_to_measure_touch = 0u;

			do {
				/*  One time measure touch sensors    */
				status_flag = qt_measure_sensors
						(current_time_ms_touch);

				burst_flag = status_flag & QTLIB_BURST_AGAIN;

				/*Time critical host application code goes here */

			} while (burst_flag);
		}

		/*  Time Non-critical host application code goes here */
#if defined BOARD_KEY_NUM_5
		if ((GET_SENSOR_STATE(BOARD_VALID_KEY_ID) != 0)
				&& (vld_pressed == 0)) {
			vld_pressed = 1;
			printf("Valid Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_VALID_KEY_ID) == 0)
					&& (vld_pressed == 1)) {
				printf("Valid Release \n\r");
				vld_pressed = 0;
			}
		}
		if ((GET_SENSOR_STATE(BOARD_UP_KEY_ID) != 0)
				&& (up_pressed == 0)) {
			up_pressed = 1;
			printf("Up Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_UP_KEY_ID) == 0)
					&& (up_pressed == 1)) {
				printf("Up Key Release \n\r");
				up_pressed = 0;
			}
		}
		if ((GET_SENSOR_STATE(BOARD_DOWN_KEY_ID) != 0)
				&& (dwn_pressed == 0)) {
			dwn_pressed = 1;
			printf("Down Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_DOWN_KEY_ID) == 0)
					&& (dwn_pressed == 1)) {
				printf("Down Key Release \n\r");
				dwn_pressed = 0;
			}
		}
		if ((GET_SENSOR_STATE(BOARD_LEFT_KEY_ID) != 0)
				&& (lft_pressed == 0)) {
			lft_pressed = 1;
			printf("Left Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_LEFT_KEY_ID) == 0)
					&& (lft_pressed == 1)) {
				printf("Left Key Release \n\r");
				lft_pressed = 0;
			}
		}
		if ((GET_SENSOR_STATE(BOARD_RIGHT_KEY_ID) != 0)
				&& (rgt_pressed == 0)) {
			rgt_pressed = 1;
			printf("Right Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_RIGHT_KEY_ID) == 0)
					&& (rgt_pressed == 1)) {
				printf("Right Key Release \n\r");
				rgt_pressed = 0;
			}
		}
#elif defined BOARD_KEY_NUM_2
		if ((GET_SENSOR_STATE(BOARD_LEFT_KEY_ID) != 0)
				&& (lft_pressed == 0)) {
			lft_pressed = 1;
			printf("Left Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_LEFT_KEY_ID) == 0)
					&& (lft_pressed == 1)) {
				printf("Left Key Release \n\r");
				lft_pressed = 0;
			}
		}
		if ((GET_SENSOR_STATE(BOARD_RIGHT_KEY_ID) != 0)
				&& (rgt_pressed == 0)) {
			rgt_pressed = 1;
			printf("Right Key Press \n\r");
		} else {
			if ((GET_SENSOR_STATE(BOARD_RIGHT_KEY_ID) == 0)
					&& (rgt_pressed == 1)) {
				printf("Right Key Release \n\r");
				rgt_pressed = 0;
			}
		}
#endif

		if (GET_ROTOR_SLIDER_POSITION(0) != old_position) {
			old_position = GET_ROTOR_SLIDER_POSITION(0);
			printf("%d ", old_position);
		}
#if defined (  __GNUC__  )
		fflush(stdout);
#endif
	}
}
