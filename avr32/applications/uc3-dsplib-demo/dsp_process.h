/*****************************************************************************
 *
 * \file
 *
 * \brief DSP Process
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
