/*****************************************************************************
 *
 * \file
 *
 * \brief PEVC software driver header for AVR32 UC3.
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


#ifndef _PEVC_H_
#define _PEVC_H_

/**
 * \defgroup group_avr32_drivers_pevc Peripheral Event Controller (PEVC) Driver
 *
 * The Peripheral EVent Controller (PEVC) offers a set of features that allows peripherals to interact
 * without intervention from the CPU. The PEVC connects Event Generators to Event Users.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"

#include "uc3c_pevc_ids.h"

/*! \name Number of Event Generator and number of Event users
 */
//! @{
//! Number of event generators
#define PEVC_NUMBER_OF_EVENT_GENERATORS   AVR32_PEVC_EVIN_BITS
//! Number of event users
#define PEVC_NUMBER_OF_EVENT_USERS        AVR32_PEVC_TRIGOUT_BITS

//! @}


/*! \name Channel state (Idle or Busy)
 */
//! @{
#define PEVC_CHANNEL_IDLE     0
#define PEVC_CHANNEL_BUSY     1
//! @}


/*! \name Event Shaper configuration values
 */
//! @{
#define PEVC_EVS_IGF_ON                                  true
#define PEVC_EVS_IGF_OFF                                 false
#define PEVC_EVS_EVF_ON                                  true
#define PEVC_EVS_EVF_OFF                                 false
#define PEVC_EVS_EVR_ON                                  true
#define PEVC_EVS_EVR_OFF                                 false
//! @}

//! Parameters of a channel Event Shaper (for channels with asynchronous and external inputs).
typedef struct
{
  //! Input Glitch Filter Divider, range[0,0xF]
  unsigned char igfdr;

  //! Input Glitch filter on/off:\n
  //!   - \ref PEVC_EVS_IGF_ON;\n
  //!   - \ref PEVC_EVS_IGF_OFF.
  unsigned int  igf;

  //! Falling edge on event input raising trigger output on/off:\n
  //!   - \ref PEVC_EVS_EVF_ON;\n
  //!   - \ref PEVC_EVS_EVF_OFF.
  unsigned int  evf;

  //! Rising edge on event input raising trigger output on/off:\n
  //!   - \ref PEVC_EVS_EVR_ON;\n
  //!   - \ref PEVC_EVS_EVR_OFF.
  unsigned int  evr;
} pevc_evs_opt_t;

/******************************************************************************/
/**                                                                          **/
/*! \name Channels configuration API                                         **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*! \brief Configure a channel
 *
 * \param  *pevc     Base address of the PEVC
 * \param  chan_id   channel to configure
 * \param  gen_id    Event generator to connect to the channel
 * \param  *pevs     Parameters for the channel Event Shaper (valid only for channels
 *                   with asynchronous and external inputs).
 *
 * \note pevs should be NULL if the event generator is not an asynchronous or external event.
 *
 * \return bool      PASS if the channel was successfully configured
 *                   FAIL if the channel configuration failed due to incorrect parameters
 */
extern bool pevc_channel_configure( volatile avr32_pevc_t *pevc, unsigned short int chan_id,
                                    unsigned short int gen_id, const pevc_evs_opt_t *pevs );


/*! \brief Set a value for the Input Glitch Filter Divider (used by the Event Shaper).
 *
 * \param  *pevc     Base address of the PEVC
 * \param  igfd      Input Glitch Filter divider [0,15]
 *
 * \note as stated in the datasheet, there is one divider value for all EVS instance.
 *
 */
extern void pevc_igfd_set( volatile avr32_pevc_t *pevc, unsigned char igfd );
// The macro equivalent without assert on pevc:
#define PEVC_IGFD_SET(pevc, igfd) \
  (pevc)->igfdr = ((igfd)<<AVR32_PEVC_IGFDR_IGFDR_OFFSET) & AVR32_PEVC_IGFDR_IGFDR_MASK;
