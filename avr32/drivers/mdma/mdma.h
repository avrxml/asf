/*****************************************************************************
 *
 * \file
 *
 * \brief MDMA driver for AVR32 UC3.
 *
 * AVR32 MDMA driver module.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _MDMA_H_
#define _MDMA_H_

/**
 * \defgroup group_avr32_drivers_mdma MEMORY - MDMA - Memory DMA Controller
 *
 * This driver interfaces the MDMA (Memory Direct Memory Access Controller) which performs
 * memory-to-memory transfers with as little CPU intervention as possible.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"

//! MDMA driver functions return value in case of invalid argument(s).
#define MDMA_INVALID_ARGUMENT                     (-1)

/*! \name Return Values
 */
//! @{
#define MDMA_SUCCESS                 0 //!< Successful completion.
#define MDMA_FAILURE                -1 //!< Failure because of some unspecified reason.
//! @}

//! Default time-out value (number of attempts).
#define MDMA_DEFAULT_TIMEOUT         10000


/*! \name MDMA Transfer Size
 */
//! @{
#define MDMA_TRANSFERT_SIZE_BYTE                   (0)
#define MDMA_TRANSFERT_SIZE_HLFW                   (1)
#define MDMA_TRANSFERT_SIZE_WORD                   (2)
//! @}

/*! \name MDMA Channel Mode
 */
//! @{
#define MDMA_SINGLE_TRANSFERT_MODE                 (0)
#define MDMA_DESCRIPTOR_MODE                       (1)
//! @}

/*! \name MDMA Arbitration Mode
 */
//! @{
#define MDMA_FIXED_PRIORITY_MODE                   (0)
#define MDMA_ROUND_ROBIN_MODE                      (1)
//! @}

/*! \name MDMA Burst Transfer Size
 */
//! @{
#define MDMA_SINGLE_TRANSFER_BURST                 (0)
#define MDMA_4_BEAT_BURST                          (1)
#define MDMA_8_BEAT_BURST                          (2)
#define MDMA_16_BEAT_BURST                         (3)
//! @}

//! MDMA Option Settings.
typedef struct
{
  unsigned int                     :6;

  //! Byte Swap
  unsigned int bswp                :2;

  unsigned int                     :1;

  //! Last Descriptor In Chain
  unsigned int L                   :1;

  //! Descriptor Valid
  unsigned int V                   :1;

  //! Transfer Channel Interrupt Enable
  unsigned int  tc_ienable         :1;

  //! Transfer Burst Size
  unsigned int burst_size          :2 ;

  //! Transfer Size
  unsigned int size                :2;

  //! Transfer Count
  unsigned int count               :16;

} mdma_opt_t;

//! MDMA Descriptor Structure Definition.
typedef struct
{
  //! Source Pointer
  U32                         src;

  //! Destination Pointer
  U32                         dest;

  //! Channel Control Register
  mdma_opt_t                  ccr;

  //! Unused Data
  unsigned int                :32;
} mdma_descriptor_t;

//! MDMA interrupts.
typedef struct
{
  unsigned int                      :20;
  //! Channel Bus Error
  unsigned int berr3                :1;
  unsigned int berr2                :1;
  unsigned int berr1                :1;
  unsigned int berr0                :1;

  unsigned int                      :4;

  //! Channel Complete
  unsigned int ch3c                 :1;
  unsigned int ch2c                 :1;
  unsigned int ch1c                 :1;
  unsigned int ch0c                 :1;

} mdma_interrupt_t;

/*! \brief MDMA interrupt settings.
 *
 * \param mdma             Pointer to the MDMA instance to access.
 *
 * \retval >=0             The interrupt enable configuration organized according to \ref mdma_interrupt_t.
 * \retval                 Interrupt Settings.
 */
extern unsigned long mdma_get_interrupt_settings(volatile avr32_mdma_t *mdma);

/*! \brief Enables various MDMA interrupts.
 *
 * \param mdma              Pointer to the MDMA instance to access.
 * \param bitfield          The interrupt enable configuration.
 */
extern void mdma_configure_interrupts(volatile avr32_mdma_t *mdma, const mdma_interrupt_t *bitfield);

/*! \brief Init a Single Mode Transfer for a Channel.
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channel          MDMA Channel To configure
 * \param opt              Pointer to the MDMA option.
 * \param src              Pointer to the source buffer.
 * \param dest             Pointer to the destination buffer.
 *
 */
extern void mdma_single_mode_xfert_init(volatile avr32_mdma_t *mdma, U32 channel, const mdma_opt_t *opt, U32* src, U32* dest);


/*! \brief Init a Descriptor Mode Transfer for a Channel.
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channel          MDMA Channel To configure
 * \param start_adress     Start address of the buffer
 *
 */
extern void mdma_descriptor_mode_xfert_init(volatile avr32_mdma_t *mdma, U32 channel, U32* start_adress);

/*! \brief Start a Single DMA Transfer for a Channel.
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channels         MDMA Channel To configure
 * \param arbitration      Pointer to the MDMA option.
 *
 */
extern void mdma_start_single_xfert(volatile avr32_mdma_t *mdma, U32 channels, U8 arbitration);

/*! \brief Start a Descriptor DMA Transfer for a Channel.
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channels         MDMA Channel To configure
 * \param arbitration      Pointer to the MDMA option.
 *
 */
extern void mdma_start_descriptor_xfert(volatile avr32_mdma_t *mdma, U32 channels, U8 arbitration);

/*! \brief Stop a DMA Transfer for a Channel
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channels         MDMA Channel To configure
 *
 * \retval                 MDMA_FAILURE Timeout has expired/ MDMA_SUCCESS success.
 */
extern int mdma_stop_channels(volatile avr32_mdma_t *mdma, U32 channels);

/*! \brief Check if a DMA Transfer for a Channel is enable
 *
 * \param mdma             Pointer to the MDMA instance to access.
 * \param channels         MDMA Channel to check
 *
 * \retval                 MDMA_SUCCESS if channel is enable.
 * \retval                 MDMA_FAILURE if channel is disable.
 */
extern int mdma_channels_is_enable(volatile avr32_mdma_t *mdma, U32 channels);

/**
 * \}
 */

#endif //_MDMA_H_
