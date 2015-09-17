/**
 * \file
 *
 * \brief Filtering Management Task.
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
#include "filter_task.h"
#include "dsp.h"
#include "conf_demo.h"

//! Reference to Signal In + Noise Signal Buff 16-bit fixed point Format
//! scaled in GUI format for the GUI task
extern dsp16_t signalin_noise_gui[GUI_BUFFER_LENGTH];
//! Reference to Signal In + Noise Signal Buff in floating point Format
//! scaled for the remote task
extern volatile float signalin_noise_remote[GUI_BUFFER_LENGTH];

//! Reference to Filtered Signal through 16-bit fixed point Format
//! scaled in GUI format for the GUI task
A_ALIGNED dsp16_t signalout_fp_gui[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through 16-bit fixed point Format
//! scaled for the remote task
volatile float signalout_fp_remote[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through floating point Format
//! scaled in GUI format for the GUI task
A_ALIGNED dsp16_t signalout_fpu_gui[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through floating point Format
//! scaled for the remote task.
volatile float signalout_fpu_remote[GUI_BUFFER_LENGTH];

#define FIR_COEF_SIZE  25
// The impulse response coefficients of the filter
A_ALIGNED dsp32_t fir_coef32b[FIR_COEF_SIZE] = {
	DSP32_Q(0.02213774312),
	DSP32_Q(0.02660861642),
	DSP32_Q(0.03103291928),
	DSP32_Q(0.03533215215),
	DSP32_Q(0.03942863824),
	DSP32_Q(0.04324724164),
	DSP32_Q(0.04671704392),
	DSP32_Q(0.04977293804),
	DSP32_Q(0.05235710024),
	DSP32_Q(0.05442030354),
	DSP32_Q(0.05592304013),
	DSP32_Q(0.05683642486),
	DSP32_Q(0.05714285714),
	DSP32_Q(0.05683642486),
	DSP32_Q(0.05592304013),
	DSP32_Q(0.05442030354),
	DSP32_Q(0.05235710024),
	DSP32_Q(0.04977293804),
	DSP32_Q(0.04671704392),
	DSP32_Q(0.04324724164),
	DSP32_Q(0.03942863824),
	DSP32_Q(0.03533215215),
	DSP32_Q(0.03103291928),
	DSP32_Q(0.02660861642),
	DSP32_Q(0.02213774312)
};

float fir_coef_fl[FIR_COEF_SIZE] = {
	(0.02213774312f),
	(0.02660861642f),
	(0.03103291928f),
	(0.03533215215f),
	(0.03942863824f),
	(0.04324724164f),
	(0.04671704392f),
	(0.04977293804f),
	(0.05235710024f),
	(0.05442030354f),
	(0.05592304013f),
	(0.05683642486f),
	(0.05714285714f),
	(0.05683642486f),
	(0.05592304013f),
	(0.05442030354f),
	(0.05235710024f),
	(0.04977293804f),
	(0.04671704392f),
	(0.04324724164f),
	(0.03942863824f),
	(0.03533215215f),
	(0.03103291928f),
	(0.02660861642f),
	(0.02213774312f)
};
//! Local function of filtering through floating point implementation of FIR
//! filter
static void filter_task_fir(float *vect2,float *vect1, int b_length, float *h,
	int h_size);
//! Local function of filtering through fixed point implementation of FIR
//! filter
static void filter_task_fir_fp(dsp32_t *vect2,dsp32_t *vect1, int b_length,
	dsp32_t *h, int h_size);
/*! \brief Floating Point Implementation of FIR Filter
*
*/
float w[FIR_COEF_SIZE];
static void filter_task_fir(float *vect2,float *vect1, int b_length, float *h,
	int h_size)
{
	int32_t i,j;
	float y;

	for (j=0;j<b_length;j++) {
		w[0] = vect1[j];
		y = 0.0f;
		for (i=0;i<h_size;i++) {
			y += h[i]*w[i];
		}
		vect2[j] = y;
		for (i=h_size; i>=1; i--)
			w[i] = w[i-1];
	}
	for (i=0; i<b_length-h_size; i++)
		vect2[i] = vect2[i+h_size-1];
}

/*! \brief Fixed Point Implementation of FIR Filter in 32-bits format
*
*/
dsp32_t w_fp[FIR_COEF_SIZE];
static void filter_task_fir_fp(dsp32_t *vect2,dsp32_t *vect1, int b_length,
	dsp32_t *h, int h_size)
{
	int32_t i,j;
	int64_t y;

	for (j=0;j<b_length;j++) {
		w_fp[0] = vect1[j];
		y = 0;
		for (i=0;i<h_size;i++) {
			y += (S64)h[i]*(S64)w_fp[i];
		}
		vect2[j] = y>>31;
		for (i=h_size; i>=1; i--)
			w_fp[i] = w_fp[i-1];
	}
	for (i=0; i<b_length-h_size; i++)
		vect2[i] = vect2[i+h_size-1];
}

/*! \brief Filter Task Init Function
*
*/
void filter_task_init(void) {
// Nothing to do ...
}

/*! \brief Filter Task Function:
*       - Filter signal using Fixed Point Implementation
*       - Filter signal using Floating Point Implementation
*       - Rr
*
*/
void filter_task(void)
{
	int32_t i;
	A_ALIGNED dsp32_t fpout_buff[GUI_BUFFER_LENGTH];
	A_ALIGNED dsp32_t fpin_buff[GUI_BUFFER_LENGTH];

	for (i=0;i<GUI_BUFFER_LENGTH;i++) {
		fpin_buff[i] = (int32_t)signalin_noise_remote[i];
	}

	// Filter the signal using 16-bit format Implementation
	filter_task_fir_fp(fpout_buff, fpin_buff,
		GUI_BUFFER_LENGTH, fir_coef32b, FIR_COEF_SIZE);
	// Scale the buffer
	for (i=0;i<GUI_BUFFER_LENGTH;i++) {
		// ... for the remote task,
		signalout_fp_remote[i] = (float)(fpout_buff[i]);
		// ... and the GUI task
		signalout_fp_gui[i] = GUI_SCALE_GAIN_VALUE *
			(S32)(signalout_fp_remote[i]) -
			GUI_SCALE_OFFSET_VALUE;
	}

	// Filter the signal using floating format Implementation
	filter_task_fir((float *)signalout_fpu_remote,
		(float *)signalin_noise_remote, GUI_BUFFER_LENGTH, fir_coef_fl,
		FIR_COEF_SIZE);
	// Scale the buffer
	for (i=0;i<GUI_BUFFER_LENGTH;i++) {
		// .. for the GUI task
		signalout_fpu_gui[i] = GUI_SCALE_GAIN_VALUE *
			(S32)(signalout_fpu_remote[i]) -
			GUI_SCALE_OFFSET_VALUE;
	}
}
