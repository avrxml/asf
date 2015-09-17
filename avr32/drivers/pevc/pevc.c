/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC software driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PEVC on AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "pevc.h"


/******************************************************************************/
/**                                                                          **/
/*! \name Channels configuration API                                         **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
bool pevc_channel_configure( volatile avr32_pevc_t *pevc,
                             unsigned short int chan_id,
                             unsigned short int gen_id,
                             const pevc_evs_opt_t *pevs )
{
  if( NULL != pevc )
  {
    if(( gen_id < PEVC_NUMBER_OF_EVENT_GENERATORS )
    && ( chan_id < PEVC_NUMBER_OF_EVENT_USERS ))
    {
      // Connect the generator gen_id to the channel.
      pevc->CHMX[chan_id].evmx = gen_id;

      // Configure the event shaper for the channel.
      if( NULL != pevs )
      {
        pevc->igfdr = pevs->igfdr;  // Only one divider for all EVS channels.
        pevc->EVS[gen_id].igf = pevs->igf;
        pevc->EVS[gen_id].evf = pevs->evf;
        pevc->EVS[gen_id].evr = pevs->evr;
      }
      return( PASS );
    }
    else return( FAIL );
  }
  else
    return( FAIL );
}


// See comment header in pevc.h
void pevc_igfd_set( volatile avr32_pevc_t *pevc, unsigned char igfd )
{
  Assert( NULL != pevc );
  PEVC_IGFD_SET(pevc, igfd);
}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels Enable/Disable API                                        **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
void pevc_channels_enable( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask )
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_ENABLE(pevc, chan_mask);
}

// See comment header in pevc.h
void pevc_channels_disable( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask )
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_DISABLE(pevc, chan_mask);
}

// See comment header in pevc.h
bool pevc_get_channel_status( volatile avr32_pevc_t *pevc, unsigned short int chan_id )
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->chsr0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->chsr1) & (1 << (chan_id-32)))>>(chan_id-32) );
}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels state (Idle/Busy) API                                     **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
bool pevc_get_channel_state(volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->busy0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->busy1) & (1 << (chan_id-32)))>>(chan_id-32) );
}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels Software Event API                                        **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
void pevc_channel_sev_enable( volatile avr32_pevc_t *pevc, unsigned short int chan_id )
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  pevc->CHMX[chan_id].smx = ENABLE;
}

// See comment header in pevc.h
void pevc_channels_trigger_sev(volatile avr32_pevc_t *pevc, unsigned long long int chan_mask)
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_TRIGGER_SEV(pevc, chan_mask);
}

// See comment header in pevc.h
void pevc_channel_sev_disable( volatile avr32_pevc_t *pevc, unsigned short int chan_id )
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  pevc->CHMX[chan_id].smx = DISABLE;
}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels trigger interrupt API                                     **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
void pevc_channels_enable_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask)
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_ENABLE_TRIGGER_INTERRUPT(pevc, chan_mask);
}

// See comment header in pevc.h
void pevc_channels_disable_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask)
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_DISABLE_TRIGGER_INTERRUPT(pevc, chan_mask);
}

// See comment header in pevc.h
bool pevc_channel_is_trigger_interrupt_enabled( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->trimr0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->trimr1) & (1 << (chan_id-32)))>>(chan_id-32) );
}

// See comment header in pevc.h
bool pevc_channel_is_trigger_interrupt_raised( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->trsr0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->trsr1) & (1 << (chan_id-32)))>>(chan_id-32) );
}

// See comment header in pevc.h
void pevc_channel_clear_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    pevc->trscr0 = (1 << chan_id);
  else
    pevc->trscr1 = (1 << (chan_id-32));
}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels overrun interrupt API                                     **/
/**                                                                          **/
/******************************************************************************/

// See comment header in pevc.h
void pevc_channels_enable_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask)
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_ENABLE_OVERRUN_INTERRUPT(pevc, chan_mask);
}

// See comment header in pevc.h
void pevc_channels_disable_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask)
{
  Assert( NULL != pevc );
  PEVC_CHANNELS_DISABLE_OVERRUN_INTERRUPT(pevc, chan_mask);
}

// See comment header in pevc.h
bool pevc_channel_is_overrun_interrupt_enabled( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->ovimr0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->ovimr1) & (1 << (chan_id-32)))>>(chan_id-32) );
}

// See comment header in pevc.h
bool pevc_channel_is_overrun_interrupt_raised( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    return( ((pevc->ovsr0) & (1 << chan_id))>>chan_id );
  else
    return( ((pevc->ovsr1) & (1 << (chan_id-32)))>>(chan_id-32) );
}

// See comment header in pevc.h
void pevc_channel_clear_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned short int chan_id)
{
  Assert( NULL != pevc );
  Assert( chan_id < PEVC_NUMBER_OF_EVENT_USERS );
  if(chan_id<32)
    pevc->ovscr0 = (1 << chan_id);
  else
    pevc->ovscr1 = (1 << (chan_id-32));
}
