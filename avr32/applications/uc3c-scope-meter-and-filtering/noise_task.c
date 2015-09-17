/**
 * \file
 *
 * \brief Noise Task Control.
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
#include <stddef.h>
#include <stdio.h>
#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "noise_task.h"
#include "dsp.h"
#include "conf_demo.h"

// External Reference to Sine In buffer in 16-bit fixed point Format
// scaled in GUI format the GUI task
extern dsp16_t signalin_gui[GUI_BUFFER_LENGTH];
// External Reference to Sine In buffer in Floating Point Format scaled
// for the remote task
extern volatile float signalin_remote[GUI_BUFFER_LENGTH];
// External Reference to Potentiometer
extern int16_t potentiometer_value;

// Reference to Noise Signal Buff 16-bit fixed point Format
// scaled in GUI format for the GUI task
A_ALIGNED dsp16_t signal_noise_gui[GUI_BUFFER_LENGTH];
// Reference to Noise Signal Buff in floating point Format
// for the remote task
volatile float signal_noise_remote[GUI_BUFFER_LENGTH];
// Reference to Signal In + Noise Signal Buff 16-bit fixed point Format
// scaled in GUI format the GUI task
A_ALIGNED dsp16_t signalin_noise_gui[GUI_BUFFER_LENGTH];
// Reference to Signal In + Noise Signal Buff in floating point Format
// for the remote task
volatile float signalin_noise_remote[GUI_BUFFER_LENGTH];

/*!
 * \brief Init of Noise Task
 */
void noise_task_init( void )
{
}

/*! \brief Noise Task:
 *        - Add random noise to buffer input signal
 */
void noise_task(void)
{
	static uint32_t noise_value = NOISE_STARTUP_VALUE;
	int32_t i;
	if (gpio_get_pin_value(GPIO_PUSH_BUTTON_0) == GPIO_PUSH_BUTTON_0_PRESSED) {
		if (noise_value < NOISE_MAX_VALUE - NOISE_STEP)
			noise_value = noise_value + NOISE_STEP;
	}
	if (gpio_get_pin_value(GPIO_PUSH_BUTTON_1) == GPIO_PUSH_BUTTON_1_PRESSED) {
		if (noise_value > NOISE_MIN_VALUE + NOISE_STEP)
			noise_value = noise_value - NOISE_STEP;
	}
	for (i=0;i<GUI_BUFFER_LENGTH;i++) {
		// Generate a Random noise reading the potentiometer Value
		signal_noise_remote[i] = (rand()%(noise_value));
		// Rescale the value on a 16-bits fixed point format
		signal_noise_gui[i] =
			GUI_SCALE_GAIN_VALUE*((int32_t)signal_noise_remote[i]) -
			GUI_SCALE_OFFSET_VALUE;
		// Add noise to the sine wave signal
		signalin_noise_remote[i] = signalin_remote[i] +
			signal_noise_remote[i];
		// Rescale the value on a 16-bits fixed point format
		signalin_noise_gui[i] = GUI_SCALE_GAIN_VALUE *
			((int32_t)signalin_noise_remote[i]) - GUI_SCALE_OFFSET_VALUE;
	}
}
