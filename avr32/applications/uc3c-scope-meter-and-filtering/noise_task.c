/**
 * \file
 *
 * \brief Noise Task Control.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
