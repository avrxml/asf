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
#include "cycle_counter.h"
#include "gpio.h"
#include "debug.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

t_cpu_time usb_stream_resync_timer;
t_cpu_time usb_stream_monitor_timer;
static uint16_t usb_stream_resync_last_room=0;
static uint32_t usb_stream_real_sample_freq=0;
static int16_t  usb_stream_resync_ppm=0;
static bool usb_stream_resync_b_acting=false;
static t_cpu_time broken_stream_timer;
#define BROKEN_STREAM_TIMER     4000 // unit is in ms.


//_____ D E C L A R A T I O N S ____________________________________________

usb_stream_context_t *usb_stream_context = NULL;
volatile uint16_t usb_stream_cnt=0;


void player_init(void)
{
  usb_stream_context = calloc(1, sizeof(*usb_stream_context));
}


//!
//! @brief This callback function is called when the DAC interrupt has sent
//! the buffer 'n-1' and switches to buffer 'n'. The aim of this function is thus to prepare
//! the buffer 'n+1'; so that there is always a pending buffer for the interrupt.
//!
void dac_sample_sent_cb( void )
{
   uint16_t fifo_used_cnt=usb_stream_fifo_get_used_room();

   if( fifo_used_cnt==0 )
   {
      // Unexpected interrupt.
      usb_stream_context->synchronized = false;
      usb_stream_context->status = USB_STREAM_ERROR_NOT_SYNCHRONIZED;
      return;
   }

   // Previous buffer has been sent by the DAC driver.
   usb_stream_fifo_pull();
   fifo_used_cnt--;

   if( fifo_used_cnt!=0 )
   {
      void* buffer;
      uint16_t   size;

      usb_stream_fifo_get(&buffer, &size);
      audio_mixer_dacs_output_direct(buffer, size/(usb_stream_context->channel_count*usb_stream_context->bits_per_sample/8));
   }
   else
   {
      usb_stream_context->synchronized = false;
      usb_stream_context->status = USB_STREAM_ERROR_NOT_SYNCHRONIZED;
   }
}


//!
//! @brief This callback function is called when the PWM DAC interrupt does not have
//! any more audio samples (i.e. "famine").
//!
void dac_underflow_cb( void )
{
   usb_stream_context->synchronized = false;
   usb_stream_context->status = USB_STREAM_ERROR_NOT_SYNCHRONIZED;
}


extern uint32_t buf_trace[];

//!
//! @brief This function initializes the USB Stream driver.
//!
void usb_stream_init(
   uint32_t sample_rate_hz
,  uint8_t num_channels
,  uint8_t bits_per_sample
,  bool swap_channels)
{
   uint32_t i;

   usb_stream_resync_b_acting = false;
   usb_stream_context->sample_rate = sample_rate_hz;
   usb_stream_context->rd_id=
   usb_stream_context->wr_id= 0;
   usb_stream_context->synchronized = false;
   usb_stream_context->bits_per_sample = bits_per_sample;
   usb_stream_context->channel_count = num_channels;
   cpu_stop_timeout(&usb_stream_resync_timer);
   cpu_stop_timeout(&usb_stream_monitor_timer);

   for( i=0 ; i<USB_STREAM_BUFFER_NUMBER ; i++ )
      usb_stream_context->audio_buffer_size[i] = 0;

   audio_mixer_dacs_setup_direct(sample_rate_hz,
                          num_channels,
                          bits_per_sample,
                          swap_channels);

  // Start the broken stream timer
  cpu_set_timeout(cpu_ms_2_cy(BROKEN_STREAM_TIMER, FCPU_HZ), &broken_stream_timer);
}


