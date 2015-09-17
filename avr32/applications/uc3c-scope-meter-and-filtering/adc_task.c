/**
 * \file
 *
 * \brief ADC Management Task.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stddef.h>
#include <stdio.h>
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "adcifa.h"
#include "pdca.h"
#include "adc_task.h"
#include "sysclk.h"
#include "dsp.h"
#include "conf_demo.h"

// Reference to Sine In Buff 16-bit fixed point Format
// scaled in GUI format the GUI task
A_ALIGNED dsp16_t signalin_gui[GUI_BUFFER_LENGTH];
// Reference to Sine In Buff floating point Format for the remote task
volatile float signalin_remote[GUI_BUFFER_LENGTH];
volatile int16_t potentiometer_value;

// Trigger process state
#define ADC_TRIGGER_SYNC_NONE             0
#define ADC_TRIGGER_SYNC_MIN              1
#define ADC_TRIGGER_SYNC_ORDER            2

// Local buffer for the elements of PDCA Channel of SEQ0
int16_t adc_values_seq0[ADC_PDCA_CHANNEL_NUMBER_OF_SAMPLES];
// Local buffer for the elements of PDCA Channel of SEQ1
int16_t adc_values_seq1[ADC_NUMBER_OF_CONV_SEQ1];

// ADCIFA IP registers address
volatile avr32_adcifa_t *adcifa = &AVR32_ADCIFA;

// Boolean to trigger end of PDCA transfer
volatile bool flag_end_of_acquisition = false;

// Boolean to start acquisition trigger
volatile int32_t trigger_process_state = ADC_TRIGGER_SYNC_NONE;

// ADC Configuration
adcifa_opt_t adc_config_t = {
	.frequency                  = 1000000,            // ADC frequency (Hz)
	.reference_source           = ADCIFA_REF06VDD,    // Reference source
	.sample_and_hold_disable    = false,              // Sample and Hold enabled
	.single_sequencer_mode      = false,              // Dual sequencer mode
	.free_running_mode_enable   = false,              // Free running not used
	.sleep_mode_enable          = false,              // Sleep mode not used
	.mux_settle_more_time       = false     		  // Multiplexer Settle Time
};

// Sequencer 0 Configuration
adcifa_sequencer_opt_t adc_sequencer0_config_t = {
	.convnb                     = ADC_NUMBER_OF_CONV_SEQ0,  // nb channel conv.
	.resolution                 = ADCIFA_SRES_10B,          // 10-bit Resolution
	.trigger_selection          = ADCIFA_TRGSEL_ITIMER,     // Timer Trigger
	.start_of_conversion        = ADCIFA_SOCB_ALLSEQ,       // All Conv. in seq
	.sh_mode                    = ADCIFA_SH_MODE_STANDARD,  // No Oversampling
	.half_word_adjustment       = ADCIFA_HWLA_NOADJ,        // Right Alignment
	.software_acknowledge       = ADCIFA_SA_NO_EOS_SOFTACK  // No Soft. Ack.
};

// Sequencer 1 Configuration
adcifa_sequencer_opt_t adc_sequencer1_config_t = {
	.convnb                     = ADC_NUMBER_OF_CONV_SEQ1,  // nb channel conv.
	.resolution                 = ADCIFA_SRES_10B,          // 10-bit Resolution
	.trigger_selection          = ADCIFA_TRGSEL_SOFT,       // Manual Trigger
	.start_of_conversion        = ADCIFA_SOCB_ALLSEQ,       // All Conv. in Seq
	.sh_mode                    = ADCIFA_SH_MODE_STANDARD,  // No Oversampling
	.half_word_adjustment       = ADCIFA_HWLA_NOADJ,        // Right Alignment
	.software_acknowledge       = ADCIFA_SA_NO_EOS_SOFTACK  // No Soft. Ack.
};

// Conversions in the sequencer configuration
// sequencer 0 conversion channels
adcifa_sequencer_conversion_opt_t
	adc_conversion_sequencer0_t[ADC_NUMBER_OF_CONV_SEQ0] = {
	{ // 1st conversion
		.channel_p = ADC_EXT_INPUT_INP,             // Positive Channel
		.channel_n = ADC_EXT_INPUT_INN,             // Negative Channel
		.gain      = ADCIFA_SHG_1                   // No Gain (x1)
	}
};

// sequencer 1 conversion channels
adcifa_sequencer_conversion_opt_t
	adc_conversion_sequencer1_t[ADC_NUMBER_OF_CONV_SEQ1] = {
	{ // 1st conversion
		.channel_p = ADC_POTENTIOMETER_INP,         // Positive Channel
		.channel_n = ADC_POTENTIOMETER_INN,         // Negative Channel
		.gain      = ADCIFA_SHG_1                   // No Gain (x1)
	},
	{ // 2nd conversion
		.channel_p = ADC_POTENTIOMETER_INP,         // Positive Channel
		.channel_n = ADC_POTENTIOMETER_INN,         // Negative Channel
		.gain      = ADCIFA_SHG_1                   // No Gain (x1)
	},
	{ // 3rd conversion
		.channel_p = ADC_POTENTIOMETER_INP,         // Positive Channel
		.channel_n = ADC_POTENTIOMETER_INN,         // Negative Channel
		.gain      = ADCIFA_SHG_1                   // No Gain (x1)
	},
	{ // 4th conversion
		.channel_p = ADC_POTENTIOMETER_INP,         // Positive Channel
		.channel_n = ADC_POTENTIOMETER_INN,         // Negative Channel
		.gain      = ADCIFA_SHG_1                   // No Gain (x1)
	}
};

// Initialize the PDCA transfer channel structure
static const pdca_channel_options_t pdca_channel0_config_t = {
	.addr = (void *)adc_values_seq0,                // Buffer address
	.pid = AVR32_ADCIFA_PDCA_ID_RX_CH0,             // Data from ADC SEQ0
	.size = ADC_PDCA_CHANNEL_NUMBER_OF_SAMPLES,     // Transfer counter
	.r_addr = NULL,                                 // No reload address
	.r_size = 0,                                    // No reload counter
	.transfer_size = PDCA_TRANSFER_SIZE_HALF_WORD   // 16-bit transfer
};

// Initialize the ADCIFA Window Monitor Structure
volatile adcifa_window_monitor_opt_t adc_window_config_t = {
	.mode = ADCIFA_WINDOW_MODE_ABOVE,               // Default init mode
	.low_threshold = 0,                             // Default LT
	.high_threshold = 0,                            // Default HT
	.source_index = ADC_NUMBER_OF_CONV_SEQ0-1       // Index
};

void pdca_init(void);

/*! \brief The ADCIFA window monitor interrupt handler.
*
*/
#if !defined(AVR32_ADCIFA_IRQ_GROUP)
#define AVR32_ADCIFA_IRQ_GROUP          36
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif
ISR(adc_int_window_handler, AVR32_ADCIFA_IRQ_GROUP, 0)
{
	// Here only sequencer 0 is enabled so the interrupt source is not checked
	switch (trigger_process_state) {
		case ADC_TRIGGER_SYNC_MIN: // Min level found
			if (potentiometer_value > adc_window_config_t.high_threshold) {
				// Start order synchronization step only if
				// trig > min signal value
				trigger_process_state = ADC_TRIGGER_SYNC_ORDER;
				adc_window_config_t.low_threshold = potentiometer_value;
				// Trig when sample > LT
				adc_window_config_t.mode = ADCIFA_WINDOW_MODE_ABOVE;
				adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
					(adcifa_window_monitor_opt_t *)&adc_window_config_t);
			}
			else {
				// Restart min level search
				adc_window_config_t.high_threshold = 0; // Stay in sync process
				adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
					(adcifa_window_monitor_opt_t *)&adc_window_config_t);
			}
		break;
		case ADC_TRIGGER_SYNC_ORDER:        // Order level found
			// Launch acquisition now
			pdca_enable(ADC_PDCA_CHANNEL_SEQ0);
			gpio_clr_gpio_pin(LED0_GPIO); // Let's switch on LED0 when sampling
			adcifa_disable_interrupt(adcifa, AVR32_ADCIFA_WM0_MASK);
		break;
		default:
		break;
	}
	// Clear any pending interrupt
	adcifa_clear_interrupt(adcifa, AVR32_ADCIFA_WM0_MASK);
}

