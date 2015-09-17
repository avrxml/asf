/*****************************************************************************
 *
 * \file
 *
 * \brief SSC I2S example driver.
 *
 * This file defines a useful set of functions for the SSC I2S interface on
 * AVR32 devices. The driver handles normal polled usage and direct memory
 * access (PDC) usage.
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


#ifndef _SSC_I2S_H_
#define _SSC_I2S_H_

/**
 * \defgroup group_avr32_drivers_ssc_i2s SSC - Synchronous Serial Controller (I2S)
 *
 * Driver for the I2S protocol using the SSC (Synchronous Serial Controller).
 * The Atmel SSC provides a synchronous communication link with external devices.
 * It supports several serial synchronous communication protocols generally used in audio
 * and telecom applications such as I2S, Short Frame Sync, Long Frame Sync, etc.
 *
 * \{
 */

#include <avr32/io.h>

#ifdef AVR32_SSC_330_H_INCLUDED
#define AVR32_SSC_TCMR_START_DETECT_ANY_EDGE_TF           0x7
#endif


#define SSC_I2S_TIMEOUT_VALUE   10000



//! Error codes used by SSC I2S driver.
enum
{
  SSC_I2S_ERROR = -1,
  SSC_I2S_OK = 0,
  SSC_I2S_TIMEOUT = 1,
  SSC_I2S_ERROR_ARGUMENT,
  SSC_I2S_ERROR_RX,
  SSC_I2S_ERROR_TX
};

//! SSC I2S modes.
enum
{
  SSC_I2S_MODE_STEREO_OUT = 1,      //!< Two output channels.
  SSC_I2S_MODE_STEREO_OUT_EXT_CLK,  //!< Two output channels sampled with an external clock received from the SSC_RX_CLOCK line.
  SSC_I2S_MODE_SLAVE_STEREO_OUT,    //!< Two output channels controlled by the DAC.
  SSC_I2S_MODE_SLAVE_STEREO_IN,     //!< Two input channels controlled by the DAC.
  SSC_I2S_MODE_STEREO_OUT_MONO_IN,  //!< Two output, one input channel.
  SSC_I2S_MODE_RIGHT_IN,            //!< Right channel in. Used because one SSC only can manage one input channel at a time.
  SSC_I2S_MODE_STEREO_IN,           //!< Two input channels.
  SSC_I2S_MODE_STEREO_OUT_STEREO_IN //!< Two output channels and two input channels
};


/*! \brief Resets the SSC module
 *
 *  \param ssc pointer to the correct volatile avr32_ssc_t struct
 */
extern void ssc_i2s_reset(volatile avr32_ssc_t *ssc);

/*! \brief Sets up registers and initializes SSC for use as I2S.
 *
 *  \param ssc Pointer to the correct volatile avr32_ssc_t struct
 *  \param sample_frequency The sample frequency given in Hz
 *  \param data_bit_res Number of significant data bits in an I2S channel frame
 *  \param frame_bit_res Total number of bits in an I2S channel frame
 *  \param mode I2S-mode
 *    \arg SSC_I2S_MODE_STEREO_OUT
 *    \arg SSC_I2S_MODE_STEREO_OUT_EXT_CLK
 *    \arg SSC_I2S_MODE_SLAVE_STEREO_OUT
 *    \arg SSC_I2S_MODE_STEREO_OUT_MONO_IN
 *    \arg SSC_I2S_MODE_RIGHT_IN
 *    \arg SSC_I2S_MODE_STEREO_IN
 *    \arg SSC_I2S_MODE_STEREO_OUT_STEREO_IN
 *  \param pba_hz The clock speed of the PBA bus in Hz.
 *
 *  \return Status
 *    \retval SSC_I2S_OK when no error occurred.
 *    \retval SSC_I2S_ERROR_ARGUMENT when invalid arguments are passed
 */
extern int ssc_i2s_init(volatile avr32_ssc_t *ssc,
                        unsigned int sample_frequency,
                        unsigned int data_bit_res,
                        unsigned int frame_bit_res,
                        unsigned char mode,
                        unsigned int pba_hz);

/*! \brief Transfers a single message of data
 *
 *  \param ssc Pointer to the correct volatile avr32_ssc_t struct
 *  \param data The data to transfer
 *
 *  \return Status
 *    \retval SSC_I2S_OK when no error occurred.
 *    \retval SSC_I2S_TIMEOUT when a timeout occurred while trying to transfer
 */
extern int ssc_i2s_transfer(volatile avr32_ssc_t *ssc, unsigned int data);

/*! \brief Disables the specified SSC interrupts.
 *
 * \param ssc Base address of the SSC instance.
 * \param int_mask Bit-mask of SSC interrupts (\c AVR32_SSC_IDR_x_MASK).
 */
extern void ssc_i2s_disable_interrupts(volatile avr32_ssc_t *ssc, unsigned long int_mask);

/*! \brief Enables the specified SSC interrupts.
 *
 * \param ssc Base address of the SSC instance.
 * \param int_mask Bit-mask of SSC interrupts (\c AVR32_SSC_IER_x_MASK).
 */
extern void ssc_i2s_enable_interrupts(volatile avr32_ssc_t *ssc, unsigned long int_mask);

/*! \brief Returns the SSC status.
 *
 * \param ssc Base address of the SSC instance.
 *
 * \return The SSC Status Register.
 */
extern unsigned long ssc_i2s_get_status(volatile avr32_ssc_t *ssc);

/**
 * \}
 */

#endif  // _SSC_I2S_H_
