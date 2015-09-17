/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB Audio streaming player.
 *
 * "USB stream" means that a device and a host are reading/writing information
 * through the USB at a specific sampling rate. But, in reality, even if the
 * sampling rate is identical for both products, there is no guaranty that they
 * are strictly equivalent. More over, some jitter may also appear. The aim of
 * the usb_stream_control module is to ensure a good audio playback by gently
 * re-synchronising the stream.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include "conf_usb.h"
#include "conf_audio_player.h"
#include "audio_mixer.h"
#include "usb_drv.h"
#include "usb_stream_player.h"
#include "dsp.h"
#include "cycle_counter.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#if AUDIO_STREAM_NB_INPUT_BUFFERS < 4
  #error AUDIO_STREAM_NB_INPUT_BUFFERS must be equal or greater than 4.
#endif
#if AUDIO_STREAM_NB_OUTPUT_BUFFERS < 2
  #warning If AUDIO_STREAM_NB_OUTPUT_BUFFERS is smaller than 2, it will not take advantage of the double buffering.
#endif

static inline void *usb_stream_get_current_buffer_for_writing(int channel);
static inline int usb_stream_current_buffer_size_left(void);
static t_cpu_time broken_stream_timer;
#define BROKEN_STREAM_TIMER     4000 // unit is in ms.

//_____ D E C L A R A T I O N S ____________________________________________

usb_stream_context_t *usb_stream_context = NULL;
volatile uint16_t usb_stream_cnt=0;

uint8_t usb_stream_get_status(void)
{
  if (!usb_stream_context)
    return USB_STREAM_STATUS_NOT_INITIALIZED;
  return usb_stream_context->status;
}

void player_init(void)
{
  int i, j;

  // Allocate memory for the context structure
  if (!(usb_stream_context = (usb_stream_context_t *) malloc(sizeof(usb_stream_context_t))))
    return;   // No error handling
  memset(usb_stream_context, 0, sizeof(*usb_stream_context));

  // Clear the error flag
  usb_stream_context->error = USB_STREAM_ERROR_NONE;

  // Defines the size of an input buffer
  usb_stream_context->input_buffer_size = AUDIO_STREAM_MAX_USB_PACKET_SIZE;

  // Allocate memory for the input buffer structure
  for(i=0; i<AUDIO_STREAM_NB_INPUT_BUFFERS; i++)
  {
    if (!(usb_stream_context->input_buffers[i] = (buffer_t *) malloc(sizeof(buffer_t))))
    {
      usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
      return;
    }
    // Allocate memory for input buffers
    for(j=0; j<AUDIO_STREAM_MAX_NB_CHANNELS; j++)
    {
      if (!(usb_stream_context->input_buffers[i]->buffer[j] = (dsp16_t *) malloc(usb_stream_context->input_buffer_size*sizeof(dsp16_t))))
      {
        usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
        return;
      }
    }
  }
}

