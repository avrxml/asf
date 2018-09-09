/*****************************************************************************
 *
 * \file
 *
 * \brief TPA6130A2 amplifier driver configuration file for UC3.
 *
 * This file contains the possible external configuration of the TPA6130A2.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
# define TPA6130_ABDAC_PDCA_INT_LEVEL AVR32_INTC_INT1
# warning The TPA6130 ABDAC setups configuration to use in the driver is missing. Default configuration is used.
#endif
//! @}

/*! \name TWI Default Configuration
 */
//! @{
#if !defined(TPA6130_TWI)              || \
    !defined(TPA6130_TWI_SCL_PIN)      || \
    !defined(TPA6130_TWI_SCL_FUNCTION) || \
    !defined(TPA6130_TWI_SDA_PIN)      || \
    !defined(TPA6130_TWI_SDA_FUNCTION)
# define TPA6130_TWI                  (&AVR32_TWIM0)
# define TPA6130_TWI_IRQ              AVR32_TWIM0_IRQ
# define TPA6130_TWI_SCL_PIN          AVR32_TWIMS0_TWCK_0_0_PIN
# define TPA6130_TWI_SCL_FUNCTION     AVR32_TWIMS0_TWCK_0_0_FUNCTION
# define TPA6130_TWI_SDA_PIN          AVR32_TWIMS0_TWD_0_0_PIN
# define TPA6130_TWI_SDA_FUNCTION     AVR32_TWIMS0_TWD_0_0_FUNCTION
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
