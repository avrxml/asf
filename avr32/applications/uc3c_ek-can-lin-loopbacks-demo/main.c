/**
 * \file
 *
 * \brief AVR UC3C CAN-LIN Loopback Demo
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
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for CAN and LIN loopback demo application running
 * on the UC3C_EK development kit.
 * - Step 1: The potentiometer value is read and displayed on the LCD screen.
 * - Step 2: The value of the potentiometer is sent through CAN network.
 * - Step 3: The content of the received message on CAN network is displayed on
 *   the LCD screen and corresponds to the screen observed in step 1.
 * - Step 4: The same message is also transmitted on LIN bus
 * - Step 5: The content of the message received on LIN network is displayed on
 *   the LCD screen and should correspond to the screen observed on step 1,3.
 * \section files Main Files
 * - main.c: Main File;
 * - can_task.c: CAN Task Management;
 * - lin_task.c: LIN Task Management;
 * - controller.c: Qtouch Controller Management;
 * - gui.c: GUI Display Management;
 * - <A href="http://www.atmel.com/dyn/resources/prod_documents/doc32137.pdf"
 *   style="text-decoration:none"><b>doc32137.pdf</b></A>: AVR32907 UC3C-EK
 *   Getting Started file - how to setup and run this application;
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 AT32UC3C devices can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 16 MHz;
 * - USART2 (on UC3C_EK) abstracted with a USB CDC connection to a PC;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "dsp.h"
#include "gui.h"
#include "controller.h"
#include "gpio.h"
#include "print_funcs.h"
#include "flashc.h"
#include "adcifa.h"
#include "twim.h"
#include "conf_at42qt1060.h"
#include "lin_task.h"
#include "can_task.h"
#include "conf_demo.h"

A_ALIGNED dsp16_t signal1_buf[BUFFER_LENGTH];
A_ALIGNED dsp16_t signal4_buf[BUFFER_LENGTH];
volatile U16 adc_current_conversion;
/*! \brief Initializes the MCU system clocks.
 */
void init_sys_clocks(void)
{
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
}

volatile bool input_fft_view = false;
volatile bool output_fft_view = false;
volatile bool zoom_view = false;
volatile int32_t zoom_view_id;

enum state_master {
	STATE_IDLE,
	STATE_SOURCE1,
	STATE_OUTPUT1,
	STATE_OUTPUT2,
	STATE_OUTPUT3
};

enum state_function {
	STATE_FCT_IDLE,
	STATE_FCT_FUNCTION1,
	STATE_FCT_FUNCTION2,
	STATE_FCT_FUNCTION3,
	STATE_FCT_FUNCTION4,
	STATE_FCT_ZOOM
};

static enum state_master state = STATE_IDLE;
static enum state_function state_fct = STATE_FCT_IDLE;
static bool new_state_fct = false;

/*! \brief Global State Machine:
 *        - Function Idle
 *        - Function Zoom
 */
static bool state_machine_global(int source_id, enum state_function *state)
{
	switch (*state) {
		case STATE_FCT_IDLE:
			if (source_id == GUI_SOURCE1_ID) {
				if (new_state_fct) {
					gui_set_selection(GUI_SOURCE1_ID);
				}
			}
			else if (source_id == GUI_OUTPUT1_ID) {
				if (new_state_fct) {
					gui_set_selection(GUI_OUTPUT1_ID);
				}
			}
			else if (source_id == GUI_OUTPUT2_ID) {
				if (new_state_fct) {
					gui_set_selection(GUI_OUTPUT2_ID);
				}
			}
			else if (source_id == GUI_OUTPUT3_ID) {
				if (new_state_fct) {
					gui_set_selection(GUI_OUTPUT3_ID);
				}
			}
			break;
		// Not Implemented
		case STATE_FCT_FUNCTION1:
			break;
		// Not Implemented
		case STATE_FCT_FUNCTION2:
			break;
		// Not Implemented
		case STATE_FCT_FUNCTION3:
			break;
		// Not Implemented
		case STATE_FCT_FUNCTION4:
			break;
		// Zoom
		case STATE_FCT_ZOOM:
			if (new_state_fct) {
				zoom_view = true;
				if (source_id == GUI_SOURCE1_ID)
					zoom_view_id = GUI_SOURCE1_ID;
				else if (source_id == GUI_OUTPUT1_ID)
					zoom_view_id = GUI_OUTPUT1_ID;
				else if (source_id == GUI_OUTPUT2_ID)
					zoom_view_id = GUI_OUTPUT2_ID;
				else if (source_id == GUI_OUTPUT3_ID)
					zoom_view_id = GUI_OUTPUT3_ID;
			}
			break;
	}
	return true;
}