static bool usb_stream_process(void)
{
  buffer_t *input_buffer;
#if (defined __GNUC__)
  dsp16_t * volatile dsp16_output_buffer;
#else
  dsp16_t * dsp16_output_buffer;
#endif
  int i, channel, size = 0;

  // If no buffer is ready, then set under flow marker
  if (usb_stream_context->current_full_buffer == -1)
    return false;

  // If the audio DAC is not ready to receive more data, then return
  if (!audio_mixer_dacs_output_direct(NULL, 0))
    return false;

  // Select the current input buffer and make sure it is valid
  input_buffer = usb_stream_context->input_buffers[usb_stream_context->current_full_buffer];

  // Make sure this buffer is not already in the process of resampling
  if (input_buffer->buffer_state != BUFFER_STATE_FULL)
    return false;

  // Mark this buffer as a "resampling" buffer
  input_buffer->buffer_state = BUFFER_STATE_RESAMPLING;

  // Select the current output buffer
  dsp16_output_buffer = (dsp16_t *) usb_stream_context->output_buffers[usb_stream_context->current_output_buffer];

  for (channel=0; channel<usb_stream_context->nb_channels; channel++)
  {
    // Interpolates the current channel buffer
    dsp16_resampling_compute(usb_stream_context->ctx->resampling,
                             usb_stream_context->output_temp_buffer,
                             input_buffer->buffer[channel],
                             channel);
    size = dsp16_resampling_get_output_current_buffer_size(usb_stream_context->ctx->resampling);
    // Fill the array with audio samples and mix the channels
    switch (usb_stream_context->nb_channels)
    {
    case 1:
      memcpy(dsp16_output_buffer, usb_stream_context->output_temp_buffer, usb_stream_context->input_buffer_size);
      break;
    case 2:
      for (i=0; i<size; i++)
        dsp16_output_buffer[i*2] = ((dsp16_t *) usb_stream_context->output_temp_buffer)[i];
      break;
    default:
      for (i=0; i<size; i++)
        dsp16_output_buffer[i*usb_stream_context->nb_channels] = ((dsp16_t *) usb_stream_context->output_temp_buffer)[i];
    }
    // change channel
    dsp16_output_buffer++;
  }

  // Re-selects the current output buffer
  dsp16_output_buffer = (dsp16_t *) usb_stream_context->output_buffers[usb_stream_context->current_output_buffer];

  // Scale the output buffer if the scaling is set
  if (usb_stream_context->ctx->gain)
    dsp16_vect_realdiv(dsp16_output_buffer, dsp16_output_buffer, size * usb_stream_context->nb_channels, usb_stream_context->ctx->gain);

  // Send the buffer to the output channel
  audio_mixer_dacs_output_direct(dsp16_output_buffer, size);

  // Set the current input buffer as an empty buffer
  input_buffer->buffer_state = BUFFER_STATE_EMPTY;

  // Update the current output buffer index
  usb_stream_context->current_output_buffer = (usb_stream_context->current_output_buffer+1)%AUDIO_STREAM_NB_OUTPUT_BUFFERS;

  // Update the current full buffer pointer
  i = (usb_stream_context->current_full_buffer+1)%AUDIO_STREAM_NB_INPUT_BUFFERS;
  input_buffer = usb_stream_context->input_buffers[i];
  if (input_buffer->buffer_state != BUFFER_STATE_FULL)
    i = -1;
  usb_stream_context->current_full_buffer = i;

  return true;
}

//!
//! @brief This callback function is called when the DAC interrupt has sent
//! the buffer 'n-1' and switches to buffer 'n'. The aim of this function is thus to prepare
//! the buffer 'n+1'; so that there is always a pending buffer for the interrupt.
//!
void dac_sample_sent_cb(void)
{
  if (usb_stream_context->current_full_buffer == -1)
    usb_stream_context->error = USB_STREAM_ERROR_UNDERFLOW;
}

//!
//! @brief This callback function is called when the PWM DAC interrupt does not have
//! any more audio samples (i.e. "famine").
//!
void dac_underflow_cb(void)
{
  usb_stream_context->error = USB_STREAM_ERROR_UNDERFLOW;
  usb_stream_context->synchronized = false;
}

void usb_stream_out_of_samples(void)
{
  usb_stream_context->error = USB_STREAM_ERROR_UNDERFLOW;
  usb_stream_context->synchronized = false;
}

void usb_stream_close(void)
{
  int i;

  // Reset status to not initialized
  usb_stream_context->status = USB_STREAM_STATUS_NOT_INITIALIZED;

  if (usb_stream_context->ctx_overrun.resampling)
    dsp16_resampling_free(usb_stream_context->ctx_overrun.resampling, (free_fct_t) free);
  if (usb_stream_context->ctx_underrun.resampling)
    dsp16_resampling_free(usb_stream_context->ctx_underrun.resampling, (free_fct_t) free);
  if (usb_stream_context->ctx_original.resampling)
    dsp16_resampling_free(usb_stream_context->ctx_original.resampling, (free_fct_t) free);

  usb_stream_context->ctx_original.resampling = NULL;
  usb_stream_context->ctx_underrun.resampling = NULL;
  usb_stream_context->ctx_overrun.resampling = NULL;
  usb_stream_context->ctx = NULL;

  // Free also the output buffers
  for(i=0; i<AUDIO_STREAM_NB_OUTPUT_BUFFERS; i++)
  {
    if (usb_stream_context->output_buffers[i])
    {
      free(usb_stream_context->output_buffers[i]);
      usb_stream_context->output_buffers[i] = NULL;
    }
  }
  // Free the temporary output buffer
  if (usb_stream_context->output_temp_buffer)
    free(usb_stream_context->output_temp_buffer);
  usb_stream_context->output_temp_buffer = NULL;
}

