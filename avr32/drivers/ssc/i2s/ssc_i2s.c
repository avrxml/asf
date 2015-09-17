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


#include "compiler.h"
#include "ssc_i2s.h"


/*! \brief Sets the clock divider (DIV-field in CMR-register)
 *
 *  This function sets the DIV field in the CMR-register to correct divider
 *  so that the divided clock runs at the specified rate.
 *
 *  \note Clock divider is always rounded down, this means that the desired
 *        clock will be correct or faster than desired clock rate.
 *
 *  \param ssc pointer to the correct volatile avr32_ssc_t struct
 *  \param bit_rate desired clock rate (bit rate)
 *  \param pba_hz PBA bus speed in Hz
 *
 *  \return Status or error code
 *    \retval SSC_I2S_OK on success
 *    \retval SSC_I2S_ERROR_ARGUMENT on invalid arguments
 */
static int set_clock_divider(volatile avr32_ssc_t *ssc,
                             unsigned int bit_rate,
                             unsigned int pba_hz)
{
  /*! \todo check input values */

  ssc->cmr = ((pba_hz + bit_rate) / (2 * bit_rate)) << AVR32_SSC_CMR_DIV_OFFSET;

  return SSC_I2S_OK;
}


void ssc_i2s_reset(volatile avr32_ssc_t *ssc)
{
  /* Software reset SSC */
  ssc->cr = AVR32_SSC_CR_SWRST_MASK;
}


