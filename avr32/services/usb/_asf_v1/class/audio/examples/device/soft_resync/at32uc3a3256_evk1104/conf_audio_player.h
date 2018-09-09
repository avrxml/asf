/**************************************************************************
 *
 * \file
 *
 * \brief Audio player configuration file.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_AUDIO_PLAYER_H_
#define _CONF_AUDIO_PLAYER_H_

#include "audio_mixer.h"

#include <avr32/io.h>
#include "board.h"


/*! \name System Clock Frequencies
 */
//! @{
#define SYS_CLOCK_PLL_MUL             11
#define FMCK_HZ                       ((SYS_CLOCK_PLL_MUL * FOSC1 + 1) / 2)
#define FCPU_HZ                       FMCK_HZ
#define FHSB_HZ                       FCPU_HZ
#define FPBB_HZ                       FMCK_HZ
#define FPBA_HZ                       FMCK_HZ
//! @}

//! Default heap initialization word.
#define DEFAULT_HEAP_INIT_WORD        0xA5A5A5A5

//! Time-out period of the WatchDog Timer in microseconds.
//!
//! \warning Do not choose a value smaller than the normal maximal time that can
//!          be spent between two consecutive calls of com_task().
#define WDT_TIMEOUT_PERIOD_US                   15000000

/*! \name Default DAC Settings
 * Please choose between:
 * - AUDIO_MIXER_DAC_AIC23B (external DAC mounted on EVK1105 board)
 * - AUDIO_MIXER_DAC_ABDAC  (internal ABDAC with external amplifier tap6130
 *   on EVK1105 and EVK1104)
 */
//! @{
#define DEFAULT_DACS                  AUDIO_MIXER_DAC_ABDAC
#define DEFAULT_DAC_SAMPLE_RATE_HZ    SPEAKER_FREQUENCY
#define DEFAULT_DAC_NUM_CHANNELS      2
#define DEFAULT_DAC_BITS_PER_SAMPLE   16
#define DEFAULT_DAC_SWAP_CHANNELS     false
//! @}

/*! \name USB resynchronization methods
 */
//! @{
//! Add/remove samples on the fly.
#define USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES  1

//! Pseudo adaptive Sampling Rate Conversion.
#define USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC      2

//! Use of an external clock synthesizer (e.g. the CL CS2200).
#define USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER 8
//! @}

/*! \name Method used for the USB resynchronization
 * Please choose between:
 * - USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES  (Add/remove samples on the fly)
 * - USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC      (Pseudo adaptive Sampling Rate Conversion)
 * - USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER (Use of an external clock synthesizer (e.g. the CL CS2200))
 */
#define USB_RESYNC_METHOD  USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES

#endif  // _CONF_AUDIO_PLAYER_H_