//!
//! @brief This function initializes the USB Stream driver.
//!
void usb_stream_init(uint32_t sample_rate_hz, uint8_t num_channels, uint8_t bits_per_sample, bool swap_channels)
{
  const resampling_config_t default_param = AUDIO_STREAM_IN_OUT_FS_DEFAULT_VALUE;
  const resampling_config_t translation_tab[] = AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB;

  int i;
  int max_buffer_size;
  int underrun_fs, overrun_fs, order;
  dsp16_resampling_options_t options;

  // Reset the structure
  memset(&options, 0, sizeof(dsp16_resampling_options_t));
  // Add default values
  options.coefficients_generation = DSP16_RESAMPLING_OPTIONS_USE_DYNAMIC;
  options.dynamic.coefficients_normalization = true;

  if (num_channels == 1)
  {
    num_channels = 2;
    usb_stream_context->duplicate_channels = true;
  }
  else
    usb_stream_context->duplicate_channels = false;

  // Free the existing context if any
  usb_stream_close();
  // Clear the error flag
  usb_stream_context->error = USB_STREAM_ERROR_NONE;

  // Choose which output sampling rate to choose
  usb_stream_context->fs_output = default_param.output_fs_hz;
  underrun_fs = default_param.output_underrun_fs_hz;
  overrun_fs = default_param.output_overrun_fs_hz;
  order = default_param.order;
  usb_stream_context->ctx_original.gain = default_param.gain;
  usb_stream_context->ctx_underrun.gain = default_param.gain_underrun;
  usb_stream_context->ctx_overrun.gain = default_param.gain_overrun;
  for (i=0; i<sizeof(translation_tab) / sizeof(translation_tab[0]); i++)
  {
    if (translation_tab[i].input_fs_hz == sample_rate_hz)
    {
      usb_stream_context->fs_output = translation_tab[i].output_fs_hz;
      underrun_fs = translation_tab[i].output_underrun_fs_hz;
      overrun_fs = translation_tab[i].output_overrun_fs_hz;
      order = translation_tab[i].order;
      // Set gain stage for each context
      usb_stream_context->ctx_original.gain = translation_tab[i].gain;
      usb_stream_context->ctx_underrun.gain = translation_tab[i].gain_underrun;
      usb_stream_context->ctx_overrun.gain = translation_tab[i].gain_overrun;
      break;
    }
  }

  if (num_channels > AUDIO_STREAM_MAX_NB_CHANNELS)
  {
    usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
    return;
  }

  // Create a new resampling context
  usb_stream_context->ctx_original.resampling = dsp16_resampling_setup(sample_rate_hz,
                                                          usb_stream_context->fs_output,
                                                          usb_stream_context->input_buffer_size,
                                                          order,
                                                          num_channels,
                                                          (malloc_fct_t) malloc,
                                                          &options);

  usb_stream_context->ctx_overrun.resampling = dsp16_resampling_setup(sample_rate_hz,
                                                          overrun_fs,
                                                          usb_stream_context->input_buffer_size,
                                                          order,
                                                          num_channels,
                                                          (malloc_fct_t) malloc,
                                                          &options);

  usb_stream_context->ctx_underrun.resampling = dsp16_resampling_setup(sample_rate_hz,
                                                          underrun_fs,
                                                          usb_stream_context->input_buffer_size,
                                                          order,
                                                          num_channels,
                                                          (malloc_fct_t) malloc,
                                                          &options);

  if (!usb_stream_context->ctx_original.resampling
   || !usb_stream_context->ctx_overrun.resampling
   || !usb_stream_context->ctx_underrun.resampling)
  {
    usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
    usb_stream_close();
    return;
  }

  // Get the maximum size of the output buffer (the same will be used with the different frequencies)
  max_buffer_size = dsp16_resampling_get_output_max_buffer_size(usb_stream_context->ctx_original.resampling);
  max_buffer_size = max(max_buffer_size, dsp16_resampling_get_output_max_buffer_size(usb_stream_context->ctx_overrun.resampling));
  max_buffer_size = max(max_buffer_size, dsp16_resampling_get_output_max_buffer_size(usb_stream_context->ctx_underrun.resampling));

  // Allocate memory for the output buffers
  for(i=0; i<AUDIO_STREAM_NB_OUTPUT_BUFFERS; i++)
  {
    if (!(usb_stream_context->output_buffers[i] = malloc(sizeof(dsp16_t)*(max_buffer_size + 1)*num_channels)))
    {
      usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
      usb_stream_close();
      return;
    }
  }
  // Allocate memory for the temporary output buffer
  if (!(usb_stream_context->output_temp_buffer = malloc(sizeof(dsp16_t)*max_buffer_size)))
  {
    usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
    usb_stream_close();
    return;
  }

  // Fills the context structure
  usb_stream_context->error = USB_STREAM_ERROR_NONE;
  usb_stream_context->nb_channels = num_channels;

  if (bits_per_sample == 32)
    bits_per_sample = 16;
  usb_stream_context->bits_per_sample = bits_per_sample;
  usb_stream_context->swap_channels = swap_channels;

  // No full buffer available yet
  usb_stream_context->current_full_buffer = -1;

  // Set buffers to "initialization" state
  usb_stream_context->current_buffer = 0;
  for(i=0; i<AUDIO_STREAM_NB_INPUT_BUFFERS; i++)
    usb_stream_context->input_buffers[i]->buffer_state = BUFFER_STATE_EMPTY;
  usb_stream_context->synchronized = false;

  // Configure the DAC
  audio_mixer_dacs_setup_direct(usb_stream_context->fs_output,
                         num_channels,
                         bits_per_sample,
                         swap_channels);

  // Use by default the original re-sampling setup
  usb_stream_context->ctx = &usb_stream_context->ctx_original;

  // Set status to initialized
  usb_stream_context->status = USB_STREAM_STATUS_IDLE;

  // Start the broken stream timer
  cpu_set_timeout(cpu_ms_2_cy(BROKEN_STREAM_TIMER, FCPU_HZ), &broken_stream_timer);
}