int ssc_i2s_init(volatile avr32_ssc_t *ssc,
                 unsigned int sample_frequency,
                 unsigned int data_bit_res,
                 unsigned int frame_bit_res,
                 unsigned char mode,
                 unsigned int pba_hz)
{
  /*! \todo check input values */

  /* Reset */
  ssc_i2s_reset(ssc);

  if (mode == SSC_I2S_MODE_SLAVE_STEREO_OUT)
  {
    ssc->cmr = AVR32_SSC_CMR_DIV_NOT_ACTIVE << AVR32_SSC_CMR_DIV_OFFSET;

    ssc->tcmr = AVR32_SSC_TCMR_CKS_TK_PIN               << AVR32_SSC_TCMR_CKS_OFFSET    |
                AVR32_SSC_TCMR_CKO_INPUT_ONLY           << AVR32_SSC_TCMR_CKO_OFFSET    |
                0                                       << AVR32_SSC_TCMR_CKI_OFFSET    |
                AVR32_SSC_TCMR_CKG_NONE                 << AVR32_SSC_TCMR_CKG_OFFSET    |
                AVR32_SSC_TCMR_START_DETECT_ANY_EDGE_TF << AVR32_SSC_TCMR_START_OFFSET  |
                1                                       << AVR32_SSC_TCMR_STTDLY_OFFSET |
                0                                       << AVR32_SSC_TCMR_PERIOD_OFFSET;
#ifdef AVR32_SSC_220_H_INCLUDED
    ssc->tfmr = (data_bit_res - 1)             << AVR32_SSC_TFMR_DATLEN_OFFSET  |
                0                              << AVR32_SSC_TFMR_DATDEF_OFFSET  |
                1                              << AVR32_SSC_TFMR_MSBF_OFFSET    |
                (1 - 1)                        << AVR32_SSC_TFMR_DATNB_OFFSET   |
                0                              << AVR32_SSC_TFMR_FSLEN_OFFSET   |
                AVR32_SSC_TFMR_FSOS_INPUT_ONLY << AVR32_SSC_TFMR_FSOS_OFFSET    |
                0                              << AVR32_SSC_TFMR_FSDEN_OFFSET   |
                0                              << AVR32_SSC_TFMR_FSEDGE_OFFSET;
#else
    ssc->tfmr = (data_bit_res - 1)             << AVR32_SSC_TFMR_DATLEN_OFFSET  |
                0                              << AVR32_SSC_TFMR_DATDEF_OFFSET  |
                1                              << AVR32_SSC_TFMR_MSBF_OFFSET    |
                (1 - 1)                        << AVR32_SSC_TFMR_DATNB_OFFSET   |
                0                              << AVR32_SSC_TFMR_FSLEN_OFFSET   |
                AVR32_SSC_TFMR_FSOS_INPUT_ONLY << AVR32_SSC_TFMR_FSOS_OFFSET    |
                0                              << AVR32_SSC_TFMR_FSDEN_OFFSET   |
                0                              << AVR32_SSC_TFMR_FSEDGE_OFFSET  |
                0                              << AVR32_SSC_TFMR_FSLENHI_OFFSET;
#endif
    ssc->cr = AVR32_SSC_CR_TXEN_MASK;
  }
  else if (mode == SSC_I2S_MODE_SLAVE_STEREO_IN)
  {
    ssc->cmr = AVR32_SSC_CMR_DIV_NOT_ACTIVE << AVR32_SSC_CMR_DIV_OFFSET;
    ssc->tcmr = 0;
    ssc->tfmr = 0;
    ssc->rcmr = (AVR32_SSC_RCMR_CKS_RK_PIN << AVR32_SSC_RCMR_CKS_OFFSET) |
                (1                             << AVR32_SSC_RCMR_CKI_OFFSET)|
                (AVR32_SSC_RCMR_CKO_INPUT_ONLY << AVR32_SSC_RCMR_CKO_OFFSET) |
                (1                         << AVR32_SSC_RCMR_STTDLY_OFFSET ) |
                (AVR32_SSC_RCMR_START_DETECT_FALLING_RF << AVR32_SSC_RCMR_START_OFFSET)
    ;

#ifdef AVR32_SSC_220_H_INCLUDED
    ssc->rfmr = (data_bit_res - 1)                                 << AVR32_SSC_RFMR_DATLEN_OFFSET                              |
                1                                                  << AVR32_SSC_RFMR_MSBF_OFFSET                                |
                (1 - 1)                                            << AVR32_SSC_RFMR_DATNB_OFFSET                               |
                (((frame_bit_res - 1)                              << AVR32_SSC_RFMR_FSLEN_OFFSET) & AVR32_SSC_RFMR_FSLEN_MASK) |
                AVR32_SSC_RFMR_FSOS_INPUT_ONLY                     << AVR32_SSC_RFMR_FSOS_OFFSET                                |
                1                                                  << AVR32_SSC_RFMR_FSEDGE_OFFSET;
#else
    ssc->rfmr = (data_bit_res - 1)                                 << AVR32_SSC_RFMR_DATLEN_OFFSET                              |
                1                                                  << AVR32_SSC_RFMR_MSBF_OFFSET                                |
                (1 - 1)                                            << AVR32_SSC_RFMR_DATNB_OFFSET                               |
                (((frame_bit_res - 1)                              << AVR32_SSC_RFMR_FSLEN_OFFSET) & AVR32_SSC_RFMR_FSLEN_MASK) |
                AVR32_SSC_RFMR_FSOS_INPUT_ONLY                     << AVR32_SSC_RFMR_FSOS_OFFSET                                |
                1                                                  << AVR32_SSC_RFMR_FSEDGE_OFFSET                              |
                ((frame_bit_res - 1) >> AVR32_SSC_RFMR_FSLEN_SIZE) << AVR32_SSC_RFMR_FSLENHI_OFFSET;
#endif

    ssc->cr = AVR32_SSC_CR_RXEN_MASK;

  }
  else
  {
    // Possible states:
    // SSC_I2S_MODE_STEREO_OUT,
    // SSC_I2S_MODE_STEREO_OUT_EXT_CLK,
    // SSC_I2S_MODE_STEREO_OUT_MONO_IN,
    // SSC_I2S_MODE_RIGHT_IN,
    // SSC_I2S_MODE_STEREO_IN,
    // SSC_I2S_MODE_STEREO_OUT_STEREO_IN

    unsigned long txen_mask = 0x00000000,
                  rxen_mask = 0x00000000;

    if (mode != SSC_I2S_MODE_STEREO_OUT_EXT_CLK)
    {
      /* Set clock divider */
      set_clock_divider(ssc, 2 * sample_frequency * frame_bit_res, pba_hz);
    }

    /* SSC can only receive one channel of audio input.  In order
     * to use two inputs, two SSC's must be used. The first (master)
     * deals with both output channels and the left input, and should
     * be set to mode=SSC_I2S_MODE_STEREO_OUT_MONO_IN, and the second to
     * mode=SSC_I2S_MODE_RIGHT_IN. The second SSC should be connected to the
     * first SSC's in the following way:
     * master-ssc:TF -> slave-ssc:RF,
     * master-ssc:TK -> slave-ssc:RK
     */

    /* If only slave I2S SSC, do not set transmit registers */
    if (mode != SSC_I2S_MODE_RIGHT_IN)
    {
      // Possible states:
      // SSC_I2S_MODE_STEREO_OUT,
      // SSC_I2S_MODE_STEREO_OUT_EXT_CLK,
      // SSC_I2S_MODE_STEREO_OUT_MONO_IN,
      // SSC_I2S_MODE_STEREO_IN,
      // SSC_I2S_MODE_STEREO_OUT_STEREO_IN

      if (mode != SSC_I2S_MODE_STEREO_OUT_EXT_CLK)
      {
        // Possible states:
        // SSC_I2S_MODE_STEREO_OUT,
        // SSC_I2S_MODE_STEREO_OUT_MONO_IN,
        // SSC_I2S_MODE_STEREO_IN,
        // SSC_I2S_MODE_STEREO_OUT_STEREO_IN

        /* Set transmit clock mode:
         *   CKS - use divided clock,
         *   CKO - transmit continuous clock on TK
         *   CKI - shift data on falling clock
         *   CKG - transmit continuous clock on TK
         *   START - on falling TF(WS) edge
         *   STTDLY - TF toggles before last bit of last word, not before
         *            first bit on next word. Therefore: delay one cycle.
         *   PERIOD - generate framesync for each sample (FS is generated
         *            every (PERIOD + 1) * 2 clock)
         */
        ssc->tcmr = AVR32_SSC_TCMR_CKS_DIV_CLOCK              << AVR32_SSC_TCMR_CKS_OFFSET    |
                    AVR32_SSC_TCMR_CKO_CONTINOUS_CLOCK_OUTPUT << AVR32_SSC_TCMR_CKO_OFFSET    |
                    0                                         << AVR32_SSC_TCMR_CKI_OFFSET    |
                    AVR32_SSC_TCMR_CKG_NONE                   << AVR32_SSC_TCMR_CKG_OFFSET    |
                    AVR32_SSC_TCMR_START_DETECT_ANY_EDGE_TF   << AVR32_SSC_TCMR_START_OFFSET  |
                    1                                         << AVR32_SSC_TCMR_STTDLY_OFFSET |
                    (frame_bit_res - 1)                       << AVR32_SSC_TCMR_PERIOD_OFFSET;
      }
      else
      {
        // Possible states:
        // SSC_I2S_MODE_STEREO_OUT_EXT_CLK

        /* Set transmit clock mode:
         *   CKS - use RX clock,
         *   CKO - transmit continuous clock on TK
         *   CKI - shift data on falling clock
         *   CKG - transmit continuous clock on TK
         *   START - on falling TF(WS) edge
         *   STTDLY - TF toggles before last bit of last word, not before
         *            first bit on next word. Therefore: delay one cycle.
         *   PERIOD - generate framesync for each sample (FS is generated
         *            every (PERIOD + 1) * 2 clock)
         */
        ssc->tcmr = AVR32_SSC_TCMR_CKS_RK_CLOCK               << AVR32_SSC_TCMR_CKS_OFFSET    |
                    AVR32_SSC_TCMR_CKO_CONTINOUS_CLOCK_OUTPUT << AVR32_SSC_TCMR_CKO_OFFSET    |
                    0                                         << AVR32_SSC_TCMR_CKI_OFFSET    |
                    AVR32_SSC_TCMR_CKG_NONE                   << AVR32_SSC_TCMR_CKG_OFFSET    |
                    AVR32_SSC_TCMR_START_DETECT_ANY_EDGE_TF   << AVR32_SSC_TCMR_START_OFFSET  |
                    1                                         << AVR32_SSC_TCMR_STTDLY_OFFSET |
                    (frame_bit_res - 1)                       << AVR32_SSC_TCMR_PERIOD_OFFSET;
        /* Set receive clock mode:
         *  CKS - uses RK pin
         */
        ssc->rcmr = (AVR32_SSC_RCMR_CKS_RK_PIN << AVR32_SSC_RCMR_CKS_OFFSET);
      }

      /* Set transmit frame mode:
       *  DATLEN - one sample for one channel
       *  DATDEF - Default to zero,
       *  MSBF - transmit msb first,
       *  DATNB - Transfer two words (left+right),
       *  FSLEN - Frame sync is entire left channel
       *  FSOS - transmit negative pulse on WS (start sync on left channel)
       *  FSDEN - Do not use transmit frame sync data
       *  FSEDGE - detect frame sync positive edge
       */
#ifdef AVR32_SSC_220_H_INCLUDED
      ssc->tfmr = (data_bit_res - 1)                                 << AVR32_SSC_TFMR_DATLEN_OFFSET                              |
                  0                                                  << AVR32_SSC_TFMR_DATDEF_OFFSET                              |
                  1                                                  << AVR32_SSC_TFMR_MSBF_OFFSET                                |
                  (1 - 1)                                            << AVR32_SSC_TFMR_DATNB_OFFSET                               |
                  (((frame_bit_res - 1)                              << AVR32_SSC_TFMR_FSLEN_OFFSET) & AVR32_SSC_TFMR_FSLEN_MASK) |
                  AVR32_SSC_TFMR_FSOS_NEG_PULSE                      << AVR32_SSC_TFMR_FSOS_OFFSET                                |
                  0                                                  << AVR32_SSC_TFMR_FSDEN_OFFSET                               |
                  1                                                  << AVR32_SSC_TFMR_FSEDGE_OFFSET;
#else
      ssc->tfmr = (data_bit_res - 1)                                 << AVR32_SSC_TFMR_DATLEN_OFFSET                              |
                  0                                                  << AVR32_SSC_TFMR_DATDEF_OFFSET                              |
                  1                                                  << AVR32_SSC_TFMR_MSBF_OFFSET                                |
                  (1 - 1)                                            << AVR32_SSC_TFMR_DATNB_OFFSET                               |
                  (((frame_bit_res - 1)                              << AVR32_SSC_TFMR_FSLEN_OFFSET) & AVR32_SSC_TFMR_FSLEN_MASK) |
                  AVR32_SSC_TFMR_FSOS_NEG_PULSE                      << AVR32_SSC_TFMR_FSOS_OFFSET                                |
                  0                                                  << AVR32_SSC_TFMR_FSDEN_OFFSET                               |
                  1                                                  << AVR32_SSC_TFMR_FSEDGE_OFFSET                              |
                  ((frame_bit_res - 1) >> AVR32_SSC_TFMR_FSLEN_SIZE) << AVR32_SSC_TFMR_FSLENHI_OFFSET;
#endif
      txen_mask = AVR32_SSC_CR_TXEN_MASK;
    }

    /* If receiving data, set receiving clock register */
    if ( (mode != SSC_I2S_MODE_STEREO_OUT) && (mode != SSC_I2S_MODE_STEREO_OUT_EXT_CLK) )
    {
      // Possible states:
      // SSC_I2S_MODE_STEREO_OUT_MONO_IN,
      // SSC_I2S_MODE_RIGHT_IN,
      // SSC_I2S_MODE_STEREO_IN,
      // SSC_I2S_MODE_STEREO_OUT_STEREO_IN

      if ( (mode == SSC_I2S_MODE_STEREO_OUT_MONO_IN) || (mode == SSC_I2S_MODE_RIGHT_IN) )
      {
          /* Set receive clock mode:
           *  CKS - left ch uses TK, right (slave) uses RK pin
           *  CKO - No clock output,
           *  CKI - shift data on rising edge,
           *  CKG - No clock output,
           *  START - left ch starts when transmit starts, right starts on
           *          word-select (RF) rising edge
           *  STTDLY - RF toggles before last bit of last word, not before
           *           first bit on next word. Therefore, delay one cycle.
           *  PERIOD - No FS generation
           */
          ssc->rcmr =
            (( mode == SSC_I2S_MODE_RIGHT_IN ? AVR32_SSC_RCMR_CKS_RK_PIN : AVR32_SSC_RCMR_CKS_TK_CLOCK )
                                           << AVR32_SSC_RCMR_CKS_OFFSET)|
            (AVR32_SSC_RCMR_CKO_INPUT_ONLY << AVR32_SSC_RCMR_CKO_OFFSET)|
            (1                             << AVR32_SSC_RCMR_CKI_OFFSET)|
            (AVR32_SSC_RCMR_CKG_NONE       << AVR32_SSC_RCMR_CKG_OFFSET)|
            (( mode == SSC_I2S_MODE_RIGHT_IN ? AVR32_SSC_RCMR_START_DETECT_RISING_RF : AVR32_SSC_RCMR_START_TRANSMIT_START )
                                           << AVR32_SSC_RCMR_START_OFFSET)|
            (1                             << AVR32_SSC_RCMR_STTDLY_OFFSET)|
            (0                             << AVR32_SSC_RCMR_PERIOD_OFFSET);

          /* Set receive frame mode:
           *  DATLEN - bits per sample
           *  LOOP - no loopback
           *  MSBF - msb first
           *  DATNB - transmit one per sync
           *  FSLEN - no generate framesync
           *  FSOS - do not generate framesync
           *  FSEDGE - detect frame sync positive edge
           */
          ssc->rfmr =
            ((data_bit_res-1)               << AVR32_SSC_RFMR_DATLEN_OFFSET)|
            (0                              << AVR32_SSC_RFMR_LOOP_OFFSET)|
            (1                              << AVR32_SSC_RFMR_MSBF_OFFSET)|
            (0                              << AVR32_SSC_RFMR_DATNB_OFFSET)|
            (0                              << AVR32_SSC_RFMR_FSLEN_OFFSET)|
            (AVR32_SSC_RFMR_FSOS_INPUT_ONLY << AVR32_SSC_RFMR_FSOS_OFFSET)|
            (0                              << AVR32_SSC_RFMR_FSEDGE_OFFSET);

            rxen_mask = AVR32_SSC_CR_RXEN_MASK;
        }
        else
        {
            // Possible states:
            // SSC_I2S_MODE_STEREO_IN,
            // SSC_I2S_MODE_STEREO_OUT_STEREO_IN

            ssc->rcmr = AVR32_SSC_RCMR_CKS_TK_CLOCK               << AVR32_SSC_RCMR_CKS_OFFSET    |
                        AVR32_SSC_RCMR_CKO_CONTINOUS_CLOCK_OUTPUT << AVR32_SSC_RCMR_CKO_OFFSET    |
                        0                                         << AVR32_SSC_RCMR_CKI_OFFSET    |
                        AVR32_SSC_RCMR_CKG_NONE                   << AVR32_SSC_RCMR_CKG_OFFSET    |
                        AVR32_SSC_RCMR_START_DETECT_ANY_EDGE_RF   << AVR32_SSC_RCMR_START_OFFSET  |
                        1                                         << AVR32_SSC_RCMR_STTDLY_OFFSET |
                        (frame_bit_res - 1)                       << AVR32_SSC_RCMR_PERIOD_OFFSET;

#ifdef AVR32_SSC_220_H_INCLUDED
            ssc->rfmr = (data_bit_res - 1)                                 << AVR32_SSC_RFMR_DATLEN_OFFSET                              |
                        1                                                  << AVR32_SSC_RFMR_MSBF_OFFSET                                |
                        (1 - 1)                                            << AVR32_SSC_RFMR_DATNB_OFFSET                               |
                        (((frame_bit_res - 1)                              << AVR32_SSC_RFMR_FSLEN_OFFSET) & AVR32_SSC_TFMR_FSLEN_MASK) |
                        AVR32_SSC_RFMR_FSOS_NEG_PULSE                      << AVR32_SSC_RFMR_FSOS_OFFSET                                |
                        1                                                  << AVR32_SSC_RFMR_FSEDGE_OFFSET;
#else
            ssc->rfmr = (data_bit_res - 1)                                 << AVR32_SSC_RFMR_DATLEN_OFFSET                              |
                        1                                                  << AVR32_SSC_RFMR_MSBF_OFFSET                                |
                        (1 - 1)                                            << AVR32_SSC_RFMR_DATNB_OFFSET                               |
                        (((frame_bit_res - 1)                              << AVR32_SSC_RFMR_FSLEN_OFFSET) & AVR32_SSC_TFMR_FSLEN_MASK) |
                        AVR32_SSC_RFMR_FSOS_NEG_PULSE                      << AVR32_SSC_RFMR_FSOS_OFFSET                                |
                        1                                                  << AVR32_SSC_RFMR_FSEDGE_OFFSET                              |
                        ((frame_bit_res - 1) >> AVR32_SSC_RFMR_FSLEN_SIZE) << AVR32_SSC_RFMR_FSLENHI_OFFSET;
#endif
            rxen_mask = AVR32_SSC_CR_RXEN_MASK;
        }

      }
    /* Enable transceiver and/or receiver */
    ssc->cr = txen_mask | rxen_mask;
  }

  return SSC_I2S_OK;
}


int ssc_i2s_transfer(volatile avr32_ssc_t *ssc, unsigned int data)
{
  unsigned int timeout = SSC_I2S_TIMEOUT_VALUE;

  while( ( ssc->sr & (1<<AVR32_SSC_SR_TXRDY_OFFSET) ) == 0 &&
      timeout > 0 ) {
    timeout--;
  }

  if( timeout <= 0 ) {
    return SSC_I2S_TIMEOUT;
  }

  ssc->thr = data;

  return SSC_I2S_OK;
}


void ssc_i2s_disable_interrupts(volatile avr32_ssc_t *ssc, unsigned long int_mask)
{
  bool global_interrupt_enabled = cpu_irq_is_enabled();

  if (global_interrupt_enabled) cpu_irq_disable();
  ssc->idr = int_mask;
  ssc->sr;
  if (global_interrupt_enabled) cpu_irq_enable();
}


void ssc_i2s_enable_interrupts(volatile avr32_ssc_t *ssc, unsigned long int_mask)
{
  ssc->ier = int_mask;
}


unsigned long ssc_i2s_get_status(volatile avr32_ssc_t *ssc)
{
  return ssc->sr;
}