//!
//! @brief This function takes the stream coming from the selected USB pipe and sends
//! it to the DAC driver. Moreover, it ensures that both input and output stream
//! keep synchronized by adding or deleting samples.
//!
//! @param side          USB_STREAM_HOST for USB host, USB_STREAM_DEVICE for device.
//! @param pipe_in       Number of the addressed pipe/endpoint
//! @param pFifoCount    (return parameter) NULL or pointer to the number of used buffers at this time
//!
//! @return              status: (USB_STREAM_STATUS_OK, USB_STREAM_STATUS_NOT_SYNCHRONIZED,
//!                      USB_STREAM_STATUS_SPEED_UP, USB_STREAM_STATUS_SLOW_DOWN, USB_STREAM_STATUS_BUFFER_OVERFLOW)
//!
int usb_stream_input(usb_stream_side_t side, uint8_t pipe_in, uint32_t* pFifoCount)
{
   uint16_t fifo_used_cnt;
   uint16_t byte_count=0;
   uint32_t i;
   UnionPtr pswap;
   UnionPtr buffer;

   // We comes here since we have received something. Let's increase the internal
   // activity counter.
   usb_stream_cnt++;

   fifo_used_cnt=usb_stream_fifo_get_used_room();
   if (pFifoCount)
      *pFifoCount = fifo_used_cnt;

   // usb_stream_fifo_get_free_room()
   if( USB_STREAM_BUFFER_NUMBER-fifo_used_cnt==0 )
   {  // Fatal error: even with the synchro mechanism acting, we are in a case in which the
      // buffers are full.
      usb_stream_context->synchronized = false;
      usb_stream_context->status = USB_STREAM_ERROR_NOT_SYNCHRONIZED;
      return usb_stream_context->status;
   }

   pswap.s8ptr  =
   buffer.s8ptr = usb_stream_fifo_get_buffer(usb_stream_context->wr_id);

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
        usb_stream_context->status = USB_STREAM_ERROR_BROKEN_STREAM;
     } else {
        usb_stream_context->status = USB_STREAM_ERROR_NO_DATA;
     }
     return usb_stream_context->status;
  }
  else
  {
    // reset time out detection
    cpu_set_timeout(cpu_ms_2_cy(BROKEN_STREAM_TIMER, FCPU_HZ), &broken_stream_timer);
  }

#if USB_HOST_FEATURE == true
   if( side==USB_STREAM_HOST )
   {
      Host_reset_pipe_fifo_access(pipe_in);
      host_read_p_rxpacket(pipe_in, (void*)buffer.s8ptr, byte_count, NULL);
   }
#endif
#if USB_DEVICE_FEATURE == true
   if( side==USB_STREAM_DEVICE )
   {
      Usb_reset_endpoint_fifo_access(pipe_in);
      usb_read_ep_rxpacket(pipe_in, (void*)buffer.s8ptr, byte_count, NULL);
   }