//! Returns the length of the free part of the current buffer size
static inline int usb_stream_current_buffer_size_left(void)
{
  return usb_stream_context->input_buffer_size - usb_stream_context->current_buffer_index;
}

//! Returns the number of buffer filtered by state
static int usb_stream_count_nb_buffer(buffer_state_t state)
{
  int i_buffer, count = 0;
  buffer_t *current_buffer;

  for (i_buffer=0; i_buffer<AUDIO_STREAM_NB_INPUT_BUFFERS; i_buffer++)
  {
    current_buffer = usb_stream_context->input_buffers[i_buffer];
    if (current_buffer->buffer_state == state)
      count++;
  }

  return count;
}

//! Update usb_stream_context->current_buffer_index
//!        usb_stream_context->current_buffer
//! \param written_buffer_size The size of sample written on 1 channel.
static void usb_stream_update_buffers(int written_buffer_size)
{
  buffer_t *current_buffer;
  int current_buffer_size = usb_stream_context->current_buffer_index + written_buffer_size;

  while(current_buffer_size >= usb_stream_context->input_buffer_size)
  {
    current_buffer = usb_stream_context->input_buffers[usb_stream_context->current_buffer];
    // Mark the current buffer as FULL if this is not part of the initialization process
    if (current_buffer->buffer_state == BUFFER_STATE_FILLING)
    {
      // If there is no current full buffer, mark this one
      if (usb_stream_context->current_full_buffer == -1)
        usb_stream_context->current_full_buffer = usb_stream_context->current_buffer;
    }
    current_buffer->buffer_state = BUFFER_STATE_FULL;
    // Update the usb_stream_context->current_buffer
    usb_stream_context->current_buffer = (usb_stream_context->current_buffer+1)%AUDIO_STREAM_NB_INPUT_BUFFERS;
    current_buffer_size -= usb_stream_context->input_buffer_size;

    usb_stream_context->error = USB_STREAM_ERROR_NONE;
  }
  usb_stream_context->current_buffer_index = current_buffer_size;

  // Current buffer
  current_buffer = usb_stream_context->input_buffers[usb_stream_context->current_buffer];

  // Mark this buffer as "filling"
  switch(current_buffer->buffer_state)
  {
  case BUFFER_STATE_EMPTY:
    current_buffer->buffer_state = BUFFER_STATE_FILLING;
    break;
  default:
  case BUFFER_STATE_FILLING:
    break;
  case BUFFER_STATE_FULL:
  case BUFFER_STATE_RESAMPLING:
    usb_stream_context->error = USB_STREAM_ERROR_OVERFLOW;
    break;
  }
}

