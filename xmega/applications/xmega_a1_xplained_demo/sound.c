/**
 * \file
 *
 * \brief Sound sample and sinewave playback driver definitions.
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
#include <board.h>
#include <compiler.h>
#include <dac.h>
#include <string.h>
#include <nvm.h>
#include <tc.h>

#include "sound.h"
#include "snd_samples.h"
// For accessing progmem data with address >64k.
#include "get_far.h"

/**
 * \internal
 * \brief Direct Digital Synthesization residue.
 *
 * This value represents the value that must be added to the DDS accumulators
 * for every DAC conversion if we wanted to convert one period of the signal
 * every second.
 * Usually, one wants to play at a higher rate than 1 Hertz, so this value is
 * multiplied by the desired frequency, before adding to the accumulator.
 */
#define SOUND_DDS_RES (0xFFFFFFFFUL / SOUND_SAMPLE_RATE)

/**
 * \internal
 * \brief Number of concurrent sinewaves that can be output.
 *
 * \note There is no point in making this higher than the number of available
 * buttons or notes.
 */
#define SOUND_SINEWAVE_VOICES 8

//! Table to contain sinewave frequencies -- this is polytonal.
static volatile uint16_t sinefreqs[SOUND_SINEWAVE_VOICES] = {0};
//! 1 or 2 bytes sample width
static uint8_t  snd_samplewidth = 0;
//! Number of frames in a sound
static volatile uint16_t snd_len = 0;
//! Current frame in sound
static volatile uint16_t snd_cnt = 0;

/**
 * \brief Progmem far flash "pointer"
 *
 * Pointer to current sample in flash memory. A uint32_t is used for this
 * because avr-gcc is limited to 16 bit pointers -- this does not cover memory
 * addresses above 2^16.
 */
volatile static flash_addr_t prog_snd_ptr = 0;

//! Table of frequencies for 20 notes, starting at an A (at 441Hz).
uint16_t notes[20] = {
	441, 467, 495, 524, 555, 588, 623, 660, 700, 741, 785,
	832, 882, 934, 990, 1048, 1111, 1177, 1247, 1321,
};

void sample_callback(void);

/**
 * \brief Initialize the DAC, timer/counter and event system
 */
