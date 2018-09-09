/**
 * \file
 *
 * \brief Sound sample and playback driver header file.
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