//! Returns a pointer on free space where to store new samples
static inline void *usb_stream_get_current_buffer_for_writing(int channel)
{
  buffer_t *current_buffer = usb_stream_context->input_buffers[usb_stream_context->current_buffer];
  if (current_buffer->buffer_state == BUFFER_STATE_FULL ||
      current_buffer->buffer_state == BUFFER_STATE_RESAMPLING)
    return NULL;
  return &current_buffer->buffer[channel][usb_stream_context->current_buffer_index];
}

static void usb_stream_fill_input_buffers(void *buffer, int size, int nb_channels, int bits_per_sample)
{
  int i, j;
  int buffer_size;

  if (usb_stream_context->error == USB_STREAM_ERROR_UNSUPPORTED)
    return;

  buffer_size = size;
  // Swap samples since they are coming from the USB world
  if (bits_per_sample == 16)
  {
    uint16_t *u16_buffer = (uint16_t *) buffer;
    dsp16_t *out_buffer[AUDIO_STREAM_MAX_NB_CHANNELS];
    int buffer_size_current_buffer;

    buffer_size /= sizeof(uint16_t);
    switch (nb_channels)
    {
    // Optimized for 2 channels
    case 2:
      while (buffer_size)
      {
        if (!(out_buffer[0] = (dsp16_t *) usb_stream_get_current_buffer_for_writing(0)))
          break;
        if (!(out_buffer[1] = (dsp16_t *) usb_stream_get_current_buffer_for_writing(1)))
          break;
        buffer_size_current_buffer = Min(buffer_size, usb_stream_current_buffer_size_left()*2);
        for (i=0; i<buffer_size_current_buffer; i += 2)
        {
          *out_buffer[0]++ = swap16(u16_buffer[i]);
          *out_buffer[1]++ = swap16(u16_buffer[i + 1]);
        }
        buffer_size -= buffer_size_current_buffer;
        u16_buffer += i;
        // Because 2 channels
        usb_stream_update_buffers(buffer_size_current_buffer/2);
      }
      break;
    default:
      while (buffer_size)
      {
        for (j=0; j<nb_channels; j++)
          if (!(out_buffer[j] = (dsp16_t *) usb_stream_get_current_buffer_for_writing(j)))
            break;
        if (j == nb_channels)
        {
          buffer_size_current_buffer = Min(buffer_size, usb_stream_current_buffer_size_left()*nb_channels);
          for (i=0; i<buffer_size_current_buffer; i += nb_channels)
            for (j=0; j<nb_channels; j++)
              *(out_buffer[j])++ = swap16(u16_buffer[i + j]);
          buffer_size -= buffer_size_current_buffer;
          u16_buffer += i;
          usb_stream_update_buffers(buffer_size_current_buffer/nb_channels);
        }
      }
    }
    buffer = (void *) u16_buffer;
  }
  else if (bits_per_sample == 32)
  {
    uint32_t *u32_buffer = (uint32_t *) buffer;
    dsp16_t *out_buffer[AUDIO_STREAM_MAX_NB_CHANNELS];
    int buffer_size_current_buffer;

    buffer_size /= sizeof(uint32_t);
    while (buffer_size)
    {
      for (j=0; j<nb_channels; j++)
        if (!(out_buffer[j] = (dsp16_t *) usb_stream_get_current_buffer_for_writing(j)))
          break;
      if (j == nb_channels)
      {
        buffer_size_current_buffer = Min(buffer_size, usb_stream_current_buffer_size_left()*nb_channels);
        // Swap and convert to 16-bit data
        for(i=0; i<buffer_size_current_buffer; i += nb_channels)
          for(j=0; j<nb_channels; j++)
            *(out_buffer[j])++ = (swap32(u32_buffer[i + j]) >> 16);
        buffer_size -= buffer_size_current_buffer;
        u32_buffer += i;
        usb_stream_update_buffers(buffer_size_current_buffer/nb_channels);
      }
    }
    buffer = (void *) u32_buffer;
  }
  else
  {
    // Other bit rates are not supported
    usb_stream_context->error = USB_STREAM_ERROR_UNSUPPORTED;
  }

  if (buffer_size)
    usb_stream_context->error = USB_STREAM_ERROR_OVERFLOW;
}

