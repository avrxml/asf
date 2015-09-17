/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB Audio streaming player.
 *
 * This file manages the USB Audio streaming player for both the USB Device
 * or Host modes.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _USB_STREAM_PLAYER_H_
#define _USB_STREAM_PLAYER_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_audio_mixer.h"
#include "conf_audio_player.h"
#include "conf_usb.h"
#include "audio_mixer.h"


#if( USB_STREAM_BUFFER_NUMBER<2 )
#  warning USB Audio stream error: Please initialize USB_STREAM_BUFFER_NUMBER for at least 2 buffers
#endif

#if( USB_STREAM_BUFFER_NUMBER!=2 )  \
&& ( USB_STREAM_BUFFER_NUMBER!=4 )  \
&& ( USB_STREAM_BUFFER_NUMBER!=8 )  \
&& ( USB_STREAM_BUFFER_NUMBER!=16 ) \
&& ( USB_STREAM_BUFFER_NUMBER!=32 ) \
&& ( USB_STREAM_BUFFER_NUMBER!=64 ) \
&& ( USB_STREAM_BUFFER_NUMBER!=128 )\
&& ( USB_STREAM_BUFFER_NUMBER!=256 )
#  warning USB Audio stream error: Please initialize USB_STREAM_BUFFER_NUMBER to a power of 2 numbers (2, 4, 8, 16, ..)
#endif

//! Compute the PPM of a number
#define PPM(value, ppm)        ( ((int)( (long long)value*(ppm)/1000000) ) )

//! Add x PPM to a value
#define PPM_ADD(value, ppm)    ( ((int)(value + (long long)value*(ppm)/1000000)) )

//! Remove x PPM to a value
#define PPM_SUB(value, ppm)    ( ((int)(value - (long long)value*(ppm)/1000000)) )

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//! Real size of the buffer, i.e. taking into account the possible data 'expansion'
#define  USB_STREAM_REAL_BUFFER_SIZE     (USB_STREAM_BUFFER_SIZE)

//! @defgroup usb_stream_status USB stream status
//! Defines the various possible status returned by the USB Stream Control functions.
//! @{
typedef enum
  {
    USB_STREAM_ERROR_NONE = 0,
    USB_STREAM_ERROR_OVERFLOW,
    USB_STREAM_ERROR_UNDERFLOW,
    USB_STREAM_ERROR_DEL_SAMPLES,
    USB_STREAM_ERROR_ADD_SAMPLES,
    USB_STREAM_ERROR_NOT_SYNCHRONIZED,
    USB_STREAM_ERROR_RECOVERING_FROM_UNDERFLOW,
    USB_STREAM_ERROR_UNSUPPORTED,
    USB_STREAM_ERROR_BROKEN_STREAM,    //!< The USB audio stream is broken (empty packets received since a long time)
    USB_STREAM_ERROR_NO_DATA           //!< The USB received an empty packet.
  } usb_stream_error_defs_t;
//! @}

typedef enum
{
  USB_STREAM_DEVICE =0,
  USB_STREAM_HOST
} usb_stream_side_t;

#if(defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC)
#   include "dsp.h"

typedef struct
{
  int input_fs_hz;
  int output_fs_hz;
  int output_underrun_fs_hz;
  int output_overrun_fs_hz;
  int order;
  dsp16_t gain;
  dsp16_t gain_underrun;
  dsp16_t gain_overrun;
} resampling_config_t;

#   define AUDIO_STREAM_IN_OUT_FS_DEFAULT_VALUE \
    { \
      .output_fs_hz = 44100, \
      .output_underrun_fs_hz = 44000, \
      .output_overrun_fs_hz = 44200, \
      .order = 6, \
      .gain = 0, \
      .gain_overrun = DSP16_Q(0.95), \
      .gain_underrun = DSP16_Q(0.95) \
    }

#   if (defined DEFAULT_DACS)
#     if (defined AUDIO_MIXER_DAC_AIC23B) && (DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
#       if FOSC0 == 14745600 && FOSC1 == 11289600
#         define AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB \
          { \
            { \
              .input_fs_hz = 48000, \
              .output_fs_hz = 48000, \
              .output_underrun_fs_hz = 47900, \
              .output_overrun_fs_hz = 48100, \
              .order = 6, \
              .gain          = DSP16_Q(1.0), \
              .gain_overrun  = DSP16_Q(1.0), \
              .gain_underrun = DSP16_Q(1.0) \
            }, \
            { \
              .input_fs_hz = 32000, \
              .output_fs_hz = 32000, \
              .output_underrun_fs_hz = 31900, \
              .output_overrun_fs_hz = 32100, \
              .order = 6, \
              .gain          = DSP16_Q(1.0), \
              .gain_overrun  = DSP16_Q(1.0), \
              .gain_underrun = DSP16_Q(1.0) \
            } \
          }