/*! \brief The PDCA interrupt handler.
 *
 * The handler disables the PDCA transfer.
 * The interrupt will happen each time the transfer is completed.
 */
ISR(pdca_int_handler_seq0, AVR32_PDCA_IRQ_GROUP0, 0)
{
	gpio_set_gpio_pin(LED0_GPIO); // switch off LED0
	// Re-initialize the PDCA channel for next acquisition
	pdca_disable(ADC_PDCA_CHANNEL_SEQ0);
	pdca_init_channel(ADC_PDCA_CHANNEL_SEQ0, &pdca_channel0_config_t);
	pdca_enable_interrupt_transfer_complete(ADC_PDCA_CHANNEL_SEQ0);
	flag_end_of_acquisition = true;
}

/*!
 * \brief Configure the PDCA and register its interrupt pdca_int_handler
 */
void pdca_init(void)
{
	// Initialize PDCA channel with the pdca_options
	pdca_init_channel(ADC_PDCA_CHANNEL_SEQ0, &pdca_channel0_config_t);

	// register Register pdca_int_handler_seq0 interrupt on level 0
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(pdca_int_handler_seq0,
					AVR32_PDCA_IRQ_0, 0);
	cpu_irq_restore(flags);

	// Enable the PDCA interrupt each time the transfer is complete,
	// i.e. each time a data buffer is ready
	pdca_enable_interrupt_transfer_complete(ADC_PDCA_CHANNEL_SEQ0);
}


/*!
 * \brief main function : initialization and loop to display ADC values
 */
