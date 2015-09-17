/**************************************************************************
 *
 * \file
 *
 * \brief Bang & Olufsen MS3 driver file.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "ms3.h"
#include "ssc_i2s.h"
#include "pdca.h"
#include "board.h"
#include "audio.h"
#include "intc.h"
#include "gpio.h"
#include "twi.h"
#include "cs2200.h"

#define MS3_SSC_TX_PDCA_CHANNEL                 (0)
#define MS3_SSC_TX_FRAME_SYNC_PIN               (AVR32_SSC_TX_FRAME_SYNC_0_PIN)

/**
 * \def CS2200_FREF
 * \brief Frequency (in Hz) of the reference clock connected to the CS2200.
 * \note By default this value is set to 12 MHz.
 */
#ifndef CS2200_FREF
#	define CS2200_FREF 12000000
#endif

//! CS2200 out frequency
extern uint32_t usb_stream_resync_frequency;

//! Output parameters.
static struct {
	U8 num_channels;
	void (*callback)(U32 arg);
	U32 callback_opt;
} ms3_output_params = {
	.num_channels = 0,
	.callback     = NULL,
	.callback_opt = 0
};

static const gpio_map_t MS3_GPIO_MAP = {
	{AVR32_SSC_TX_CLOCK_0_PIN, AVR32_SSC_TX_CLOCK_0_FUNCTION},
	{AVR32_SSC_TX_DATA_0_PIN, AVR32_SSC_TX_DATA_0_FUNCTION},
	{AVR32_SSC_TX_FRAME_SYNC_0_PIN, AVR32_SSC_TX_FRAME_SYNC_0_FUNCTION},
	{AVR32_PM_GCLK_2_0_PIN, AVR32_PM_GCLK_2_0_FUNCTION}
};

#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void ms3_ssc_tx_pdca_int_handler(void);

#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void ms3_ssc_tx_pdca_int_handler(void)
{
	if (pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE)
	{
		pdca_disable_interrupt_transfer_complete(MS3_SSC_TX_PDCA_CHANNEL);
		if (ms3_output_params.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
			ms3_output_params.callback(AUDIO_DAC_OUT_OF_SAMPLE_CB);
	}

	if (pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO)
	{
		pdca_disable_interrupt_reload_counter_zero(MS3_SSC_TX_PDCA_CHANNEL);
		if (ms3_output_params.callback_opt & AUDIO_DAC_RELOAD_CB)
			ms3_output_params.callback(AUDIO_DAC_RELOAD_CB);
	}
}

void ms3_dac_start(U32 sample_rate_hz, U8 num_channels, U8 bits_per_sample,
		bool swap_channels, void (*callback)(U32 arg), U32 callback_opt,
		U32 pba_hz)
{
	ms3_dac_stop();

	// Enable 4.2V for MS3.
	gpio_set_gpio_pin(AVR32_PIN_PA24);

	// Enable SSC interface
	gpio_enable_module(MS3_GPIO_MAP,
			sizeof(MS3_GPIO_MAP) / sizeof(MS3_GPIO_MAP[0]));

	// Interrupt is registered before we enable the PDCA. This to make sure no unhandled
	// interrupt case will be met. (after a CPU reset for example).
	INTC_register_interrupt(&ms3_ssc_tx_pdca_int_handler, AVR32_PDCA_IRQ_0,
			AVR32_INTC_INT2);

	ms3_dac_setup(sample_rate_hz, num_channels, bits_per_sample,
			swap_channels, callback, callback_opt, pba_hz);
}


void ms3_dac_setup(U32 sample_rate_hz, U8 num_channels, U8 bits_per_sample,
		bool swap_channels, void (*callback)(U32 arg), U32 callback_opt,
		U32 pba_hz)
{
	//configure clock
	if (sample_rate_hz < (8000 + 8021) / 2) {
		usb_stream_resync_frequency = 4096000;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}
	else if (sample_rate_hz < (8021 + 22050) / 2) {
		usb_stream_resync_frequency = 4106752;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}
	else if (sample_rate_hz < (22050 + 32000) / 2) {
		usb_stream_resync_frequency = 11289600;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}
	else if (sample_rate_hz < (32000 + 44100) / 2) {
		usb_stream_resync_frequency = 16384000;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}
	else if (sample_rate_hz < (44100 + 48000) / 2) {
		usb_stream_resync_frequency = 22579200;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}
	else if (sample_rate_hz < (48000 + 88200) / 2) {
		usb_stream_resync_frequency = 24576000;
		cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, CS2200_FREF));
	}

	//configure ssc to use clock on TX_CLOCK pin
	AVR32_SSC.tcmr = (0 << AVR32_SSC_TCMR_CKO_OFFSET)
			| (1 << AVR32_SSC_TCMR_STTDLY_OFFSET)
			| (2 << AVR32_SSC_TCMR_CKS_OFFSET)
			| (7 << AVR32_SSC_TCMR_START_OFFSET)
			| (0x1f << AVR32_SSC_TCMR_PERIOD_OFFSET);

	AVR32_SSC.tfmr = (0xf << AVR32_SSC_TFMR_DATLEN_OFFSET)
			| (1 << AVR32_SSC_TFMR_MSBF_OFFSET)
			| (1 << AVR32_SSC_TFMR_FSOS_OFFSET)
			| (1 << AVR32_SSC_TFMR_FSLENHI_OFFSET)
			| (0xf << AVR32_SSC_TFMR_FSLEN_OFFSET);

	AVR32_SSC.cr = AVR32_SSC_CR_TXEN_MASK;

	//configure DMA
	pdca_channel_options_t ms3_ssc_pdca_options = {
		.addr = NULL,
		.size = 0,
		.r_addr = NULL,
		.r_size = 0,
		.pid = AVR32_PDCA_PID_SSC_TX,
		.transfer_size = (bits_per_sample <=  8)?PDCA_TRANSFER_SIZE_BYTE:
				(bits_per_sample <= 16)?PDCA_TRANSFER_SIZE_HALF_WORD:
				PDCA_TRANSFER_SIZE_WORD
	};
	pdca_init_channel(MS3_SSC_TX_PDCA_CHANNEL, &ms3_ssc_pdca_options);
	pdca_enable(MS3_SSC_TX_PDCA_CHANNEL);

	//configure audio parameters
	ms3_output_params.num_channels = num_channels;
	ms3_output_params.callback = callback;
	ms3_output_params.callback_opt = callback_opt;
}

