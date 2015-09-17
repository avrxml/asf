/*****************************************************************************
 *
 * \file
 *
 * \brief A streaming IMA/DVI ADPCM player
 *
 * This file show a way to use the IMA/DVI ADPCM decoding functions with a full example.
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
 ******************************************************************************/

/*! \mainpage
 *
 * \section intro Introduction
 * This is a full example of a IMA/DVI ADPCM WAV decoder.\n
 * It uses the USART as a display to show wav file information and
 * two mixed PWM to play through a 16 bits precision sample sound.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section interface Hardware Interface
 * Here is the hardware interface used to mixed the two PWM, to make them act as a 16 bits DAC.
 * \image html pwm_to_dac_mixer.jpg "PWM to DAC mixer"
 *
 * \section configuration Configuration
 * - To change the PWM DAC channel, first identify which pins and functions are associated with the
 * PWM channels and then modify the defines \ref DAC_PWM_CHANNEL_LSB, \ref DAC_PWM_CHANNEL_LSB_PIN,
 * \ref DAC_PWM_CHANNEL_LSB_FUNCTION, \ref DAC_PWM_CHANNEL_MSB, \ref DAC_PWM_CHANNEL_MSB_PIN and
 * \ref DAC_PWM_CHANNEL_MSB_FUNCTION.
 * - To use another audio file, copy this file into the ADPCM_WAV_PLAYER directory where the sample
 * file is already located. Then update the line \code .incbin "enterprise_ima-dvi.wav" \endcode in the file "load.S" (for GCC)
 * or update the following field in the project options for IAR:
 * \image html iar_audio_file_update.jpg "IAR audio file update"
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for 32-bit AVRs and for IAR Embedded Workbench
 * for Atmel 32-bit AVR. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All 32-bit AVR devices with an USART, PWM and PDCA modules can be used.
 *
 * \section configinfo Configuration Information
 * To make this example works, you will need a PC and a serial cable
 * to connect the board to the PC.\n
 * This example has been tested with the following configuration:
 * - EVK1100 or EVK1101 evaluation kit;
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include <stdio.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"

#include "pm.h"
#include "gpio.h"
#include "usart.h"
#include "intc.h"
#include "pwm.h"
#include "flashc.h"
#include "preprocessor.h"

#include "ima_dvi_adpcm_wav.h"

//! The clock frequency
#define CP_PBA_SPEED              48000000
#define DAC_PWM_FREQUENCY         (CP_PBA_SPEED >> 8)


#define SWITCH_ENDIANNESS(width, data)  (TPASTE2(Swap, width)(data))


// Structures used to parse the WAVE file
//******************************************
#define WAVE_FORMAT_DVI_ADPCM  0x0011

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long

#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  BYTE riff_type[4];
}s_wave_riff;
#if __ICCAVR32__
# pragma pack()
#endif

#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED
{
  BYTE chunk_id[4];
  DWORD chunk_size;
  WORD compression_code;
  WORD nb_channels;
  DWORD sample_rate;
  DWORD avg_bytes_per_sec;
  WORD block_align;
  WORD bits_per_sample;
  WORD extra_bytes;
}s_wave_fmt;
#if __ICCAVR32__
# pragma pack()
#endif

#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED
{
  s_wave_fmt fmt;
  WORD samples_per_block;
}s_wave_fmt_dvi;
#if __ICCAVR32__
# pragma pack()
#endif

#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED
{
  BYTE chunk_id[4];
  DWORD chunk_size;
}s_wave_data;
#if __ICCAVR32__
# pragma pack()
#endif

#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED
{
  WORD isamp0;
  BYTE step_table_index;
  BYTE reserved;
}s_wave_dvi_block_header;
#if __ICCAVR32__
# pragma pack()
#endif

//******************************************

#define MAX_SEARCH_LENGTH 256
/*
 * Get a structure starting with the specific string from the wave data
 */