/*! \brief Navigation State Machine:
 *        - STATE_SOURCE1, STATE_OUTPUT1, STATE_OUTPUT2, OUTPUT3
 *
 */
static void state_machine_task(void)
{
	// Set function state
	if (controller_key_fct5()) {
		state_fct = STATE_FCT_ZOOM;
		new_state_fct = true;
	}
	else if (controller_key_fct1()) {
		state_fct = STATE_FCT_IDLE;
		state = STATE_SOURCE1;
		new_state_fct = true;
	}
	else if (controller_key_fct2()) {
		state_fct = STATE_FCT_IDLE;
		state = STATE_OUTPUT1;
		new_state_fct = true;
	}
	else if (controller_key_fct3()) {
		state_fct = STATE_FCT_IDLE;
		state = STATE_OUTPUT2;
		new_state_fct = true;
	}
	else if (controller_key_fct4()) {
		state_fct = STATE_FCT_IDLE;
		state = STATE_OUTPUT3;
		new_state_fct = true;
	}
	// Clear Zoom state if on and a key is pressed
	if (zoom_view && !controller_key_fct5()) {
		zoom_view = false;
		gui_clear_view();
		new_state_fct = true;
		state_fct = STATE_FCT_IDLE;
	}

	switch (state) {
		case STATE_IDLE:
			break;
		case STATE_SOURCE1:
			if (!state_machine_global(GUI_SOURCE1_ID, &state_fct))
				return;
			break;
		case STATE_OUTPUT1:
			if (!state_machine_global(GUI_OUTPUT1_ID, &state_fct))
				return;
			break;
		case STATE_OUTPUT2:
			if (!state_machine_global(GUI_OUTPUT2_ID, &state_fct))
				return;
			break;
		case STATE_OUTPUT3:
			if (!state_machine_global(GUI_OUTPUT3_ID, &state_fct))
				return;
			break;
	}
	new_state_fct = false;
}

// ADC Configuration
adcifa_opt_t adc_config_t = {
	.frequency                = 1000000,        // ADC frequency (Hz)
	.reference_source         = ADCIFA_ADCREF0, // Reference Source
	.sample_and_hold_disable  = false,    		// Disable Sample and Hold Time
	.single_sequencer_mode    = false,   		// Single Sequencer Mode
	.free_running_mode_enable = false,    		// Free Running Mode
	.sleep_mode_enable        = false,    		// Sleep Mode
	.mux_settle_more_time     = false     		// Multiplexer Settle Time
};

// Sequencer Configuration: same for sequencer 0 and sequencer 1
adcifa_sequencer_opt_t adcifa_sequence_opt = {
	NUMBER_OF_INPUTS_ADC_SEQ0,
	ADCIFA_SRES_12B,
	ADCIFA_TRGSEL_SOFT,
	ADCIFA_SOCB_ALLSEQ,
	ADCIFA_SH_MODE_OVERSAMP,
	ADCIFA_HWLA_NOADJ,
	ADCIFA_SA_NO_EOS_SOFTACK
};

int16_t adc_values_seq0[NUMBER_OF_INPUTS_ADC_SEQ0];
adcifa_sequencer_conversion_opt_t
	adcifa_sequence_conversion_opt_seq0[NUMBER_OF_INPUTS_ADC_SEQ0] = {
		{
			INPUT1_ADC_INP,
			INPUT1_ADC_INN,
			ADCIFA_SHG_1
		}
	};

int16_t adc_values_seq1[NUMBER_OF_INPUTS_ADC_SEQ1];
adcifa_sequencer_conversion_opt_t
	adcifa_sequence_conversion_opt_seq1[NUMBER_OF_INPUTS_ADC_SEQ1] = {
		{
			INPUT2_ADC_INP,
			INPUT2_ADC_INN,
			ADCIFA_SHG_8
		}
	};

volatile avr32_adcifa_t *adcifa = &AVR32_ADCIFA; // ADCIFA IP registers address

/*! \brief ADC Process Init
 *
 *
 */