#       else
#         define AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB \
          { \
            { \
              .input_fs_hz = 48000, \
              .output_fs_hz = 44100, \
              .output_underrun_fs_hz = 44000, \
              .output_overrun_fs_hz = 44200, \
              .order = 6, \
              .gain          = DSP16_Q(1.0), \
              .gain_overrun  = DSP16_Q(1.0), \
              .gain_underrun = DSP16_Q(1.0) \
            }, \
            { \
              .input_fs_hz = 32000, \
              .output_fs_hz = 44100, \
              .output_underrun_fs_hz = 44000, \
              .output_overrun_fs_hz = 44200, \
              .order = 6, \
              .gain          = DSP16_Q(1.0), \
              .gain_overrun  = DSP16_Q(1.0), \
              .gain_underrun = DSP16_Q(1.0) \
            } \
          }
#       endif
#     elif (defined AUDIO_MIXER_DAC_ABDAC ) && (DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC)
#       define AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB  { { 0 } }
#     else
#       define AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB  { { 0 } }
#     endif
#   else
#     define AUDIO_STREAM_IN_OUT_FS_TRANSLATION_TAB  { { 0 } }
#  endif

#   define AUDIO_STREAM_NB_INPUT_BUFFERS           8
#   define AUDIO_STREAM_NB_OUTPUT_BUFFERS          2
#   define AUDIO_STREAM_MAX_USB_PACKET_SIZE        (USB_STREAM_REAL_BUFFER_SIZE)
#   define AUDIO_STREAM_MAX_NB_CHANNELS            2
#   define AUDIO_STREAM_RESAMPLING_FILTER_ORDER    6

typedef enum
{
    BUFFER_STATE_EMPTY,
    BUFFER_STATE_FULL,
    BUFFER_STATE_FILLING,
    BUFFER_STATE_RESAMPLING
} buffer_state_t;

typedef struct
{
  buffer_state_t buffer_state;
  dsp16_t *buffer[AUDIO_STREAM_MAX_NB_CHANNELS];
} buffer_t;


#   define USB_STREAM_STATUS_NOT_INITIALIZED 0
#   define USB_STREAM_STATUS_IDLE            1
#   define USB_STREAM_STATUS_DATA_PROCESSING 2
#   define USB_STREAM_STATUS_DATA_ACQUIRING  4

typedef struct
{
  dsp_resampling_t *resampling;
  dsp16_t gain;
} resampling_context_t;

typedef struct
{
  uint8_t status;

  resampling_context_t *ctx;
  // Over/Under-run handling
  resampling_context_t ctx_original;
  resampling_context_t ctx_overrun;
  resampling_context_t ctx_underrun;

  buffer_t *input_buffers[AUDIO_STREAM_NB_INPUT_BUFFERS];
  uint8_t input_temp_buffer[AUDIO_STREAM_MAX_USB_PACKET_SIZE];
  void *output_temp_buffer;
  int current_output_buffer;
  void *output_buffers[AUDIO_STREAM_NB_OUTPUT_BUFFERS];
  int input_buffer_size;
  int nb_channels;
  int bits_per_sample;
  int swap_channels;
  int current_buffer;
  int current_buffer_index;
  int current_full_buffer;
  int fs_output;
  bool synchronized;
  bool duplicate_channels;
  char error;
} usb_stream_context_t;

extern usb_stream_context_t *usb_stream_context;

#else

typedef struct
{
  //! The USB stream audio FIFO is composed of buffers.
  volatile int8_t   audio_buffer[     USB_STREAM_BUFFER_NUMBER][USB_STREAM_REAL_BUFFER_SIZE];

  //! Holds the data size of the audio packets stored into the buffers.
  volatile uint16_t  audio_buffer_size[USB_STREAM_BUFFER_NUMBER];

  //! Reader buffer id.
  volatile uint8_t   rd_id;

  //! Writer buffer id.
  volatile uint8_t   wr_id;

  //! Boolean telling if synchro is acting or not.
  volatile bool synchronized;

  //! Driver status.
  volatile int  status;

  uint8_t   bits_per_sample;
  uint8_t   channel_count;
  uint32_t  sample_rate;
} usb_stream_context_t;

