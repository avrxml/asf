/**
 * \file
 *
 * \brief Sound sample and playback driver header file.
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

#ifndef _SOUND_H
#define _SOUND_H

#include <tc.h>
#include <conf_example.h>

//! Rate at which DA conversions should be done.
#define SOUND_SAMPLE_RATE 22050UL

/**
 * \brief Start the timer/counter used for sound output.
 *
 * This will start the timer/counter which is used to:
 * - generate events which trigger DAC conversions
 * - trigger interrupt which causes loading of next sample to DAC
 *
 * \ref sound_init should be called before this function.
 */
static inline void sound_start_timer(void)
{
		tc_write_clock_source(&SOUND_TIMER_MODULE, TC_CLKSEL_DIV1_gc);
}

/**
 * \brief Stop the timer/counter used for sound output.
 *
 * This will stop the timer/counter which is used for sound output, effectively
 * causing playback to halt.
 */
static inline void sound_stop_timer(void)
{
		tc_write_clock_source(&SOUND_TIMER_MODULE, TC_CLKSEL_OFF_gc);
}

void sound_init(void);
void sound_play_note(uint8_t button);
void sound_play_sample(uint8_t button);
bool sound_is_playing(void);

#endif // _SOUND_H