void sound_init(void) {
	struct dac_config conf;

	// Clear the configuration structure.
	memset(&conf, 0, sizeof(struct dac_config));

	/* Create configuration:
	 * - 1V from bandgap as reference, left adjusted channel value
	 * - one active DAC channel, no internal output
	 * - conversions triggered by event channel 0
	 * - 1 us conversion intervals
	 */
	dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_LEFT);
	dac_set_active_channel(&conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_trigger(&conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_interval(&conf, 1);
	dac_write_configuration(&SPEAKER_DAC_MODULE, &conf);
	dac_enable(&SPEAKER_DAC_MODULE);

	// Configure timer/counter to generate events at sample rate.
	tc_enable(&SOUND_TIMER_MODULE);
	tc_write_period(&SOUND_TIMER_MODULE, (sysclk_get_per_hz() /
			SOUND_SAMPLE_RATE) - 1);

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = SOUND_TIMER_EVENT;

	// Enable interrupt for the timer used to update the DAC.
	tc_set_overflow_interrupt_callback(&SOUND_TIMER_MODULE,
			&sample_callback);
	tc_set_overflow_interrupt_level(&SOUND_TIMER_MODULE, PMIC_LVL_LOW);
}

/**
 * \internal
 * \brief Get next sinewave sample by means of DDS (Direct Digital Synth).
 *
 * \param dds_acc Accumulator to be used for calculation and lookup.
 * \param freq The frequency in Hz that is desired.
 */
static inline uint16_t sound_get_next_sinewave_sample(uint32_t *dds_acc,
		uint16_t freq)
{
        flash_addr_t address;

        *dds_acc += SOUND_DDS_RES * freq;
        uint8_t smpl = (*dds_acc >> 24) & 0xFF;
        address = (flash_addr_t)snd_sine + smpl * sizeof(snd_sine[0]);

        return nvm_flash_read_word(address);
}

/**
 * \internal
 * \brief Get the frequency of specified note.
 *
 * \param note Note number, must be in range of \ref notes.
 *
 * \return Frequency, in Hertz, for specified note.
 */
static inline uint16_t sound_get_frequency_from_note(uint8_t note)
{
	Assert(note < sizeof(notes));

	return notes[note];
}

/**
 * \internal
 * \brief Get 16-bit sample from soundtable.
 *
 * \param snd_ptr Pointer to start of soundtable.
 * \param smpl_cnt Sample in table to get.
 *
 * \return 16-bit sample value.
 */
static inline uint16_t sound_get_16_bit_sample(uint32_t snd_ptr,
		uint16_t smpl_cnt)
{
	flash_addr_t address = snd_ptr + smpl_cnt * sizeof(uint16_t);
	uint16_t val = nvm_flash_read_word(address);

	return val;
}

/**
 * \internal
 * \brief Get 8-bit sample from soundtable.
 *
 * \param snd_ptr Pointer to start of soundtable.
 * \param smpl_cnt Sample in table to get.
 *
 * \return 8-bit sample value.
 */
static inline uint8_t sound_get_8_bit_sample(uint32_t snd_ptr,
		uint16_t smpl_cnt)
{
	flash_addr_t address = snd_ptr + smpl_cnt * sizeof(uint8_t);
	uint8_t val = nvm_flash_read_byte(address);

	return val;
}



/**
 * \brief Set sinewave frequencies to generate.
 *
 * Sets the frequencies of notes to generate based on currently pressed buttons
 * (each button corresponds to a unique note).
 *
 * \note This updates the global variable sinefreqs, and will start or stop
 * the sound timer if some or no buttons are pressed, respectively.
 *
 * \param buttons Mask of buttons which are pressed.
 */
void sound_play_note(uint8_t buttons)
{
	if (buttons) {
		sound_start_timer();
	} else {
		sound_stop_timer();
	}

#if SOUND_SINEWAVE_VOICES > 0
	sinefreqs[0] = (buttons & PIN0_bm) ?
		sound_get_frequency_from_note(0) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 1
	sinefreqs[1] = (buttons & PIN1_bm) ?
		sound_get_frequency_from_note(1) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 2
	sinefreqs[2] = (buttons & PIN2_bm) ?
		sound_get_frequency_from_note(2) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 3
	sinefreqs[3] = (buttons & PIN3_bm) ?
		sound_get_frequency_from_note(3) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 4
	sinefreqs[4] = (buttons & PIN4_bm) ?
		sound_get_frequency_from_note(4) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 5
	sinefreqs[5] = (buttons & PIN5_bm) ?
		sound_get_frequency_from_note(5) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 6
	sinefreqs[6] = (buttons & PIN6_bm) ?
		sound_get_frequency_from_note(6) : 0;
#endif
#if SOUND_SINEWAVE_VOICES > 7
	sinefreqs[7] = (buttons & PIN7_bm) ?
		sound_get_frequency_from_note(7) : 0;
#endif
}

/**
 * \brief Starts playing samples when button status is sent.
 *
 * Loads information about the sound to play starts the sound timer which
 * triggers conversions and updates of sample value in DAC, at a rate of
 * SOUND_SAMPLE_RATE times per second.
 *
 * \note This modifies the global variables snd_cnt, snd_len snd_samplewidth,
 * and starts the sound timer.
 *
 * See get_far.h for usage of GET_FAR_ADDRESS(symbol).
 *
 * \param buttons Mask of buttons which are pressed.
 */
void sound_play_sample(uint8_t buttons)
{
	if (sound_is_playing() || !buttons) {
		return;
	}

	if ((buttons & PIN0_bm) && (buttons & PIN1_bm) && (buttons & PIN7_bm)) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_youwin);
		snd_len = sizeof(snd_youwin) / sizeof(snd_youwin[0]);
		snd_samplewidth = 1;
	} else if ((buttons & PIN0_bm) && (buttons & PIN2_bm) && (buttons & PIN7_bm)) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_fanfare);
		snd_len = sizeof(snd_fanfare) / sizeof(snd_fanfare[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN0_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_drum);
		snd_len = sizeof(snd_drum) / sizeof(snd_drum[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN1_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_sym);
		snd_len = sizeof(snd_sym) / sizeof(snd_sym[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN2_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_drumroll);
		snd_len = sizeof(snd_drumroll) / sizeof(snd_drumroll[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN3_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_drum);
		snd_len = (sizeof(snd_drum) / sizeof(snd_drum[0])) + (sizeof(snd_sym) / sizeof(snd_sym[0]));
		snd_samplewidth = 1;
	} else if (buttons & PIN4_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_sax);
		snd_len = sizeof(snd_sax) / sizeof(snd_sax[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN5_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_synth1);
		snd_len = sizeof(snd_synth1) / sizeof(snd_synth1[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN6_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_synth2);
		snd_len = sizeof(snd_synth2) / sizeof(snd_synth2[0]);
		snd_samplewidth = 1;
	} else if (buttons & PIN7_bm) {
		prog_snd_ptr = GET_FAR_ADDRESS(snd_synth3);
		snd_len = sizeof(snd_synth3) / sizeof(snd_synth3[0]);
		snd_samplewidth = 1;
	}

	snd_cnt = 0;
	sound_start_timer();
}

/**
 * \brief Convenience function to check if a sound is currently playing.
 *
 * \retval true Sound is currently being played back.
 * \retval false Sound is not being played back.
 */
bool sound_is_playing(void) {
	return snd_len || snd_cnt;
}

/**
 * \brief Callback function for updating DAC sample value.
 *
 * This handles loading of the next sample into the DAC channel data register.
 * Depending on the global variables associated with the currently playing
 * sounds, this will load either a sound sample or the next value for the
 * sinewaves.
 *
 * The actual DAC conversion is initiated by the overflow event, not this ISR.
 *
 * The volume is controlled by bit-shifting the sample values. However, if the
 * values are shifted too much, the signal will be severely distorted since the
 * MSBs are lost first.
 */
void sample_callback(void)
{
	// Accumulators used for Direct Digital Synth sinewave.
	static uint32_t acc[8] = {0};
	uint16_t        value = 0;

	// Temporarily copy volatile variables to define order of access.
	uint16_t _snd_len = snd_len;
	uint16_t _snd_cnt = snd_cnt;

	// Don't generate sine if sample is playing.
	if (!_snd_len && !_snd_cnt) {
		uint8_t i;
		for (i = 0; i < SOUND_SINEWAVE_VOICES; ++i) {
			if (sinefreqs[i]) {
				value += sound_get_next_sinewave_sample(&acc[i],
						sinefreqs[i]) << 1;
			}
		}
	// Get next sample, if we're not at the end yet.
	} else if (_snd_len != _snd_cnt) {
		if (snd_samplewidth == 1) {
			value += sound_get_8_bit_sample(prog_snd_ptr, _snd_cnt)
				<< 5;
		} else {
			value += sound_get_16_bit_sample(prog_snd_ptr, _snd_cnt)
				<< 2;
		}
		snd_cnt = _snd_cnt + 1;
	// We're done playing the sample, so reset and stop playback.
	} else {
		snd_cnt = 0;
		snd_len = 0;
		sound_stop_timer();
	}

	dac_set_channel_value(&SPEAKER_DAC_MODULE, SPEAKER_DAC_CHANNEL, value);
}