#endif

   usb_stream_context->status = USB_STREAM_ERROR_NONE;

   if( byte_count > USB_STREAM_REAL_BUFFER_SIZE )
   {
      byte_count = USB_STREAM_REAL_BUFFER_SIZE;
      usb_stream_context->status = USB_STREAM_ERROR_OVERFLOW;
   }

   // Swap samples since they are coming from the USB world.
   if( usb_stream_context->bits_per_sample==16 )
      for( i=0 ; i<byte_count/(16/8) ; i++ )
         pswap.s16ptr[i] = swap16(pswap.s16ptr[i]);

   else if( usb_stream_context->bits_per_sample==32 )
      for( i=0 ; i<byte_count/(32/8) ; i++ )
         pswap.s32ptr[i] = swap32(pswap.s32ptr[i]);

   //for( i=0 ; i<byte_count/2 ; i++ )
   //   printf("0x%04hx ", pswap[i]);
   //printf("\r\n");

   // Points on the sample after the last one.
   i = byte_count/(usb_stream_context->bits_per_sample/8);

   // Add/Remove samples is done whatever the buffering position is.
   if( usb_stream_resync_b_acting )
   {
     if( cpu_is_timer_stopped(&usb_stream_resync_timer) )
     { // Launch the timer.
       uint32_t freq_diff;
       freq_diff= abs((int32_t)usb_stream_real_sample_freq - (int32_t)usb_stream_context->sample_rate);
       cpu_set_timeout( cpu_us_2_cy(1000000/freq_diff, FCPU_HZ), &usb_stream_resync_timer );
     }
     else if( cpu_is_timeout(&usb_stream_resync_timer) )
     { // Reload the timer.
       uint32_t freq_diff;
       freq_diff= abs((int32_t)usb_stream_real_sample_freq - (int32_t)usb_stream_context->sample_rate);
       cpu_set_timeout( cpu_us_2_cy(1000000/freq_diff, FCPU_HZ), &usb_stream_resync_timer );

       // Here start the USB stream resynchronization.
       if( usb_stream_resync_ppm>0 )
       { // Remove a sample
         if( usb_stream_context->bits_per_sample==16 )
         {
           int32_t sum;
           // merge samples i-6 with i-4 (R) and i-5 with i-3 (L).
           sum  = buffer.s16ptr[i-8];
           sum += buffer.s16ptr[i-6];
           sum += buffer.s16ptr[i-4];
           sum += buffer.s16ptr[i-2];
           buffer.s16ptr[i-6] = sum/4; // Right channel.

           sum  = buffer.s16ptr[i-7];
           sum += buffer.s16ptr[i-5];
           sum += buffer.s16ptr[i-3];
           sum += buffer.s16ptr[i-1];
           buffer.s16ptr[i-5] = sum/4; // Left channel.

           // Move samples i-2 to i-4 (R) and i-1 to i-3 (L).
           buffer.s16ptr[i-4] = buffer.s16ptr[i-2]; // Right channel.
           buffer.s16ptr[i-3] = buffer.s16ptr[i-1]; // Left channel.
         }
         else if( usb_stream_context->bits_per_sample==32 )
         {
           int64_t sum;
           // merge samples i-6 with i-4 (R) and i-5 with i-3 (L).
           sum  = buffer.s32ptr[i-8];
           sum += buffer.s32ptr[i-6];
           sum += buffer.s32ptr[i-4];
           sum += buffer.s32ptr[i-2];
           buffer.s32ptr[i-6] = sum/4; // Right channel.

           sum  = buffer.s32ptr[i-7];
           sum += buffer.s32ptr[i-5];
           sum += buffer.s32ptr[i-3];
           sum += buffer.s32ptr[i-1];
           buffer.s32ptr[i-5] = sum/4; // Left channel.

           // Move samples i-2 to i-4 (R) and i-1 to i-3 (L).
           buffer.s32ptr[i-4] = buffer.s32ptr[i-2]; // Right channel.
           buffer.s32ptr[i-3] = buffer.s32ptr[i-1]; // Left channel.
         }

         byte_count -= usb_stream_context->channel_count*(usb_stream_context->bits_per_sample/8);
       }
       else if( usb_stream_resync_ppm<0 )
       { // Copy a sample
         if( usb_stream_context->bits_per_sample==16 )
         {
           int32_t sum;
           // Move last sample
           buffer.s16ptr[i  ] = buffer.s16ptr[i-2];
           buffer.s16ptr[i+1] = buffer.s16ptr[i-1];

           sum  = buffer.s16ptr[i  ];
           sum += buffer.s16ptr[i-4];
           buffer.s16ptr[i-2] = sum/2;

           sum  = buffer.s16ptr[i+1];
           sum += buffer.s16ptr[i-3];
           buffer.s16ptr[i-1] = sum/2;
         }
         else if( usb_stream_context->bits_per_sample==32 )
         {
           int64_t sum;
           // Move last sample
           buffer.s32ptr[i  ] = buffer.s32ptr[i-2];
           buffer.s32ptr[i+1] = buffer.s32ptr[i-1];

           sum  = buffer.s32ptr[i  ];
           sum += buffer.s32ptr[i-4];
           buffer.s32ptr[i-2] = sum/2;

           sum  = buffer.s32ptr[i+1];
           sum += buffer.s32ptr[i-3];
           buffer.s32ptr[i-1] = sum/2;
         }

         byte_count += usb_stream_context->channel_count*(usb_stream_context->bits_per_sample/8);
       }
     }
   }
   else
   {
     cpu_stop_timeout(&usb_stream_resync_timer);
   }

   #if (defined USB_RESYNC_AUDIO_STREAM) && (USB_RESYNC_AUDIO_STREAM==true)
   usb_stream_resync();
   #endif

   usb_stream_fifo_push(byte_count);
   fifo_used_cnt++;

   if( !usb_stream_context->synchronized )
   {
      usb_stream_context->status = USB_STREAM_ERROR_NOT_SYNCHRONIZED;

      if( fifo_used_cnt>=(USB_STREAM_BUFFER_NUMBER/2) )
      {  // We have enough buffers to start the playback.
         void* buffer;
         uint16_t   size;

         usb_stream_resync_ppm = 0;
         usb_stream_resync_b_acting = false;
         usb_stream_real_sample_freq = usb_stream_context->sample_rate;
         usb_stream_resync_last_room = fifo_used_cnt;
         cpu_set_timeout( cpu_ms_2_cy(TIMER_USB_STREAM_MONITOR, FCPU_HZ), &usb_stream_monitor_timer );

         usb_stream_context->synchronized=true;
         usb_stream_fifo_get(&buffer, &size);
         audio_mixer_dacs_output_direct(buffer, size/(usb_stream_context->channel_count*usb_stream_context->bits_per_sample/8));

         // Fill also the reload stage of the PDCA.
         usb_stream_fifo_pull();
         usb_stream_fifo_get(&buffer, &size);
         audio_mixer_dacs_output_direct(buffer, size/(usb_stream_context->channel_count*usb_stream_context->bits_per_sample/8));
      }
   }

   return usb_stream_context->status;
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
  audio_mixer_dacs_flush_direct(false);

  if (usb_stream_context)
  {
    // Free the main structure
    free(usb_stream_context);
    usb_stream_context = NULL;
  }
}