void adc_task_init( void )
{
	// GPIO pin/adc-function map.
	static const gpio_map_t adc_io_map_t = {
		{AVR32_ADCREF0_PIN,AVR32_ADCREF0_FUNCTION},
		{AVR32_ADCREFP_PIN,AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN,AVR32_ADCREFN_FUNCTION},
		{ADC_EXT_INPUT_PIN, ADC_EXT_INPUT_FUNCTION},
		{ADC_POTENTIOMETER_PIN, ADC_POTENTIOMETER_FUNCTION}
	};

	AVR32_HMATRIXB.mcfg[AVR32_HMATRIXB_MASTER_CPU_INSN] = 0x1;

	// Assign and enable GPIO pins to the ADC function.
	gpio_enable_module(adc_io_map_t,
		sizeof(adc_io_map_t) / sizeof(adc_io_map_t[0]));

	// Get ADCIFA factory Configuration
	adcifa_get_calibration_data(adcifa, &adc_config_t);

	// Configure ADCIFA core
	adcifa_configure(adcifa, &adc_config_t, sysclk_get_pbc_hz());

	// Configure ADCIFA sequencer 0 & 1
	adcifa_configure_sequencer(adcifa, ADCIFA_SEQ0, &adc_sequencer0_config_t,
			adc_conversion_sequencer0_t);

	adcifa_configure_sequencer(adcifa, ADCIFA_SEQ1, &adc_sequencer1_config_t,
			adc_conversion_sequencer1_t);

	// Configure the trigger monitor
	trigger_process_state = ADC_TRIGGER_SYNC_MIN;           // Start sync search
	adc_window_config_t.mode = ADCIFA_WINDOW_MODE_BELOW;// Trig when sample < HT
	adc_window_config_t.high_threshold = 0;             // Sweep threshold
	adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
			(adcifa_window_monitor_opt_t *)&adc_window_config_t);

	// register Register adc_int_window_handler interrupt on level 0
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(adc_int_window_handler,
			AVR32_ADCIFA_WINDOW_IRQ, 0);
	cpu_irq_restore(flags);

	// Clear any pending interrupt
	adcifa_clear_interrupt(adcifa, AVR32_ADCIFA_WM0_MASK);
	adcifa_enable_interrupt(adcifa, AVR32_ADCIFA_WM0_MASK);

	// Start the internal timer fs = FADC / 16
	adcifa_start_itimer(adcifa, 20);

	// Initialize the PDCA
	pdca_init();

	// Start a first acquisition now
	pdca_enable(ADC_PDCA_CHANNEL_SEQ0);
}

/*! \brief ADC Task:
 *  - Check if DMA Transfer is completed
 *  - Update Buffer Signal
 *  - Manage the trigger search feature
 */
void adc_task(void)
{
	int32_t i, j, temp;

	if (flag_end_of_acquisition == true) {
		// End of buffer acquisition
		flag_end_of_acquisition = false;
		// Normalize samples for display
		for (i = 0; i < GUI_BUFFER_LENGTH; i++) {
			temp = 0;
			for (j = 0; j < ADC_NUMBER_OF_CONV_SEQ0; j++) {
				// Averaging sample data
				temp += adc_values_seq0[i*ADC_NUMBER_OF_CONV_SEQ0+j];
			}
			signalin_gui[i] = (ADC_SCALE_GAIN * temp/ADC_NUMBER_OF_CONV_SEQ0)
				- ADC_SCALE_OFFSET;
			signalin_remote[i] = temp/ADC_NUMBER_OF_CONV_SEQ0;
		}
		// Restart trigger synchronization
		trigger_process_state = ADC_TRIGGER_SYNC_MIN; // Start min signal search
		// Trig when sample < HT
		adc_window_config_t.mode = ADCIFA_WINDOW_MODE_BELOW;
		adc_window_config_t.high_threshold = 0;
		adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
				(adcifa_window_monitor_opt_t *)&adc_window_config_t);
		adcifa_enable_interrupt(adcifa, AVR32_ADCIFA_WM0_MASK);
	}

	if (trigger_process_state == ADC_TRIGGER_SYNC_MIN) {
		// Increase the trigger threshold to find min value
		adc_window_config_t.high_threshold += 50;
		adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
				(adcifa_window_monitor_opt_t *)&adc_window_config_t);
	}
	else {
		// Update the potentiometer value
		adc_window_config_t.low_threshold = potentiometer_value;
		adcifa_configure_window_monitor(adcifa, ADCIFA_SEQ0,
				(adcifa_window_monitor_opt_t *)&adc_window_config_t);
	}

	// Start potentiometer position sampling: sequencer 1
	adcifa_start_sequencer(adcifa, ADCIFA_SEQ1);

	// Normalize potentiometer position value
	if (adcifa_get_values_from_sequencer(adcifa, ADCIFA_SEQ1,
				&adc_sequencer1_config_t, adc_values_seq1) ==
				ADCIFA_STATUS_COMPLETED) {
		// Process the data
		temp = 0;
		for (i = 0; i < ADC_NUMBER_OF_CONV_SEQ1; i++) {
			// Averaging sample data
			temp += adc_values_seq1[i];
		}
		temp /= ADC_NUMBER_OF_CONV_SEQ1;
		if (temp < 0) temp = 0;
			potentiometer_value = temp;
	}
}