int fget_struct(const char *_data, char *_ptr, int size, char *_start_str)
{
	int i = 0, j, len;

	while (1)
	{
		j = 0;
		while (*(_data+i) != *_start_str && i < MAX_SEARCH_LENGTH)
			i++;
		if (i >= MAX_SEARCH_LENGTH)
			return 0;
		while (*(_data+i+j) == *(_start_str+j) && *(_start_str+j) != '\0' && j < MAX_SEARCH_LENGTH)
			j++;
		if (j >= MAX_SEARCH_LENGTH)
			return 0;
		if (*(_start_str+j) == '\0')
		{
			for (len = 0; len < size; len++)
				*(_ptr+len) = *(_data+i+len);
			return i+len;
		}
	}

//	_start = strstr(_data, _start_str);	// Does not work on binary data
}


//! A flag to tell if we need to play a new sample with the DAC
volatile int get_sample = 0;

//! The offset of the current reading buffer
int cur_i_read_buffer = 0;
//! Step index for the adpcm decoder
A_ALIGNED short step_index;
//! Predicted value for the adpcm decoder
A_ALIGNED short predicted_value;

volatile int dac_pwm_period;
int dac_pwm_periods_per_sample;

/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DAC_PWM_CHANNEL_LSB         1
#  define DAC_PWM_CHANNEL_LSB_PIN
#  define DAC_PWM_CHANNEL_MSB         0
#  define DAC_PWM_CHANNEL_MSB_PIN
#elif BOARD == EVK1101
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DAC_PWM_CHANNEL_LSB       6
#  define DAC_PWM_CHANNEL_LSB_PIN   _0
#  define DAC_PWM_CHANNEL_MSB       4
#  define DAC_PWM_CHANNEL_MSB_PIN   _1
#endif
//! @}

/*! \brief This function is used by the debugging module. It permits to print a string through the USART.
 * \param str The input string to print.
 * \pre It as to be used after having initializing the USART module.
 */
void print_fct(char *str)
{
  usart_write_line(EXAMPLE_USART, str);
}