//!
//! @brief This function takes the stream coming from the selected USB pipe and sends
//! it to the DAC driver. Moreover, it ensures that both input and output stream
//! keep synchronized by adding or deleting samples.
//!
//! @param side          USB_STREAM_HOST for USB host, USB_STREAM_DEVICE for device.
//! @param pipe_in       Number of the addressed pipe
//! @param pFifoCount    (return parameter) NULL or pointer to the number of used buffers at this time
//!
//! @return              status: (USB_STREAM_STATUS_OK, USB_STREAM_STATUS_NOT_SYNCHRONIZED,
//!                      USB_STREAM_STATUS_SPEED_UP, USB_STREAM_STATUS_SLOW_DOWN, USB_STREAM_STATUS_BUFFER_OVERFLOW)
//!
int usb_stream_input(usb_stream_side_t side, uint8_t pipe_in, uint32_t* pFifoCount)
{
  uint16_t      byte_count=0;

  // We comes here since we have received something. Let's increase the internal
  // activity counter.
  usb_stream_cnt++;

#if USB_HOST_FEATURE == true
  if( side==USB_STREAM_HOST )
  {
     byte_count=Host_byte_count(pipe_in);
  }
#endif
#if USB_DEVICE_FEATURE == true
  if( side==USB_STREAM_DEVICE )
  {
     byte_count=Usb_byte_count(pipe_in);
  }
#endif
  if( byte_count==0 )
  {
     if( cpu_is_timeout(&broken_stream_timer) ) {
        usb_stream_context->error = USB_STREAM_ERROR_BROKEN_STREAM;
     } else {
        usb_stream_context->error = USB_STREAM_ERROR_NO_DATA;
     }
     return usb_stream_context->status;
  }
  else
  {
    // reset time out detection
    cpu_set_timeout(cpu_ms_2_cy(BROKEN_STREAM_TIMER, FCPU_HZ), &broken_stream_timer);
  }

  // Make sure the structure is initialized
  if (usb_stream_context->status == USB_STREAM_STATUS_NOT_INITIALIZED)
    return USB_STREAM_ERROR_NONE;

  // Set status to data acquiring
  if (usb_stream_context->status & USB_STREAM_STATUS_DATA_ACQUIRING)
    return USB_STREAM_ERROR_NOT_SYNCHRONIZED;
  usb_stream_context->status |= USB_STREAM_STATUS_DATA_ACQUIRING;

#if USB_HOST_FEATURE == true
  if( side==USB_STREAM_HOST )
  {
     Host_reset_pipe_fifo_access(pipe_in);
     host_read_p_rxpacket(pipe_in, usb_stream_context->input_temp_buffer, Min(byte_count, sizeof(usb_stream_context->input_temp_buffer)), NULL);
  }
#endif
#if USB_DEVICE_FEATURE == true
  if( side==USB_STREAM_DEVICE )
  {
     Usb_reset_endpoint_fifo_access(pipe_in);
     usb_read_ep_rxpacket(pipe_in, usb_stream_context->input_temp_buffer, Min(byte_count, sizeof(usb_stream_context->input_temp_buffer)), NULL);
  }
#endif

  // The Mono is not well supported in the current version. We prefer here to force the Stereo mode
  // and to duplicate the sample at the source.
  if( usb_stream_context->duplicate_channels )
  {
    int16_t *s16_sample_buffer = (int16_t*)usb_stream_context->input_temp_buffer;
    int i;

    for (i = byte_count/sizeof(int16_t) - 1; i >= 0; i--)
    {
      s16_sample_buffer[2 * i + 1] =
      s16_sample_buffer[2 * i]     = s16_sample_buffer[i];
    }
    byte_count*=2;
  }

  // Fill the input buffer
  usb_stream_fill_input_buffers(usb_stream_context->input_temp_buffer,
                                byte_count,
                                usb_stream_context->nb_channels,
                                usb_stream_context->bits_per_sample);

  *pFifoCount = usb_stream_count_nb_buffer(BUFFER_STATE_FULL);

  // Reset status
  usb_stream_context->status &= ~USB_STREAM_STATUS_DATA_ACQUIRING;

  return USB_STREAM_ERROR_NONE;
}