//! @}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels Enable/Disable API                                        **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*! \brief Enable one or more channels
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_enable(volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_ENABLE(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->cher0 = (chan_mask); \
  else  (pevc)->cher1 = (chan_mask);

/*! \brief Disable one or more channels
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_disable(volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_DISABLE(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->chdr0 = (chan_mask); \
  else  (pevc)->chdr1 = (chan_mask);


/*! \brief Get the status of a channel
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      true if channel is enabled
 *                   false if channel is disabled
 */
extern bool pevc_get_channel_status(volatile avr32_pevc_t *pevc, unsigned short int chan_id);
//! @}

/******************************************************************************/
/**                                                                          **/
/*! \name Channels state (Idle/Busy) API                                     **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*! \brief Get the state of a channel (idle or busy)
 *
 *  \note the channel must have previously been enabled.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      PEVC_CHANNEL_BUSY if the channel is busy (i.e. it is not ready to accept an event)
 *                   PEVC_CHANNEL_IDLE if the channel is idle
 */
extern bool pevc_get_channel_state(volatile avr32_pevc_t *pevc, unsigned short int chan_id);
//! @}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels Software Event API                                        **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*! \brief Enable the Software Event as the event source for a channel.
 *
 * \note When the sev is enabled, the event generator is ignored.
 *
 * \param  *pevc     Base address of the PEVC
 * \param  chan_id   channel to configure
 */
extern void pevc_channel_sev_enable( volatile avr32_pevc_t *pevc, unsigned short int chan_id );
// The macro equivalent without assert on pevc and chan_id:
#define PEVC_CHANNEL_SEV_ENABLE(pevc, chan_id)  ((pevc)->CHMX[(chan_id)].smx = ENABLE)

/*! \brief Trigger a software event to one or more channels
 *
 *  \note the channel must have previously been enabled.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_trigger_sev(volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_TRIGGER_SEV(pevc, chan_mask)  \
  if((chan_mask)<=0x80000000)  (pevc)->sev0 = (chan_mask); \
  else  (pevc)->sev1 = (chan_mask);

/*! \brief Disable the Software Event as the event source for a channel.
 *
 * \note When the sev is disabled, the event generator is the input event for the channel.
 *
 * \param  *pevc     Base address of the PEVC
 * \param  chan_id   channel to configure
 */
extern void pevc_channel_sev_disable( volatile avr32_pevc_t *pevc, unsigned short int chan_id );
// The macro equivalent without assert on pevc and chan_id:
#define PEVC_CHANNEL_SEV_DISABLE(pevc, chan_id) ((pevc)->CHMX[(chan_id)].smx = DISABLE)
//! @}


/******************************************************************************/
/**                                                                          **/
/*! \name Channels trigger interrupt API                                     **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*!
 * \brief Enable the event trigger interrupt of one or more channels.
 *
 * \note A trigger interrupt is raised when an event trigger is forwarded to a
 *  channel user (if the channel state is ready).
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_enable_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_ENABLE_TRIGGER_INTERRUPT(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->trier0 = (chan_mask); \
  else  (pevc)->trier1 = (chan_mask);

/*!
 * \brief Disable the event trigger interrupt of one or more channels.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_disable_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_DISABLE_TRIGGER_INTERRUPT(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->tridr0 = (chan_mask); \
  else  (pevc)->tridr1 = (chan_mask);

/*!
 * \brief Check if the event trigger interrupt for a given channel is enabled.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      true if the channel trigger interrupt is enabled
 *                   false if the channel trigger interrupt is disabled
 */
extern bool pevc_channel_is_trigger_interrupt_enabled( volatile avr32_pevc_t *pevc, unsigned short int chan_id);

/*!
 * \brief Check if the event trigger interrupt was raised for a given channel.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      true if the channel trigger interrupt was raised
 *                   false otherwise
 */
extern bool pevc_channel_is_trigger_interrupt_raised( volatile avr32_pevc_t *pevc, unsigned short int chan_id);

/*!
 * \brief Clear the event trigger interrupt for a given channel.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 */
extern void pevc_channel_clear_trigger_interrupt( volatile avr32_pevc_t *pevc, unsigned short int chan_id);

//! @}

/******************************************************************************/
/**                                                                          **/
/*! \name Channels overrun interrupt API                                     **/
/**                                                                          **/
/******************************************************************************/
//! @{

/*!
 * \brief Enable the event overrun interrupt of one or more channels.
 *
 * \note An overrun interrupt is raised when an event could not be forwarded to
 *  a channel user (if the channel state is busy or if the previous event is still
 *  being processed).
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_enable_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_ENABLE_OVERRUN_INTERRUPT(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->ovier0 = (chan_mask); \
  else  (pevc)->ovier1 = (chan_mask);

/*!
 * \brief Disable the event overrun interrupt of one or more channels.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_mask   bitmask of channel masks to enable (1 bit per channel, up
 *  to 64 channels, actual number of channels depending on the part, chan_mask being (1 << chan_id))
 */
extern void pevc_channels_disable_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned long long int chan_mask);
// The macro equivalent without assert on pevc:
#define PEVC_CHANNELS_DISABLE_OVERRUN_INTERRUPT(pevc, chan_mask) \
  if((chan_mask)<=0x80000000)  (pevc)->ovidr0 = (chan_mask); \
  else  (pevc)->ovidr1 = (chan_mask);

/*!
 * \brief Check if the event overrun interrupt for a given channel is enabled.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      true if the channel trigger interrupt is enabled
 *                   false if the channel trigger interrupt is disabled
 */
extern bool pevc_channel_is_overrun_interrupt_enabled( volatile avr32_pevc_t *pevc, unsigned short int chan_id);

/*!
 * \brief Check if the event overrun interrupt was raised for a given channel.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 * \return bool      true if the channel trigger interrupt was raised
 *                   false otherwise
 */
extern bool pevc_channel_is_overrun_interrupt_raised( volatile avr32_pevc_t *pevc, unsigned short int chan_id);

/*!
 * \brief Clear the event overrun interrupt for a given channel.
 *
 * \param *pevc Base address of the PEVC
 * \param  chan_id   channel id
 */
extern void pevc_channel_clear_overrun_interrupt( volatile avr32_pevc_t *pevc, unsigned short int chan_id);
//! @}

/**
 * \}
 */

#endif  // _PEVC_H_
