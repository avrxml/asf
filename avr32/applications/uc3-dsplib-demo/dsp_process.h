/*****************************************************************************
 *
 * \file
 *
 * \brief DSP Process
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _DSP_PROCESS_H_
#define _DSP_PROCESS_H_

#include "dsp.h"

#define NUM_FILTERS	3

struct signal_source {
	unsigned int frequency;
	dsp16_t volume;
	dsp16_t phase;
	unsigned int default_freq;
	dsp16_t default_volume;
};

extern struct signal_source signal1_generator;
extern struct signal_source signal2_generator;

void output_set_volume(dsp16_t volume);

void output_set_default_vol();

dsp16_t output_get_volume();

void signal_source_init(struct signal_source *source,
		unsigned int frequency, dsp16_t volume);

void signal_source_set_freq(struct signal_source *source,
		unsigned int frequency);

unsigned int signal_source_get_freq(struct signal_source *source);

void signal_source_restore_defaults(struct signal_source *source);

void signal_source_set_volume(struct signal_source *source,
		dsp16_t volume);

dsp16_t signal_source_get_volume(struct signal_source *source);

void filter_set_active(unsigned int num);
void filter_restore_default();
const char * filter_get_description(unsigned int num);
const char * filter_active_get_description();

void dsp_process_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz);
void dsp_process_task(void);

#endif //_DSP_PROCESS_H_