bool ms3_dac_output(void *sample_buffer, size_t sample_length)
{
	bool global_interrupt_enabled;

	if (!(pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) &
			PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO))
		return false;

	if (sample_length) {
		if (ms3_output_params.num_channels == 1) {
			S16 *s16_sample_buffer = sample_buffer;
			int i;

			for (i = sample_length - 1; i >= 0; i--) {
				s16_sample_buffer[2 * i + 1] =
						s16_sample_buffer[2 * i] =
						s16_sample_buffer[i];
			}
		}

		// The PDCA is not able to synchronize its start of transfer with the SSC
		// start of period, so this has to be done by polling the TF pin.
		// Not doing so may result in channels being swapped randomly.
		if ((global_interrupt_enabled = Is_global_interrupt_enabled()))
			Disable_global_interrupt();

		if (pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) &
				PDCA_TRANSFER_COMPLETE) {
			while (gpio_get_pin_value(MS3_SSC_TX_FRAME_SYNC_PIN));
			while (!gpio_get_pin_value(MS3_SSC_TX_FRAME_SYNC_PIN));
		}

		pdca_reload_channel(MS3_SSC_TX_PDCA_CHANNEL, sample_buffer, sample_length * 2);
		pdca_get_reload_size(MS3_SSC_TX_PDCA_CHANNEL);
		if (global_interrupt_enabled)
			Enable_global_interrupt();

		if (ms3_output_params.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
			pdca_enable_interrupt_transfer_complete(MS3_SSC_TX_PDCA_CHANNEL);
		if (ms3_output_params.callback_opt & AUDIO_DAC_RELOAD_CB)
			pdca_enable_interrupt_reload_counter_zero(MS3_SSC_TX_PDCA_CHANNEL);
	}

	return true;
}

static void ms3_write_reg(uint8_t reg, uint8_t data)
{
	int twi_status;
	twi_package_t twi_package = {
		.chip         = MS3_TWI_ADDRESS,
		.addr[0]      = reg,
		.addr_length  = 1,
		.buffer       = &data,
		.length       = sizeof(data)
	};
	do {
		twi_status = twi_master_write(MS3_TWI, &twi_package);
	} while( twi_status != TWI_SUCCESS );
}


static uint16_t ms3_read_reg(uint8_t reg)
{
	int twi_status;
	uint8_t data;
	twi_package_t twi_package = {
		.chip         = MS3_TWI_ADDRESS,
		.addr[0]      = reg,
		.addr_length  = 1,
		.buffer       = &data,
		.length       = sizeof(data)
	};
	do {
		twi_status = twi_master_read(MS3_TWI, &twi_package);
	} while( twi_status != TWI_SUCCESS );

	return data;
}


static uint8_t ms3_get_volume(uint8_t ch)
{
	uint8_t gain = 0;
	switch (ch) {
	case MS3_LEFT_CHANNEL:
		gain = ms3_read_reg(MS3_HP_VOL_L);
		break;
	case MS3_RIGHT_CHANNEL:
	default:
		gain = ms3_read_reg(MS3_HP_VOL_R);
		break;
	}
	return gain;
}