//! this function initializes the USART module at 115200 bauds
void init_usart()
{
  static const gpio_map_t USART_GPIO_MAP =
  {
    {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
    {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
  };

  // Configuration structure for the USART module
  static const usart_options_t USART_OPTIONS =
  {
    // Baudrate at 115200 bauds
    .baudrate     = 115200,
    // 8 bits per characters
    .charlength   = 8,
    // No parity
    .paritytype   = USART_NO_PARITY,
    // One stop bit
    .stopbits     = USART_1_STOPBIT,
    // Mode normal
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to USART.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode.
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FOSC0*2);
}

/*!
 * \brief The PWM interrupt handler.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dac_pwm_int_handler(void)
{
  AVR32_PWM.isr;

  if (!dac_pwm_period--)
    get_sample = 1;
}


//! Initialize the PWM to make it works as a DAC
void dac_pwm_init()
{
  static const gpio_map_t DAC_PWM_GPIO_MAP =
  {
    {ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_LSB, DAC_PWM_CHANNEL_LSB_PIN, _PIN),
     ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_LSB, DAC_PWM_CHANNEL_LSB_PIN, _FUNCTION)},
    {ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_MSB, DAC_PWM_CHANNEL_MSB_PIN, _PIN),
     ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_MSB, DAC_PWM_CHANNEL_MSB_PIN, _FUNCTION)}
  };

  INTC_register_interrupt(&dac_pwm_int_handler, AVR32_PWM_IRQ, AVR32_INTC_INT3);

  gpio_enable_module(DAC_PWM_GPIO_MAP, sizeof(DAC_PWM_GPIO_MAP) / sizeof(DAC_PWM_GPIO_MAP[0]));

  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cmr = ((AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) |
                                                AVR32_PWM_CPOL_MASK) &
                                               ~(AVR32_PWM_CALG_MASK |
                                                 AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cprd = 0x000000FF;
  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cdty = 0x00;

  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cmr = ((AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) |
                                                AVR32_PWM_CPOL_MASK) &
                                               ~(AVR32_PWM_CALG_MASK |
                                                 AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cprd = 0x000000FF;
  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cdty = 0x80;

  AVR32_PWM.ena = (1 << DAC_PWM_CHANNEL_LSB) | (1 << DAC_PWM_CHANNEL_MSB);
  AVR32_PWM.ier = (1 << DAC_PWM_CHANNEL_LSB) | (1 << DAC_PWM_CHANNEL_MSB);
}



//! To set the clock frequency to 48MHz
void sys_cpu_48MHz(void)
{
  // Switch to external Oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Set PLL1 @ 96 MHz from Osc0
  pm_pll_setup(&AVR32_PM, 0,   // pll
                   15,   // mul
                   1,   // div
                   0,   // osc
                   16); // lockcount

  // PLL clock is lower than 160MHz: need to set pllopt.
  pm_pll_set_option(&AVR32_PM
    , 0 // pll
    , 0 // pll_freq
    , 1 // pll_div2
    , 0 // pll_wbwdisable
  );

  // Enable PLL1
  pm_pll_enable(&AVR32_PM, 0);

  // Wait for PLL0 locked
  pm_wait_for_pll0_locked(&AVR32_PM);
  pm_cksel(&AVR32_PM, TRUE, 1, TRUE, 0, TRUE, 0);
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);
}


//! The main function
int main(int argc, char *argv[])
{
	char c;
	int sample_rate;
	int i, j, even, len;
	int sound_ampl;
	s_wave_riff header_riff;
	s_wave_fmt_dvi header_dvi;
	s_wave_data header_data;
	s_wave_dvi_block_header header_block;
	short step_index;
	short predicted_value;
	char _buffer[4];
	char text[128];
	int nb_batches_per_block;
	int batches_read;
	int length;
	char * sound;

	extern const char sound_data[];
	sound = (char *) &sound_data[0];

	// Set the clock frequency to 48MHz
	sys_cpu_48MHz();
	flashc_set_wait_state(1);

	// Disable all interrupts.
	Disable_global_interrupt();

	// Initialize the USART
	init_usart();

	usart_write_line(EXAMPLE_USART, "\n\nIMA/DVI ADPCM decoder\r\n\r\n");

	if (!(length = fget_struct(sound, (char *) &header_riff, sizeof(s_wave_riff), "RIFF")))
	{
		usart_write_line(EXAMPLE_USART, "Invalid RIFF File.\r\n");
		return 0;
	}
	sound += length;
	if (!(length = fget_struct(sound, (char *)&header_dvi, sizeof(s_wave_fmt_dvi), "fmt ")))
	{
		usart_write_line(EXAMPLE_USART, "Invalid RIFF Format.\r\n");
		return 0;
	}
	sound += length;
	if (!(length = fget_struct(sound, (char *)&header_data, sizeof(s_wave_data), "data")))
	{
		usart_write_line(EXAMPLE_USART, "Invalid RIFF Data.\r\n");
		return 0;
	}
	sound += length;

	header_data.chunk_size = SWITCH_ENDIANNESS(32, header_data.chunk_size);
	header_dvi.fmt.compression_code = SWITCH_ENDIANNESS(16, header_dvi.fmt.compression_code);
	header_dvi.fmt.nb_channels = SWITCH_ENDIANNESS(16, header_dvi.fmt.nb_channels);
	header_dvi.fmt.sample_rate = SWITCH_ENDIANNESS(32, header_dvi.fmt.sample_rate);
	header_dvi.fmt.avg_bytes_per_sec = SWITCH_ENDIANNESS(32, header_dvi.fmt.avg_bytes_per_sec);
	header_dvi.fmt.block_align = SWITCH_ENDIANNESS(16, header_dvi.fmt.block_align);
	header_dvi.fmt.bits_per_sample = SWITCH_ENDIANNESS(16, header_dvi.fmt.bits_per_sample);
	header_dvi.fmt.extra_bytes = SWITCH_ENDIANNESS(16, header_dvi.fmt.extra_bytes);
	header_dvi.samples_per_block = SWITCH_ENDIANNESS(16, header_dvi.samples_per_block);

	if (header_dvi.fmt.compression_code != WAVE_FORMAT_DVI_ADPCM)
	{
		usart_write_line(EXAMPLE_USART, "Invalid IMA/DVI ADPCM File.\r\n");
		return 0;
	}

	if (header_dvi.fmt.bits_per_sample != 4)
	{
		usart_write_line(EXAMPLE_USART, "Error! The input adpcm wav file must have a number of bits per sample equals to 4.\r\n");
		return 0;
	}

	sprintf(text, "Number of channels: %d\r\n", header_dvi.fmt.nb_channels);
	usart_write_line(EXAMPLE_USART, text);
	sprintf(text, "Sample rate: %d Hz\r\n", (int)header_dvi.fmt.sample_rate);
	usart_write_line(EXAMPLE_USART, text);
	sprintf(text, "Total average data rate: %d\r\n", (int)header_dvi.fmt.avg_bytes_per_sec);
	usart_write_line(EXAMPLE_USART, text);
	sprintf(text, "Block alignment: %d bytes\r\n", header_dvi.fmt.block_align);
	usart_write_line(EXAMPLE_USART, text);
	sprintf(text, "Number of bits per sample: %d bits\r\n", header_dvi.fmt.bits_per_sample);
	usart_write_line(EXAMPLE_USART, text);
	sprintf(text, "Number of samples per channel per Block: %d samples\r\n", header_dvi.samples_per_block);
	usart_write_line(EXAMPLE_USART, text);

	if (header_dvi.fmt.nb_channels > 1)
		usart_write_line(EXAMPLE_USART, "This program will only decode the last channel of the input wav file.\r\n");


	// Support only the 4 bits per sample format.
	nb_batches_per_block = (header_dvi.fmt.block_align/(4*header_dvi.fmt.nb_channels)-1);

	usart_write_line(EXAMPLE_USART, "Start playback...\r\n");

	sample_rate = header_dvi.fmt.sample_rate;
	dac_pwm_periods_per_sample = (DAC_PWM_FREQUENCY + (sample_rate >> 1))/(sample_rate << 1);

	// Initialize interrupt vectors.
	INTC_init_interrupts();

	// Initialize the PWM
	dac_pwm_init();

	// Enable all interrupts.
	Enable_global_interrupt();

	for(j=0; j<header_data.chunk_size/header_dvi.fmt.block_align; j++)
	{
		// Read the last channel
		for(i=0; i<header_dvi.fmt.nb_channels; i++)
		{
			char * _header_ptr = (char *) &header_block;
			for (len = 0; len < sizeof(s_wave_dvi_block_header); len++)
				*(_header_ptr+len) = *(sound+len);
			sound += sizeof(s_wave_dvi_block_header);
		}
		predicted_value = SWITCH_ENDIANNESS(16, header_block.isamp0);
		step_index = header_block.step_table_index;
		batches_read = 0;

		// Ignore the first channels
		for(i=0; i<header_dvi.fmt.nb_channels; i++)
		{
			for (len = 0; len < 4; len++)
				*(_buffer+len) = *(sound+len);
			sound += 4;
		}
		batches_read++;

		cur_i_read_buffer = 0;
		dac_pwm_period = 0;
		get_sample = 0;

		even = 0;
		while(1)
		{
			if (get_sample)
			{
                                get_sample = 0;
				dac_pwm_period = dac_pwm_periods_per_sample - 1;

				if (!even)
					c = *(_buffer+cur_i_read_buffer) & 0xF;
				else
				{
					c = (*(_buffer+cur_i_read_buffer) >> 4) & 0xF;
					cur_i_read_buffer++;
				}
				even = !even;

				sound_ampl = ((U16) dsp_adpcm_ima_decode_nibble(c, &step_index, &predicted_value)) + 0x8000;
				AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cupd = (sound_ampl >> 8) & 0xFF;
				AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cupd = sound_ampl & 0xFF;

				// If buffer is empty, refill it
				if (cur_i_read_buffer >= 4)
				{
					if (batches_read >= nb_batches_per_block)
						break;
					cur_i_read_buffer = 0;
					for(i=0; i<header_dvi.fmt.nb_channels; i++)
					{
						for (len = 0; len < 4; len++)
							*(_buffer+len) = *(sound+len);
						sound += 4;
					}
					batches_read++;
				}
			}
		}
	}
	usart_write_line(EXAMPLE_USART, "Playback completed\r\n");
	return 0;
}
