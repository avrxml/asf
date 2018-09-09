/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC software driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PEVC on AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