void adc_process_init(void)
{
	// GPIO pin/adc-function map.
	static const gpio_map_t ADCIFA_GPIO_MAP = {
		{AVR32_ADCREF0_PIN,AVR32_ADCREF0_FUNCTION},
		{AVR32_ADCREFP_PIN,AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN,AVR32_ADCREFN_FUNCTION},
		{INPUT1_ADC_PIN, INPUT1_ADC_FUNCTION},
		{INPUT2_ADC_PIN, INPUT2_ADC_FUNCTION}
	};

	// Assign and enable GPIO pins to the ADC function.
	gpio_enable_module(ADCIFA_GPIO_MAP,
			sizeof(ADCIFA_GPIO_MAP) / sizeof(ADCIFA_GPIO_MAP[0]));

	// Get ADCIFA Factory Configuration
	adcifa_get_calibration_data(adcifa, &adc_config_t);

	// Configure ADCIFA core
	adcifa_configure(adcifa, &adc_config_t, FOSC0);

	// Configure ADCIFA sequencer 0
	adcifa_configure_sequencer(adcifa, 0,
			&adcifa_sequence_opt,
			adcifa_sequence_conversion_opt_seq0);

	// Configure ADCIFA sequencer 1
	adcifa_configure_sequencer(adcifa, 1,
			&adcifa_sequence_opt,
			adcifa_sequence_conversion_opt_seq1);
}

/*! \brief ADC Process Task
 *
 *
 */
void adc_process_task(void)
{

int32_t i;
	// Start ADCIFA sequencer 0
	adcifa_start_sequencer(adcifa, 0);

	// Get Values from sequencer 0
	if (adcifa_get_values_from_sequencer(adcifa,
			0,
			&adcifa_sequence_opt,
			adc_values_seq0) == ADCIFA_STATUS_COMPLETED) {
		for (i=BUFFER_LENGTH-1;i>=1;i--) {
			signal1_buf[i] = signal1_buf[i-1];
		}
		adc_current_conversion = adc_values_seq0[0];
		signal1_buf[0] = ( adc_current_conversion*0x20) - 0x8000;
	}

	// Start ADCIFA sequencer 1
	adcifa_start_sequencer(adcifa, 1);

	// Get Values from sequencer 1
	if (adcifa_get_values_from_sequencer(adcifa,
			1,
			&adcifa_sequence_opt,
			adc_values_seq1) == ADCIFA_STATUS_COMPLETED) {
		for (i=BUFFER_LENGTH-1;i>=1;i--) {
			signal4_buf[i] = signal4_buf[i-1];
		}
		signal4_buf[0] = ( (~adc_values_seq1[0])*0x20) - 0x8000;
	}
}
/*! \brief TWI Initialization for QTouch Controller
 *
 *
 */
static void twi_init(void)
{
	const gpio_map_t AT42QT1060_TWI_GPIO_MAP = {
		{AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
		{AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
	};

	const twi_options_t AT42QT1060_TWI_OPTIONS = {
		.pba_hz = FPBA_HZ,
		.speed = AT42QT1060_TWI_MASTER_SPEED,
		.chip = AT42QT1060_TWI_ADDRESS,
		.smbus        = false,
	};

	// Assign I/Os to TWI.
	gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
	sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
	// Initialize as master.
	twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);
}

/*! \brief Main File Section:
 *          - Initialization (CPU, TWI, Usart,...)
 *          - Main loop with task management (CAN, LIN, ADC)
 */
int main(void)
{
	// Initialize System Clock
	init_sys_clocks();

	// Initialize USART
	init_dbg_rs232(FPBA_HZ);

	// Print Startup Message
	print_dbg("AT32UC3C-EK DEMO\r\n");

	// Initialize ADC for Potentiometer and Microphone
	adc_process_init();

	// Initialize Interrupts
	irq_initialize_vectors();

	// Initialize TWI Interface
	twi_init();

	// GUI, Controller and DSP process init
	gui_init(FCPU_HZ, FHSB_HZ, FPBB_HZ, FPBA_HZ);
	controller_init(FCPU_HZ, FHSB_HZ, FPBA_HZ, FPBB_HZ);

	// Initialize LIN Interface
	lin_task_init();

	// Initialize CAN Interface
	can_task_init();

	cpu_irq_enable();

	// Main loop
	while (true) {
		// Call Lin Task for communication management
		lin_task();
		// Call CAN Task for communication management
		can_task();
		// Call Gui Task for display update
		gui_task();
		// Call Controller Task for control Update
		controller_task();
		// Call ADC Task for sensors update
		adc_process_task();
		// Here add the other concurrent process
		state_machine_task();
	}
}