//!
//! @brief This function monitors the buffering level and set a timer for the resynchronization. This will ensures that no
//! underflow/underflow will never occur by inserting/removing samples.
void usb_stream_resync( void )
{
  uint32_t freq_diff;

  uint32_t fifo_used_cnt;

  if (!usb_stream_context->synchronized)
    return;

  if( !cpu_is_timeout(&usb_stream_monitor_timer) )
    return;

  // time-out occur. Let's check frequency deviation.
  fifo_used_cnt=usb_stream_fifo_get_used_room();

  // Frequency control
  if( fifo_used_cnt>USB_STREAM_BUFFER_NUMBER/2 )
  { // Need to increase the frequency / remove samples
    if( fifo_used_cnt >= usb_stream_resync_last_room )
    {
      usb_stream_resync_ppm += USB_STREAM_RESYNC_PPM_STEPS;
      usb_stream_real_sample_freq = PPM_ADD(usb_stream_context->sample_rate, usb_stream_resync_ppm);
      usb_stream_resync_last_room = fifo_used_cnt;
      cpu_set_timeout( cpu_ms_2_cy(TIMER_USB_STREAM_MONITOR, FCPU_HZ), &usb_stream_monitor_timer );

      freq_diff= abs((int32_t)usb_stream_real_sample_freq - (int32_t)usb_stream_context->sample_rate);

      if( freq_diff!=0 )
        usb_stream_resync_b_acting = true;
      else
        usb_stream_resync_b_acting = false;
    }
  }
  else if (fifo_used_cnt<USB_STREAM_BUFFER_NUMBER/2 )
  { // Need to slow down the frequency / copy a sample
    if( fifo_used_cnt <= usb_stream_resync_last_room )
    {
      usb_stream_resync_ppm -= USB_STREAM_RESYNC_PPM_STEPS;
      usb_stream_real_sample_freq = PPM_ADD(usb_stream_context->sample_rate, usb_stream_resync_ppm);
      usb_stream_resync_last_room = fifo_used_cnt;
      cpu_set_timeout( cpu_ms_2_cy(TIMER_USB_STREAM_MONITOR, FCPU_HZ), &usb_stream_monitor_timer );

      freq_diff= abs((int32_t)usb_stream_real_sample_freq - (int32_t)usb_stream_context->sample_rate);

      if( freq_diff!=0 )
        usb_stream_resync_b_acting = true;
      else
        usb_stream_resync_b_acting = false;
    }
  }
}


//!
//! @brief This function returns the PPM differences between the input audio stream frequency
//! and the DAC output frequency.
//! Note that this information should only be considered as an indication, for debug or visual effect for example.
//!
int32_t usb_stream_ppm_get( void )
{
  return (int32_t)usb_stream_resync_ppm;
}


//!
//! @brief This function returns the number of full buffers.
//!
uint32_t usb_stream_get_buffer_level()
{
  return usb_stream_fifo_get_used_room();
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
  return usb_stream_context->status;
}