static void ms3_set_volume(uint8_t ch, uint8_t vol)
{
	if (ch & MS3_LEFT_CHANNEL) {
		ms3_write_reg(MS3_HP_VOL_L, vol);
		ms3_write_reg(MS3_SP_VOL_L, vol);
	}
	if (ch & MS3_RIGHT_CHANNEL) {
		ms3_write_reg(MS3_HP_VOL_R, vol);
		ms3_write_reg(MS3_SP_VOL_R, vol);
	}
}

void ms3_dac_increase_volume(void)
{
	//static int8_t vol_state=0;
	uint8_t volume = ms3_get_volume(MS3_LEFT_CHANNEL);
	volume -=MS3_VOL_STEP;
	if (volume < MS3_VOL_MIN)
		volume = MS3_VOL_MIN;
	ms3_set_volume(MS3_LEFT_CHANNEL | MS3_RIGHT_CHANNEL, volume);
}

void ms3_dac_decrease_volume(void)
{
	uint8_t volume = ms3_get_volume(MS3_LEFT_CHANNEL);
	volume += MS3_VOL_STEP;
	if (volume > MS3_VOL_MAX)
		volume = MS3_VOL_MAX;
	ms3_set_volume(MS3_LEFT_CHANNEL | MS3_RIGHT_CHANNEL, volume);
}

void ms3_dac_flush(void)
{
	pdca_disable_interrupt_transfer_complete(MS3_SSC_TX_PDCA_CHANNEL);
	pdca_disable_interrupt_reload_counter_zero(MS3_SSC_TX_PDCA_CHANNEL);

	while (!(pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) &
		PDCA_TRANSFER_COMPLETE));
}

void ms3_dac_stop(void)
{
	ms3_dac_flush();

	pdca_disable(MS3_SSC_TX_PDCA_CHANNEL);

	ssc_i2s_reset(&AVR32_SSC);

	// Set as gpio pin
	gpio_enable_gpio(MS3_GPIO_MAP, sizeof(MS3_GPIO_MAP) / sizeof(MS3_GPIO_MAP[0]));

	ms3_output_params.num_channels = 0;
	ms3_output_params.callback = NULL;
	ms3_output_params.callback_opt = 0;
}

void ms3_dac_set_volume(U8 volume)
{
	uint8_t ms3_volume;

	// Inverse the range (because the MS3 volume goes up when its value goes down)
	volume = (U8) (-((int16_t) volume) + 0xFF);
	// Conversion audio mixer volume [0; 255] to MS3 volume [MS3_VOL_MIN; MS3_VOL_MAX]
	ms3_volume = ((U16) (MS3_VOL_MAX - MS3_VOL_MIN) * volume) / 255;
	ms3_volume += MS3_VOL_MIN;

	ms3_set_volume(MS3_LEFT_CHANNEL | MS3_RIGHT_CHANNEL, ms3_volume);
}

U8 ms3_dac_get_volume(void)
{
	return 0;
}

bool ms3_dac_is_volume_boosted(void)
{
	return false;
}

void ms3_dac_mute(bool mute)
{
	if (mute) {
		U32 save_dac_reload_callback_opt;

		// Mute all channels
		// ms3_write_reg(MS3_MUTE_VOL_CTRL, 0xAF);

		// Disable the reload callback function
		save_dac_reload_callback_opt = ms3_output_params.callback_opt;
		ms3_output_params.callback_opt = 0;
		// Disable the transfer complete interruption and wait until the transfer is complete
		pdca_disable_interrupt_reload_counter_zero(MS3_SSC_TX_PDCA_CHANNEL);
		while (!(pdca_get_transfer_status(MS3_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE));
		// Re-enable the reload callback function
		ms3_output_params.callback_opt = save_dac_reload_callback_opt;
		// Set as gpio pin
		gpio_enable_gpio(MS3_GPIO_MAP, sizeof(MS3_GPIO_MAP) / sizeof(MS3_GPIO_MAP[0]));
	}
	else {
		// Re-enable the reload interrupt
		pdca_enable_interrupt_reload_counter_zero(MS3_SSC_TX_PDCA_CHANNEL);
		// Unmute all channels
		// ms3_write_reg(MS3_MUTE_VOL_CTRL, 0xA0);
		// Enable SSC interface
		gpio_enable_module(MS3_GPIO_MAP, sizeof(MS3_GPIO_MAP) / sizeof(MS3_GPIO_MAP[0]));
	}
}

bool ms3_dac_is_volume_muted(void)
{
	uint8_t mute;
	mute = ms3_read_reg(MS3_MUTE_VOL_CTRL);
	return ((mute & 0xF) == 0xF);
}
