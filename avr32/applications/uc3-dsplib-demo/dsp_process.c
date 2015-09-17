/*****************************************************************************
 *
 * \file
 *
 * \brief DSP Process
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#include "dsp_process.h"
#include "dsp.h"
#include "tpa6130.h"
#include "abdac.h"
#include "audio.h"
#include "conf_demo.h"

bool signals_are_updated;
struct signal_source signal1_generator;
struct signal_source signal2_generator;

A_ALIGNED dsp16_t signal1_buf[BUFFER_LENGTH];
A_ALIGNED dsp16_t signal2_buf[BUFFER_LENGTH];

A_ALIGNED dsp16_t signal_pre_filter_buf[FIR_NUM_COEF + BUFFER_LENGTH];
dsp16_t *signal_in_buf;
A_ALIGNED dsp16_t signal_out_buf[BUFFER_LENGTH + 4];

A_ALIGNED dsp16_t signal_in_fft[BUFFER_LENGTH];
A_ALIGNED dsp16_t signal_out_fft[BUFFER_LENGTH];

A_ALIGNED dsp16_t stereo_out_buf1[BUFFER_LENGTH * 2];
A_ALIGNED dsp16_t stereo_out_buf2[BUFFER_LENGTH * 2];
dsp16_t *current_stereo_out_buf;

void dac_overrun_callback(void);
void dac_reload_callback(void);

static A_ALIGNED dsp16_t fft_window[BUFFER_LENGTH];

unsigned int active_filter;

const char * filter_description[NUM_FILTERS] = {
	"No filtering",
	"High-pass filter,\n1000Hz cut-off",
	"Low-pass filter,\n1000Hz cut-off",
};

A_ALIGNED dsp16_t filter_coef[NUM_FILTERS - 1][FIR_NUM_COEF] = {
	{
		DSP16_Q(-0.0184183),
		DSP16_Q(-0.0207874),
		DSP16_Q(-0.0231255),
		DSP16_Q(-0.0254103),
		DSP16_Q(-0.0276199),
		DSP16_Q(-0.0297327),
		DSP16_Q(-0.0317281),
		DSP16_Q(-0.0335863),
		DSP16_Q(-0.0352887),
		DSP16_Q(-0.0368182),
		DSP16_Q(-0.0381593),
		DSP16_Q(-0.0392986),
		DSP16_Q(-0.0402242),
		DSP16_Q(-0.0409269),
		DSP16_Q(-0.0413995),
		DSP16_Q(0.9583631),
		DSP16_Q(-0.0416369),
		DSP16_Q(-0.0413995),
		DSP16_Q(-0.0409269),
		DSP16_Q(-0.0402242),
		DSP16_Q(-0.0392986),
		DSP16_Q(-0.0381593),
		DSP16_Q(-0.0368182),
		DSP16_Q(-0.0352887),
		DSP16_Q(-0.0335863),
		DSP16_Q(-0.0317281),
		DSP16_Q(-0.0297327),
		DSP16_Q(-0.0276199),
		DSP16_Q(-0.0254103),
		DSP16_Q(-0.0231255),
		DSP16_Q(-0.0207874),
		DSP16_Q(-0.0184183),
	}, {
		DSP16_Q(0.0184183),
		DSP16_Q(0.0207874),
		DSP16_Q(0.0231255),
		DSP16_Q(0.0254103),
		DSP16_Q(0.0276199),
		DSP16_Q(0.0297327),
		DSP16_Q(0.0317281),
		DSP16_Q(0.0335863),
		DSP16_Q(0.0352887),
		DSP16_Q(0.0368182),
		DSP16_Q(0.0381593),
		DSP16_Q(0.0392986),
		DSP16_Q(0.0402242),
		DSP16_Q(0.0409269),
		DSP16_Q(0.0413995),
		DSP16_Q(0.0416369),
		DSP16_Q(0.0416369),
		DSP16_Q(0.0413995),
		DSP16_Q(0.0409269),
		DSP16_Q(0.0402242),
		DSP16_Q(0.0392986),
		DSP16_Q(0.0381593),
		DSP16_Q(0.0368182),
		DSP16_Q(0.0352887),
		DSP16_Q(0.0335863),
		DSP16_Q(0.0317281),
		DSP16_Q(0.0297327),
		DSP16_Q(0.0276199),
		DSP16_Q(0.0254103),
		DSP16_Q(0.0231255),
		DSP16_Q(0.0207874),
		DSP16_Q(0.0184183),
	}
};

void signal_source_set_freq(struct signal_source *source,
		unsigned int frequency)
{
	source->frequency = frequency;
}

unsigned int signal_source_get_freq(struct signal_source *source)
{
	return source->frequency;
}

void signal_source_set_volume(struct signal_source *source,
		dsp16_t volume)
{
	if (volume < 0)
		volume = 0;
	source->volume = volume;
}

dsp16_t signal_source_get_volume(struct signal_source *source)
{
	return source->volume;
}

void signal_source_restore_defaults(struct signal_source *source)
{
	source->volume = source->default_volume;
	source->frequency = source->default_freq;
}

void signal_source_init(struct signal_source *source,
		unsigned int frequency, dsp16_t volume)
{
	if (volume < 0)
		volume = 0;

	source->default_volume = volume;
	source->default_freq = frequency;

	signal_source_restore_defaults(source);
}

void generate_signal(dsp16_t *signal, struct signal_source *source)
{
	source->phase = dsp16_gen_sin(signal, BUFFER_LENGTH,
			source->frequency, DAC_SAMPLING_RATE, source->phase);
	dsp16_vect_realmul(signal, signal, BUFFER_LENGTH, source->volume);
}

void filter_set_active(unsigned int num)
{
	if (num >= NUM_FILTERS)
		active_filter = 0;

	active_filter = num;
}

void filter_restore_default()
{
	active_filter = 0;
}

const char * filter_get_description(unsigned int num)
{
	if (num >= NUM_FILTERS)
		return "";

	return filter_description[num];
}

const char * filter_active_get_description()
{
	return filter_get_description(active_filter);
}

void audio_callback(U32 arg)
{
  if( arg == AUDIO_DAC_OUT_OF_SAMPLE_CB )
  {
    dac_overrun_callback();
  }

  else if( arg == AUDIO_DAC_RELOAD_CB )
  {
    dac_reload_callback();
  }

  else if( arg == AUDIO_ADC_OUT_OF_SAMPLE_CB )
  {
  }

  else if( arg == AUDIO_ADC_RELOAD_CB )
  {
  }
}

void dac_reload_callback(void)
{
	int i;

	if (current_stereo_out_buf == stereo_out_buf1)
		current_stereo_out_buf = stereo_out_buf2;
	else
		current_stereo_out_buf = stereo_out_buf1;

	generate_signal(signal1_buf, &signal1_generator);
	generate_signal(signal2_buf, &signal2_generator);

	/* In order for the FIR filter to work correctly we need to keep
	 * the last part of the previous buffer */
	for (i = 0; i < FIR_NUM_COEF; i++) {
		signal_pre_filter_buf[i] =
				signal_pre_filter_buf[BUFFER_LENGTH + i];
	}

	/* New data is put in the signal_in_buf which points behind the data
	 * kept from the previous buffer */
	dsp16_vect_add_and_sat(signal_in_buf, signal1_buf, signal2_buf,
			BUFFER_LENGTH);

	/* Run the FIR filter; the input buffer will be ... longer then the
	 * output buffer, which has normal length */
	if (active_filter > 0) {
		dsp16_filt_fir(signal_out_buf, signal_pre_filter_buf,
				BUFFER_LENGTH + FIR_NUM_COEF - 1,
				filter_coef[active_filter - 1], FIR_NUM_COEF);
	} else {
		for (i = 0; i < BUFFER_LENGTH; i++)
			signal_out_buf[i] = signal_in_buf[i];
	}

	for (i = 0; i < BUFFER_LENGTH; i++) {
		current_stereo_out_buf[i*2] = signal_out_buf[i];
		current_stereo_out_buf[i*2+1] = signal_out_buf[i];
	}

	signals_are_updated = 1;

	/* Update PDCA buffer */
	tpa6130_dac_output(current_stereo_out_buf, BUFFER_LENGTH);
}