extern usb_stream_context_t *usb_stream_context;


/*! \brief This function returns the number of used buffers in the USB stream FIFO.
 *
 * \return \c number of used buffers.
 */
__always_inline static int usb_stream_fifo_get_used_room( void )
{
  return (usb_stream_context->wr_id + 2 * USB_STREAM_BUFFER_NUMBER -
          usb_stream_context->rd_id) % (2 * USB_STREAM_BUFFER_NUMBER) +
         !audio_mixer_dacs_output_direct(NULL, 0); // Avoid overwriting the loaded PDCA
                                            // buffer since buffers are pulled
                                            // from the FIFO on PDCA load and
                                            // not on unload.
}


/*! \brief This function returns the number of free buffers in the USB stream FIFO.
 *
 * \return \c number of free buffers.
 */
__always_inline static int usb_stream_fifo_get_free_room( void )
{
  return USB_STREAM_BUFFER_NUMBER - usb_stream_fifo_get_used_room();
}


/*! \brief This function returns the pointer on a buffer according to its index
 *
 * \param index      Index of the buffer
 *
 * \return \c pointer on buffer
 */
__always_inline static int8_t* usb_stream_fifo_get_buffer( uint8_t index )
{
  return (int8_t*)usb_stream_context->audio_buffer[index % USB_STREAM_BUFFER_NUMBER];
}


/*! \brief This function put into the audio stream FIFO a new buffer
 * and increases the write index of the FIFO.
 *
 * \param size       Size of the buffer.
 */
__always_inline static void usb_stream_fifo_push( uint16_t size )
{
  // Prepare current buffer. Buffer content has been filled before.
  usb_stream_context->audio_buffer_size[usb_stream_context->wr_id % USB_STREAM_BUFFER_NUMBER] = size;

  // Validate current buffer which is ready.
  usb_stream_context->wr_id = (usb_stream_context->wr_id + 1) % (2 * USB_STREAM_BUFFER_NUMBER);
}

/*! \brief This function gets from the audio stream FIFO the next buffer to read.
 *
 *  Note that it does not increase the read index of the FIFO.
 *
 * \param pp_buffer  (return parameter) Pointer to the buffer address to update.
 * \param p_size     (return parameter) Pointer to the size of the buffer.
 */
__always_inline static void usb_stream_fifo_get( void** pp_buffer, uint16_t* p_size )
{
  // Gives current buffer to PWM DAC driver.
  uint8_t audio_buffer_rd_id = usb_stream_context->rd_id % USB_STREAM_BUFFER_NUMBER;
  *pp_buffer = (void*)usb_stream_context->audio_buffer[audio_buffer_rd_id];
  *p_size    = usb_stream_context->audio_buffer_size[  audio_buffer_rd_id];
  usb_stream_context->audio_buffer_size[audio_buffer_rd_id] = 0;
}

/*! \brief This function increases the read index of the FIFO. */
__always_inline static void usb_stream_fifo_pull( void )
{
  usb_stream_context->rd_id = (usb_stream_context->rd_id + 1) % (2 * USB_STREAM_BUFFER_NUMBER);
}
#endif


//_____ D E C L A R A T I O N S ____________________________________________

#if(defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC)
extern uint8_t usb_stream_get_status(void);
extern bool usb_stream_task(void);
extern void usb_stream_close(void);
extern void usb_stream_out_of_samples(void);
#endif
extern void player_init(void);
extern void player_shutdown(void);

extern void usb_stream_init(uint32_t sample_rate_hz, uint8_t num_channels, uint8_t bits_per_sample, bool swap_channels);

extern int usb_stream_input(usb_stream_side_t side, uint8_t pipe_in, uint32_t* pFifoCount);
extern uint32_t usb_stream_get_buffer_level(void);
extern uint16_t usb_stream_get_counter(void);
extern uint16_t usb_stream_get_stream_status(void);

extern void usb_stream_resync( void );

extern int32_t usb_stream_ppm_get( void );


extern void dac_sample_sent_cb( void );
extern void dac_underflow_cb( void );
extern void adc_sample_sent_cb( void );
extern void adc_underflow_cb( void );

/*! \brief Sync the sample rate of the audio output stream with the USB stream.
 *
 * \retval false The new sample rate already matches the current settings.
 * \retval true The output sample rate was changed.
 */
extern bool usb_stream_new_sample_rate(uint32_t sample_rate);



#endif  // _USB_STREAM_PLAYER_H_
