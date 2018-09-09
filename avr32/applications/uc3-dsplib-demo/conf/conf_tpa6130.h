/*****************************************************************************
 *
 * \file
 *
 * \brief TPA6130A2 amplifier driver configuration file for UC3.
 *
 * This file contains the possible external configuration of the TPA6130A2.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "board.h"

#include "board.h"

#include "tpa6130.h"
#include <avr32/io.h>


#ifndef _CONF_TPA6130_H_
#define _CONF_TPA6130_H_

//!
#define TPA6130_MODE  TPA6130_MODE_STEREO

//! Input Selected: \c AIC23B_INPUT_LINE or \c AIC23B_INPUT_MIC
#define TPA6130_SIG TPA6130_SIG_DIFF

/*! \name Audio ABDAC Sampling Configuration
 */
//! @{

// APPLICATION MAX VOLUME 0x3F = 63 = 4dB, minimum is 0x0 = -59dB.
#define TPA6130_APPLICATION_MAX_VOLUME (TPA6130_VOL_MAX/2)

#define TPA6130_ABDAC_GCLK_INPUT_HZ   FOSC1
//! @}

#define TPA6130_TWI_MASTER_SPEED  100000

#define BUFFER_LENGTH		256
#define BUFFER_LENGTH_LOG	8

/* We only use "low" frequencies, so no need to display the high half */
#define FFT_LENGTH		BUFFER_LENGTH/4

#define FIR_NUM_COEF		32

#define DAC_SAMPLING_RATE		48000
#define DAC_NUM_CHANNELS	2
#define DAC_BITS_PER_SAMPLE	16
#define DAC_SWAP_CHANNELS	false

#define DEFAULT_OUTPUT_VOLUME	16384


#endif  // _CONF_TLV320AIC23B_H_