void dac_overrun_callback(void)
{
  // Nothing to do
}

void dsp_process_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz)
{
	// Initialize TPA6130
	tpa6130_init();

	// Initialize DAC that send audio to TPA6130
	tpa6130_dac_start(DAC_SAMPLING_RATE, DAC_NUM_CHANNELS,
			DAC_BITS_PER_SAMPLE, DAC_SWAP_CHANNELS,
			audio_callback, AUDIO_DAC_RELOAD_CB,
			FOSC0);

	tpa6130_set_volume(0x20);
	tpa6130_get_volume();

	signal_source_init(&signal1_generator, 433, 20000);
	signal_source_init(&signal2_generator, 2000, 10000);

	current_stereo_out_buf = stereo_out_buf1;
	signal_in_buf = signal_pre_filter_buf + FIR_NUM_COEF;
	filter_restore_default();

	/* Fill the initial buffer for the hamming window with ones. This buffer
	 * will then be multiplied by the hamming window.
	 */
	dsp16_gen_step(fft_window, BUFFER_LENGTH, DSP16_Q(1.), DSP16_Q(1.), 0);
	dsp16_win_hamm(fft_window, fft_window, BUFFER_LENGTH);

	/* Run the interrupt handler manually once to start the ABDAC */
	dac_reload_callback();
}

static void dsp_calculate_fft(dsp16_t *fft, dsp16_t *signal)
{
	int i;
	dsp16_t max_value;
	dsp16_t temp[BUFFER_LENGTH];
	dsp16_complex_t temp_res[BUFFER_LENGTH];

	dsp16_vect_dotmul(temp, signal, fft_window, BUFFER_LENGTH);
	dsp16_trans_realcomplexfft(temp_res, temp, BUFFER_LENGTH_LOG);
	dsp16_vect_complex_abs(fft, temp_res, BUFFER_LENGTH);

	for (i = 0; i < FFT_LENGTH; i++)
		fft[i] += fft[BUFFER_LENGTH - i - 1];

	max_value = dsp16_vect_max(fft, FFT_LENGTH);
	dsp16_vect_realdiv(fft, fft, FFT_LENGTH, max_value + 1);
}

void dsp_process_task(void)
{
	dsp_calculate_fft(signal_in_fft, signal_in_buf);
	dsp_calculate_fft(signal_out_fft, signal_out_buf);
}
