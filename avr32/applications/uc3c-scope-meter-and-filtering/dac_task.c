/**
 * \file
 *
 * \brief DAC Management Task.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#include "compiler.h"
#include "gpio.h"
#include "dacifb.h"
#include "pdca.h"
#include "sinus.h"
#include "pevc.h"
#include "dac_task.h"
#include "scif_uc3c.h"
#include "sysclk.h"
#include "conf_demo.h"

// DACIFB registers address
volatile avr32_dacifb_t *dacifb = &AVR32_DACIFB0;
// PEVC registers address
volatile avr32_pevc_t *ppevc  = &AVR32_PEVC;

extern uint16_t SIGNAL_TABLE[SIGNAL_NB_SAMPLES];

 // DACIFB Configuration
dacifb_opt_t dacifb_opt = {
	.reference          = DACIFB_REFERENCE_VDDANA,     // VDDANA reference
	.channel_selection  = DACIFB_CHANNEL_SELECTION_A,  // No sample & hold
	.low_power          = false,                       // Low power mode
	.dual               = false                        // Dual mode
};

// DACIFB Channel Configuration
dacifb_channel_opt_t dacifb_channel_opt = {
	.auto_refresh_mode = false,                        // No auto refresh
	.trigger_mode      = DACIFB_TRIGGER_MODE_EVENT,    // Trigger selection
	.left_adjustment   = false,                        // Right alignment
	.data_shift        = 0,                            // Number of data shift
	.data_round_enable = false                         // No data rounding
};

// Boolean to trigger end of PDCA transfer
volatile bool flag_dac = false;

void init_pevc(void);
void init_gclk(void);

/*! \brief The PDCA interrupt handler.
 *
 * The handler reload the PDCA settings using the reload register.
 * The interrupt will happen each time the buffer is transmitted to the DAC.
 */
ISR(pdca_int_handler, AVR32_PDCA_IRQ_GROUP1, 1)
{
	// Set PDCA channel reload values with address where data to load are
	//stored, and size of the data block to load.
	pdca_reload_channel(DAC_PDCA_CHANNEL_DAC, (void *)SIGNAL_TABLE,
		SIGNAL_NB_SAMPLES);
	flag_dac = true;
}

void init_pevc(void)
{
	// PEVC Event Shaper options.
	static const pevc_evs_opt_t PEVC_EVS_OPTIONS =
	{
		.igfdr = 0x0A,                // Set the IGF clock (don't care here).
		.igf = PEVC_EVS_IGF_OFF,      // Input Glitch Filter off
		.evf = PEVC_EVS_EVF_OFF,      // Enable Event on falling edge
		.evr = PEVC_EVS_EVR_ON        // Enable Event on rising edge
	};

	// PEVC Init.
	// Configuring the PEVC path: input is the generic clock, each clock cycle,
	// the PEVC trigger a new DAC sample. The sinus samples are sent through
	// the PDCA.
	if(FAIL == pevc_channel_configure(ppevc, AVR32_PEVC_ID_USER_DACIFB0_CHA,
		AVR32_PEVC_ID_GEN_GCLK_0, &PEVC_EVS_OPTIONS)) {
		while(true); // error
	}
	// Enable the PEVC channel 0.
	pevc_channels_enable(ppevc, 1<<AVR32_PEVC_ID_USER_DACIFB0_CHA);
}

void init_gclk(void)
{
	// Setup Sampling Rate
	// Sampling Rate = (FOSC/256) = 62500 HZ
	// Setup the generic clock for EVENT
	scif_gc_setup(AVR32_SCIF_GCLK_GCLK2_EVENT, SCIF_GCCTRL_OSC0,
		AVR32_SCIF_GC_DIV_CLOCK, 256);
	// Now enable the generic clock
	scif_gc_enable(AVR32_SCIF_GCLK_GCLK2_EVENT);
}

/*!
 * \brief DAC Init function :
 *           - Initialize GPIO Alternate function for the PDCA
 *           - Initialize the event controller
 */
void dac_task_init( void )
{
int32_t i;
	// GPIO pin/dac-function map.
	static const gpio_map_t DACIFB_GPIO_MAP = {
		{AVR32_DACREF_PIN,AVR32_DACREF_FUNCTION},
		{AVR32_ADCREFP_PIN,AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN,AVR32_ADCREFN_FUNCTION},
		{DAC_OUTPUT_PIN, DAC_OUTPUT_FUNCTION}
	};

	// Assign and enable GPIO pins to the DAC function.
	gpio_enable_module(DACIFB_GPIO_MAP,
		sizeof(DACIFB_GPIO_MAP) / sizeof(DACIFB_GPIO_MAP[0]));


	// normalize samples to fit the ADC input range
	for (i = 0; i < SIGNAL_NB_SAMPLES; i++)
		SIGNAL_TABLE[i] = (U16)(SIGNAL_TABLE[i] * 0.4);

	// Get DACIFB Factory Configuration
        dacifb_opt.prescaler_clock_hz = sysclk_get_pba_hz()/2;
	dacifb_get_calibration_data(dacifb, &dacifb_opt, DAC_OUTPUT_INSTANCE);

	// configure DACIFB
	dacifb_configure(dacifb, &dacifb_opt, sysclk_get_pba_hz());

	// configure channel DACIFB
	dacifb_configure_channel(dacifb, DAC_OUTPUT_CHANNEL, &dacifb_channel_opt,
			sysclk_get_pba_hz()/2);

	dacifb_start_channel(dacifb, DAC_OUTPUT_CHANNEL, sysclk_get_pba_hz());

	// Initialize Event Controller
	init_pevc();

	// PDCA channel options
	static const pdca_channel_options_t PDCA_OPTIONS = {
		.addr = (void *)SIGNAL_TABLE,                   // memory address
		.pid = DAC_PDCA_PID_DAC_TX,                     // select peripheral
		.size = SIGNAL_NB_SAMPLES,                      // transfer counter
		.r_addr = NULL,                                 // next memory address
		.r_size = 0,                                    // next transfer counter
		.transfer_size = PDCA_TRANSFER_SIZE_HALF_WORD   // Size of the transfer
	};

	// Initialize PDCA channel with the pdca_options
	pdca_init_channel(DAC_PDCA_CHANNEL_DAC, &PDCA_OPTIONS);

	// register Register pdca_int_handler interrupt on level 1
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(pdca_int_handler,
			AVR32_PDCA_IRQ_4, 1);
	cpu_irq_restore(flags);

	// Enable PDCA interrupt each time the reload counter reaches zero
	pdca_enable_interrupt_reload_counter_zero(DAC_PDCA_CHANNEL_DAC);

	// Enable now the transfer
	pdca_enable(DAC_PDCA_CHANNEL_DAC);

	// Initialize the generic clock
	init_gclk();
}

/*! \brief DAC Task:
 */
void dac_task(void)
{
	// nothing to do here, everything is done by hardware
	// thanks to the PEVC and PDCA
}
