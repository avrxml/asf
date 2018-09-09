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