bool usb_stream_task(void)
{
  int nb_full_buffers;
  bool remaining_data_to_process = false;
  // Defines the buffer limits to change the resampling methods.
  const int buffer_limit_stable = AUDIO_STREAM_NB_INPUT_BUFFERS / 2;
  const int buffer_limit_overrun = 1;
  const int buffer_limit_underrun = AUDIO_STREAM_NB_INPUT_BUFFERS - 1;

  // Make sure the structure is initialized
  if (usb_stream_context->status == USB_STREAM_STATUS_NOT_INITIALIZED)
    return false;

  // Set status to data processing
  if (usb_stream_context->status & USB_STREAM_STATUS_DATA_PROCESSING)
    return false;
  usb_stream_context->status |= USB_STREAM_STATUS_DATA_PROCESSING;

  nb_full_buffers = usb_stream_count_nb_buffer(BUFFER_STATE_FULL);

  // Check for the most appropriate re-sampling setup to use
  if (usb_stream_context->synchronized)
  {
    if (nb_full_buffers == 0)
    {
      usb_stream_context->error = USB_STREAM_ERROR_OVERFLOW;
      usb_stream_context->synchronized = false;
    }
    else if (nb_full_buffers == buffer_limit_stable)
    {
      if (usb_stream_context->ctx != &usb_stream_context->ctx_original)
      {
        dsp16_resampling_link(usb_stream_context->ctx_original.resampling, usb_stream_context->ctx->resampling);
        usb_stream_context->ctx = &usb_stream_context->ctx_original;
      }
    }
    else if (nb_full_buffers >= buffer_limit_underrun)
    {
      if (usb_stream_context->ctx != &usb_stream_context->ctx_underrun)
      {
        dsp16_resampling_link(usb_stream_context->ctx_underrun.resampling, usb_stream_context->ctx->resampling);
        usb_stream_context->ctx = &usb_stream_context->ctx_underrun;
      }
    }
    else if (nb_full_buffers <= buffer_limit_overrun)
    {
      if (usb_stream_context->ctx != &usb_stream_context->ctx_overrun)
      {
        dsp16_resampling_link(usb_stream_context->ctx_overrun.resampling, usb_stream_context->ctx->resampling);
        usb_stream_context->ctx = &usb_stream_context->ctx_overrun;
      }
    }
    // Critical under-run condition (no buffer left)
    else if (nb_full_buffers == AUDIO_STREAM_NB_INPUT_BUFFERS)
    {
      usb_stream_context->error = USB_STREAM_ERROR_UNDERFLOW;
      usb_stream_context->synchronized = false;
    }
  }

/*
  // Used for testing
#warning remove the LEDs
#if defined(LED0) && defined(LED1) && defined(LED2)
  LED_Off(LED0);
  LED_Off(LED1);
  LED_Off(LED2);
  if (usb_stream_context->ctx->resampling == usb_stream_context->ctx_overrun.resampling)
    LED_On(LED0);
  if (usb_stream_context->ctx->resampling == usb_stream_context->ctx_original.resampling)
    LED_On(LED1);
  if (usb_stream_context->ctx->resampling == usb_stream_context->ctx_underrun.resampling)
    LED_On(LED2);
#endif
*/

  // If a buffer is ready to be sent to the DAC
  if (usb_stream_context->synchronized && nb_full_buffers)
    remaining_data_to_process = usb_stream_process();

  // Audio stream (re-)synchronization process
  if (!usb_stream_context->synchronized)
  {
    if (nb_full_buffers >= buffer_limit_stable + 2)
    {
      usb_stream_process();
      usb_stream_process();
      usb_stream_context->synchronized = true;
    }
  }

#if defined(USB_STREAM_OVERRUN_CALLBACK)
  if (usb_stream_context->error == USB_STREAM_ERROR_OVERFLOW)
  {
    USB_STREAM_OVERRUN_CALLBACK();
    usb_stream_context->error = USB_STREAM_ERROR_NONE;
  }
#endif
#if defined(USB_STREAM_UNDERRUN_CALLBACK)
  if (usb_stream_context->error == USB_STREAM_ERROR_UNDERFLOW)
  {
    USB_STREAM_UNDERRUN_CALLBACK();
    usb_stream_context->error = USB_STREAM_ERROR_NONE;
  }
#endif

  // Reset status
  usb_stream_context->status &= ~USB_STREAM_STATUS_DATA_PROCESSING;

  return remaining_data_to_process;
}

