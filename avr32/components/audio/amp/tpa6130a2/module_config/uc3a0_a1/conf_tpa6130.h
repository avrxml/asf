/*****************************************************************************
 *
 * \file
 *
 * \brief TPA6130A2 amplifier driver configuration file for UC3.
 *
 * This file contains the possible external configuration of the TPA6130A2.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


 //! Configuration of the TPA6130 amplifier driver.

#include <avr32/io.h>
#include "board.h"
#include "tpa6130.h"

#ifndef _CONF_TPA6130_H_
#define _CONF_TPA6130_H_

/*! \name ABDAC Default Configuration
 */
//! @{
#if !defined(TPA6130_ABDAC)              || \
    !defined(TPA6130_DATA0_PIN)          || \
    !defined(TPA6130_DATA0_FUNCTION)     || \
    !defined(TPA6130_DATAN0_PIN)         || \
    !defined(TPA6130_DATAN0_FUNCTION)    || \
    !defined(TPA6130_DATA1_PIN)          || \
    !defined(TPA6130_DATA1_FUNCTION)     || \
    !defined(TPA6130_DATAN1_PIN)         || \
    !defined(TPA6130_DATAN1_FUNCTION)    || \
    !defined(TPA6130_ABDAC_PDCA_PID)     || \
    !defined(TPA6130_ABDAC_PDCA_CHANNEL) || \
    !defined(TPA6130_ABDAC_PDCA_IRQ)     || \
    !defined(TPA6130_ABDAC_PDCA_INT_LEVEL)
# define TPA6130_ABDAC                (&AVR32_ABDAC)
# define TPA6130_DATA0_PIN            AVR32_ABDAC_DATA_0_1_PIN
# define TPA6130_DATA0_FUNCTION       AVR32_ABDAC_DATA_0_1_FUNCTION
# define TPA6130_DATAN0_PIN           AVR32_ABDAC_DATAN_0_1_PIN
# define TPA6130_DATAN0_FUNCTION      AVR32_ABDAC_DATAN_0_1_FUNCTION
# define TPA6130_DATA1_PIN            AVR32_ABDAC_DATA_1_1_PIN
# define TPA6130_DATA1_FUNCTION       AVR32_ABDAC_DATA_1_1_FUNCTION
# define TPA6130_DATAN1_PIN           AVR32_ABDAC_DATAN_1_1_PIN
# define TPA6130_DATAN1_FUNCTION      AVR32_ABDAC_DATAN_1_1_FUNCTION
# define TPA6130_ABDAC_PDCA_PID       AVR32_PDCA_PID_ABDAC_TX
# define TPA6130_ABDAC_PDCA_CHANNEL   0
# define TPA6130_ABDAC_PDCA_IRQ       AVR32_PDCA_IRQ_0
# define TPA6130_ABDAC_PDCA_INT_LEVEL AVR32_INTC_INT3
# warning The TPA6130 ABDAC setups configuration to use in the driver is missing. Default configuration is used.
#endif
//! @}

/*! \name TWI Default Configuration
 */
//! @{
#if !defined(TPA6130_TWI)             || \
    !defined(TPA6130_TWI_SCL_PIN)     || \
    !defined(TPA6130_TWI_SCL_FUNCTION)|| \
    !defined(TPA6130_TWI_SDA_PIN)     || \
    !defined(TPA6130_TWI_SDA_FUNCTION) 
# define TPA6130_TWI                  (&AVR32_TWI)
# define TPA6130_TWI_SCL_PIN          AVR32_TWI_SCL_0_0_PIN
# define TPA6130_TWI_SCL_FUNCTION     AVR32_TWI_SCL_0_0_FUNCTION
# define TPA6130_TWI_SDA_PIN          AVR32_TWI_SDA_0_0_PIN
# define TPA6130_TWI_SDA_FUNCTION     AVR32_TWI_SDA_0_0_FUNCTION
# warning The TPA6130 TWI setups configuration to use in the driver is missing. Default configuration is used.
#endif
//! @}

//!
#define TPA6130_MODE  TPA6130_MODE_STEREO

//! Input Selected: \c AIC23B_INPUT_LINE or \c AIC23B_INPUT_MIC
#define TPA6130_SIG TPA6130_SIG_DIFF

/*! \name Audio ABDAC Sampling Configuration
 */
//! @{
#define DEFAULT_DAC_SAMPLE_RATE_HZ    (11025)
#define DEFAULT_DAC_NUM_CHANNELS      2           //!< Number of channels: 2 in stereo mode
#define DEFAULT_DAC_BITS_PER_SAMPLE   16          //!< Bits per sample: 16
#define DEFAULT_DAC_SWAP_CHANNELS     false       //!< Swap of channel

// APPLICATION MAX VOLUME 0x3F = 63 = 4dB, minimum is 0x0 = -59dB.
#define TPA6130_APPLICATION_MAX_VOLUME (TPA6130_VOL_MAX/2)

#define TPA6130_ABDAC_GCLK_INPUT_HZ   FOSC0
//! @}
#define TPA6130_TWI_MASTER_SPEED  100000

#endif  // _CONF_TPA6130_H_