bool usb_stream_new_sample_rate(uint32_t sample_rate)
{
  audio_mixer_dacs_flush_direct(false);

  usb_stream_init(sample_rate,
                  2,
                  16,
                  false);

  return true;
}

void player_shutdown(void)
{
  int i, j;

  audio_mixer_dacs_flush_direct(false);

  if (usb_stream_context)
  {
    // Free the resampling structure
    usb_stream_close();
    // Free memory for input buffers
    for(i=0; i<AUDIO_STREAM_NB_INPUT_BUFFERS; i++)
    {
      for(j=0; j<AUDIO_STREAM_MAX_NB_CHANNELS; j++)
      {
        if (usb_stream_context->input_buffers[i]->buffer[j])
          free(usb_stream_context->input_buffers[i]->buffer[j]);
      }
      if (usb_stream_context->input_buffers[i])
        free(usb_stream_context->input_buffers[i]);
    }

    // Free the main structure
    free(usb_stream_context);
    usb_stream_context = NULL;
  }
}


//!
//! @brief This function ensures that no underflow/underflow will never occur
//! by adjusting the SSC/ABDAC frequencies.
void usb_stream_resync(void)
{
    // Process all remaining data
    while (usb_stream_task());
}

//!
//! @brief This function returns the PPM differences between the input audio stream frequency
//! and the DAC output frequency.
//! Note that this information should only be considered as an indication, for debug or visual effect for example.
//!
int32_t usb_stream_ppm_get( void )
{
  return 0;
}


//!
//! @brief This function returns the number of full buffers.
//!
uint32_t usb_stream_get_buffer_level()
{
  return usb_stream_count_nb_buffer(BUFFER_STATE_FULL);
}


//!
//! @brief This function returns the internal audio counter.
//!
uint16_t usb_stream_get_counter()
{
  return usb_stream_cnt;
}


//!
//! @brief This function returns the audio status.
//!
uint16_t usb_stream_get_stream_status()
{
  return usb_stream_context->error;
}
